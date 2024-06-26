// -*- C++ -*-
//
// Package:    EventFilter/L1TRawToDigi
// Class:      L1TDigiToRaw
//
/**\class L1TDigiToRaw L1TDigiToRaw.cc EventFilter/L1TRawToDigi/plugins/L1TDigiToRaw.cc

 Description: [one line class summary]

 Implementation:
     [Notes on implementation]
*/
//
// Original Author:  Matthias Wolf
//         Created:  Mon, 10 Feb 2014 14:29:40 GMT
//
//

// system include files
#include <iomanip>
#include <memory>

#define EDM_ML_DEBUG 1

// user include files
#include "DataFormats/FEDRawData/interface/FEDNumbering.h"
#include "DataFormats/FEDRawData/interface/FEDRawDataCollection.h"
#include "DataFormats/FEDRawData/interface/FEDHeader.h"
#include "DataFormats/FEDRawData/interface/FEDTrailer.h"

#include "FWCore/Framework/interface/ConsumesCollector.h"
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/stream/EDProducer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/Utilities/interface/CRC16.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "FWCore/Utilities/interface/InputTag.h"

#include "EventFilter/L1TRawToDigi/interface/AMC13Spec.h"

#include "PackingSetupFactory.h"

namespace l1t {
  class L1TDigiToRaw : public edm::stream::EDProducer<> {
  public:
    explicit L1TDigiToRaw(const edm::ParameterSet&);

    static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);

  private:
    void produce(edm::Event&, const edm::EventSetup&) override;

    // ----------member data ---------------------------
    int evtType_;
    int fedId_;
    unsigned fwId_;

    // header and trailer sizes in chars
    int slinkHeaderSize_;
    int slinkTrailerSize_;

    std::unique_ptr<PackingSetup> setup_;
    std::unique_ptr<PackerTokens> tokens_;

    bool ctp7_mode_;
  };
}  // namespace l1t

namespace l1t {
  L1TDigiToRaw::L1TDigiToRaw(const edm::ParameterSet& config)
      : fedId_(config.getParameter<int>("FedId")), ctp7_mode_(config.getUntrackedParameter<bool>("CTP7")) {
    // Register products
    produces<FEDRawDataCollection>();

    fwId_ = config.getParameter<unsigned int>("FWId");
    evtType_ = config.getUntrackedParameter<int>("eventType", 1);

    auto cc = edm::ConsumesCollector(consumesCollector());

    setup_ = PackingSetupFactory::get()->make(config.getParameter<std::string>("Setup"));
    tokens_ = setup_->registerConsumes(config, cc);

    slinkHeaderSize_ = config.getUntrackedParameter<int>("lenSlinkHeader", 8);
    slinkTrailerSize_ = config.getUntrackedParameter<int>("lenSlinkTrailer", 8);
  }

  // ------------ method called to produce the data  ------------
  void L1TDigiToRaw::produce(edm::Event& event, const edm::EventSetup& setup) {
    using namespace edm;

    LogDebug("L1T") << "Packing data with FED ID " << fedId_;

    amc13::Packet amc13;

    auto bxId = event.bunchCrossing();
    // Note: L1ID != event ID
    // See e.g. triggerCount vs. eventNumber
    // in EventFilter/FEDInterface/interface/FED1024.h
    // But the trigger count is not stored in cmssw event class
    auto evtId = event.id().event();
    auto orbit = event.eventAuxiliary().orbitNumber();
    LogDebug("L1T") << "Forming FED with metadata bxId=" << bxId << ", l1ID=" << evtId << ", orbit=" << orbit;

    // Create all the AMC payloads to pack into the AMC13
    for (const auto& item : setup_->getPackers(fedId_, fwId_)) {
      auto amc_no = item.first.first;
      auto board = item.first.second;
      auto packers = item.second;

      Blocks block_load;
      for (const auto& packer : packers) {
        LogDebug("L1T") << "Adding packed blocks";
        packer->setBoard(board);
        auto blocks = packer->pack(event, tokens_.get());
        block_load.insert(block_load.end(), blocks.begin(), blocks.end());
      }

      std::sort(block_load.begin(), block_load.end());

      LogDebug("L1T") << "Concatenating blocks";

      std::vector<uint32_t> load32;
      // CTP7 stores this info in AMC user header
      if (not ctp7_mode_) {
        // TODO Infrastructure firmware version.  Currently not used.
        // Would change the way the payload has to be unpacked.
        load32.push_back(0);
        load32.push_back(fwId_);
      }
      for (const auto& block : block_load) {
        LogDebug("L1T") << "Adding block " << block.header().getID() << " with size " << block.payload().size();
        auto load = block.payload();

#ifdef EDM_ML_DEBUG
        std::stringstream s("");
        s << "Block content:" << std::endl << std::hex << std::setfill('0');
        for (const auto& word : load)
          s << std::setw(8) << word << std::endl;
        LogDebug("L1T") << s.str();
#endif

        if (block.header().getType() == CTP7) {
          // Header is two words for CTP7, first word is just a magic
          load32.push_back(0xA110CA7E);
        }
        load32.push_back(block.header().raw());
        load32.insert(load32.end(), load.begin(), load.end());
      }

      LogDebug("L1T") << "Converting payload";

      std::vector<uint64_t> load64;
      for (unsigned int i = 0; i < load32.size(); i += 2) {
        uint64_t word = load32[i];
        if (i + 1 < load32.size()) {
          word |= static_cast<uint64_t>(load32[i + 1]) << 32;
        } else {
          word |= static_cast<uint64_t>(0xffffffff) << 32;
        }
        load64.push_back(word);
      }

      LogDebug("L1T") << "Creating AMC packet";

      unsigned amc_user_header = 0;
      if (ctp7_mode_)
        amc_user_header = fwId_;
      amc13.add(amc_no, board, evtId, orbit, bxId, load64, amc_user_header);
    }

    std::unique_ptr<FEDRawDataCollection> raw_coll(new FEDRawDataCollection());
    FEDRawData& fed_data = raw_coll->FEDData(fedId_);

    unsigned int size = slinkHeaderSize_ + slinkTrailerSize_ + amc13.size() * 8;
    fed_data.resize(size);
    unsigned char* payload = fed_data.data();
    unsigned char* payload_start = payload;

    FEDHeader header(payload);
    header.set(payload, evtType_, evtId, bxId, fedId_);

    amc13.write(event, payload, slinkHeaderSize_, size - slinkHeaderSize_ - slinkTrailerSize_);

    payload += slinkHeaderSize_;
    payload += amc13.size() * 8;

    FEDTrailer trailer(payload);
    trailer.set(payload, size / 8, evf::compute_crc(payload_start, size), 0, 0);

    event.put(std::move(raw_coll));
  }

  // ------------ method called when starting to processes a run  ------------
  /*
   void
   L1TDigiToRaw::beginRun(edm::Run const&, edm::EventSetup const&)
   {
   }
   */

  // ------------ method called when ending the processing of a run  ------------
  /*
   void
   L1TDigiToRaw::endRun(edm::Run const&, edm::EventSetup const&)
   {
   }
   */

  // ------------ method called when starting to processes a luminosity block  ------------
  /*
   void
   L1TDigiToRaw::beginLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&)
   {
   }
   */

  // ------------ method called when ending the processing of a luminosity block  ------------
  /*
   void
   L1TDigiToRaw::endLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&)
   {
   }
   */

  // ------------ method fills 'descriptions' with the allowed parameters for the module  ------------
  void L1TDigiToRaw::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
    edm::ParameterSetDescription desc;
    desc.add<unsigned int>("FWId", -1);
    desc.add<int>("FedId");
    desc.addUntracked<int>("eventType", 1);
    desc.add<std::string>("Setup");
    desc.addOptional<edm::InputTag>("InputLabel", edm::InputTag(""));
    desc.addOptional<edm::InputTag>("InputLabel2", edm::InputTag(""));
    desc.addUntracked<int>("lenSlinkHeader", 8);
    desc.addUntracked<int>("lenSlinkTrailer", 8);
    desc.addUntracked<bool>("CTP7", false);

    PackingSetupFactory::get()->fillDescription(desc);

    descriptions.add("l1tDigiToRaw", desc);
  }
}  // namespace l1t

using namespace l1t;
//define this as a plug-in
DEFINE_FWK_MODULE(L1TDigiToRaw);
