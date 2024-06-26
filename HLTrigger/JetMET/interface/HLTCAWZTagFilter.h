#ifndef HLTCAWZTagFilter_h
#define HLTCAWZTagFilter_h

// system include files
#include <memory>
#include <vector>
#include <sstream>

// user include files
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "DataFormats/BTauReco/interface/CATopJetTagInfo.h"
#include "FWCore/Utilities/interface/InputTag.h"
#include "DataFormats/JetReco/interface/BasicJet.h"
#include "DataFormats/JetReco/interface/CaloJet.h"
#include "DataFormats/Candidate/interface/CompositeCandidate.h"
#include "DataFormats/HLTReco/interface/TriggerTypeDefs.h"
#include "HLTrigger/HLTcore/interface/HLTFilter.h"
#include "FWCore/ParameterSet/interface/ConfigurationDescriptions.h"
#include "FWCore/ParameterSet/interface/ParameterSetDescription.h"
#include <Math/VectorUtil.h>

class CAWZJetHelperUser {
public:
  CAWZJetHelperUser(double massdropcut) : massdropcut_(massdropcut) {}

  reco::CATopJetProperties operator()(reco::Jet const& ihardJet) const;

protected:
  double massdropcut_;
};

//
// class declaration
//

class HLTCAWZTagFilter : public HLTFilter {
public:
  explicit HLTCAWZTagFilter(const edm::ParameterSet&);
  ~HLTCAWZTagFilter() override;
  static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);
  bool hltFilter(edm::Event&,
                 const edm::EventSetup&,
                 trigger::TriggerFilterObjectWithRefs& filterobject) const override;

private:
  // ----------member data ---------------------------

  edm::InputTag src_;
  edm::InputTag pfsrc_;
  const edm::EDGetTokenT<reco::BasicJetCollection> inputToken_;
  const edm::EDGetTokenT<reco::PFJetCollection> inputPFToken_;
  double minWMass_;
  double maxWMass_;
  double massdropcut_;
};

inline reco::CATopJetProperties CAWZJetHelperUser::operator()(reco::Jet const& ihardJet) const {
  reco::CATopJetProperties properties;
  // Get subjets
  reco::Jet::Constituents subjets = ihardJet.getJetConstituents();
  properties.nSubJets = subjets.size();  // number of subjets
  properties.wMass = 999999.;            // best W mass
  properties.topMass = 999999.;
  properties.minMass = -1;

  if (properties.nSubJets == 2) {
    sort(subjets.begin(), subjets.end(), [](auto const& t1, auto const& t2) { return t1->pt() > t2->pt(); });

    reco::Jet::Constituent icandJet = subjets[0];

    reco::Candidate::LorentzVector isubJet = icandJet->p4();
    double imass = isubJet.mass();
    double imw = ihardJet.mass();

    if (imass / imw < massdropcut_) {
      // Get the candidate mass
      properties.wMass = imw;
    }
  }

  return properties;
}

#endif
