/****************************************************************************
 * Authors:
 *   Wagner Carvalho wcarvalh@cern.ch
 *   Jan Kašpar
 ****************************************************************************/

#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/Framework/interface/SourceFactory.h"
#include "FWCore/Framework/interface/ModuleFactory.h"

#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/Framework/interface/ESProducer.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"

#include "CondTools/RunInfo/interface/LHCInfoCombined.h"
#include "CondFormats/DataRecord/interface/CTPPSBeamParametersRcd.h"

#include "CondFormats/RunInfo/interface/LHCInfo.h"

#include "CondFormats/PPSObjects/interface/CTPPSBeamParameters.h"

//----------------------------------------------------------------------------------------------------

class CTPPSBeamParametersFromLHCInfoESSource : public edm::ESProducer {
public:
  CTPPSBeamParametersFromLHCInfoESSource(const edm::ParameterSet&);
  ~CTPPSBeamParametersFromLHCInfoESSource() override = default;

  std::unique_ptr<CTPPSBeamParameters> produce(const CTPPSBeamParametersRcd&);
  static void fillDescriptions(edm::ConfigurationDescriptions&);

private:
  edm::ESGetToken<LHCInfo, LHCInfoRcd> lhcInfoToken_;
  edm::ESGetToken<LHCInfoPerLS, LHCInfoPerLSRcd> lhcInfoPerLSToken_;
  edm::ESGetToken<LHCInfoPerFill, LHCInfoPerFillRcd> lhcInfoPerFillToken_;
  const bool useNewLHCInfo_;

  CTPPSBeamParameters defaultParameters_;
};

//----------------------------------------------------------------------------------------------------

CTPPSBeamParametersFromLHCInfoESSource::CTPPSBeamParametersFromLHCInfoESSource(const edm::ParameterSet& iConfig)
    : useNewLHCInfo_(iConfig.getParameter<bool>("useNewLHCInfo")) {
  auto cc = setWhatProduced(this);
  lhcInfoToken_ = cc.consumes(edm::ESInputTag("", iConfig.getParameter<std::string>("lhcInfoLabel")));
  lhcInfoPerLSToken_ = cc.consumes(edm::ESInputTag("", iConfig.getParameter<std::string>("lhcInfoPerLSLabel")));
  lhcInfoPerFillToken_ = cc.consumes(edm::ESInputTag("", iConfig.getParameter<std::string>("lhcInfoPerFillLabel")));

  defaultParameters_.setBeamDivergenceX45(iConfig.getParameter<double>("beamDivX45"));
  defaultParameters_.setBeamDivergenceY45(iConfig.getParameter<double>("beamDivX56"));
  defaultParameters_.setBeamDivergenceX56(iConfig.getParameter<double>("beamDivY45"));
  defaultParameters_.setBeamDivergenceY56(iConfig.getParameter<double>("beamDivY56"));

  defaultParameters_.setVtxOffsetX45(iConfig.getParameter<double>("vtxOffsetX45"));
  defaultParameters_.setVtxOffsetY45(iConfig.getParameter<double>("vtxOffsetY45"));
  defaultParameters_.setVtxOffsetZ45(iConfig.getParameter<double>("vtxOffsetZ45"));
  defaultParameters_.setVtxOffsetX56(iConfig.getParameter<double>("vtxOffsetX56"));
  defaultParameters_.setVtxOffsetY56(iConfig.getParameter<double>("vtxOffsetY56"));
  defaultParameters_.setVtxOffsetZ56(iConfig.getParameter<double>("vtxOffsetZ56"));

  defaultParameters_.setVtxStddevX(iConfig.getParameter<double>("vtxStddevX"));
  defaultParameters_.setVtxStddevY(iConfig.getParameter<double>("vtxStddevY"));
  defaultParameters_.setVtxStddevZ(iConfig.getParameter<double>("vtxStddevZ"));
}

//----------------------------------------------------------------------------------------------------

std::unique_ptr<CTPPSBeamParameters> CTPPSBeamParametersFromLHCInfoESSource::produce(
    const CTPPSBeamParametersRcd& iRecord) {
  auto lhcInfoCombined =
      LHCInfoCombined::createLHCInfoCombined<CTPPSBeamParametersRcd,
                                             edm::mpl::Vector<LHCInfoRcd, LHCInfoPerFillRcd, LHCInfoPerLSRcd>>(
          iRecord, lhcInfoPerLSToken_, lhcInfoPerFillToken_, lhcInfoToken_, useNewLHCInfo_);

  auto bp = std::make_unique<CTPPSBeamParameters>(defaultParameters_);

  const auto beamMom = lhcInfoCombined.energy;
  const auto betaStar = lhcInfoCombined.betaStarX * 1E2;       // conversion m --> cm
  const auto xangle = lhcInfoCombined.crossingAngle() * 1E-6;  // conversion mu rad --> rad

  bp->setBeamMom45(beamMom);
  bp->setBeamMom56(beamMom);

  bp->setBetaStarX45(betaStar);
  bp->setBetaStarX56(betaStar);
  bp->setBetaStarY45(betaStar);
  bp->setBetaStarY56(betaStar);

  bp->setHalfXangleX45(xangle);
  bp->setHalfXangleX56(xangle);
  bp->setHalfXangleY45(xangle);
  bp->setHalfXangleY56(xangle);

  return bp;
}

//----------------------------------------------------------------------------------------------------

void CTPPSBeamParametersFromLHCInfoESSource::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  edm::ParameterSetDescription desc;

  desc.add<std::string>("lhcInfoLabel", "");
  desc.add<std::string>("lhcInfoPerLSLabel", "");
  desc.add<std::string>("lhcInfoPerFillLabel", "");
  desc.add<bool>("useNewLHCInfo", false);

  // beam divergence (rad)
  desc.add<double>("beamDivX45", 0.1);
  desc.add<double>("beamDivY45", 0.1);
  desc.add<double>("beamDivX56", 0.1);
  desc.add<double>("beamDivY56", 0.1);

  // vertex offset (cm)
  desc.add<double>("vtxOffsetX45", 1.e-2);
  desc.add<double>("vtxOffsetY45", 1.e-2);
  desc.add<double>("vtxOffsetZ45", 1.e-2);
  desc.add<double>("vtxOffsetX56", 1.e-2);
  desc.add<double>("vtxOffsetY56", 1.e-2);
  desc.add<double>("vtxOffsetZ56", 1.e-2);

  // vertex sigma (cm)
  desc.add<double>("vtxStddevX", 2.e-2);
  desc.add<double>("vtxStddevY", 2.e-2);
  desc.add<double>("vtxStddevZ", 2.e-2);

  descriptions.add("ctppsBeamParametersFromLHCInfoESSourceDefault", desc);
}

//----------------------------------------------------------------------------------------------------

DEFINE_FWK_EVENTSETUP_MODULE(CTPPSBeamParametersFromLHCInfoESSource);
