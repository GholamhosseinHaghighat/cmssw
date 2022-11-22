///////////////////////////////////////////////////////////////////////////////
// File: HFShowerLibrary.cc
// Description: Shower library for Very forward hadron calorimeter
///////////////////////////////////////////////////////////////////////////////

#include "SimG4CMS/Calo/interface/HFShowerLibrary.h"
#include "SimDataFormats/CaloHit/interface/HFShowerLibraryEventInfo.h"
#include "SimG4Core/Notification/interface/G4TrackToParticleID.h"

#include "FWCore/Utilities/interface/Exception.h"

#include "G4VPhysicalVolume.hh"
#include "G4NavigationHistory.hh"
#include "G4Step.hh"
#include "G4Track.hh"
#include "G4ParticleTable.hh"
#include "Randomize.hh"
#include "CLHEP/Units/SystemOfUnits.h"
#include "CLHEP/Units/PhysicalConstants.h"

//#define EDM_ML_DEBUG

HFShowerLibrary::HFShowerLibrary(const std::string& name,
                                 const HcalDDDSimConstants* hcons,
                                 const HcalSimulationParameters* hps,
                                 edm::ParameterSet const& p)
    : hcalConstant_(hcons), hf_(nullptr), emBranch_(nullptr), hadBranch_(nullptr), npe_(0) {
  edm::ParameterSet m_HF =
      (p.getParameter<edm::ParameterSet>("HFShower")).getParameter<edm::ParameterSet>("HFShowerBlock");
  probMax_ = m_HF.getParameter<double>("ProbMax");
  equalizeTimeShift_ = m_HF.getParameter<bool>("EqualizeTimeShift");

  edm::ParameterSet m_HS =
      (p.getParameter<edm::ParameterSet>("HFShowerLibrary")).getParameter<edm::ParameterSet>("HFLibraryFileBlock");
  edm::FileInPath fp = m_HS.getParameter<edm::FileInPath>("FileName");
  std::string pTreeName = fp.fullPath();
  backProb_ = m_HS.getParameter<double>("BackProbability");
  std::string emName = m_HS.getParameter<std::string>("TreeEMID");
  std::string hadName = m_HS.getParameter<std::string>("TreeHadID");
  std::string branchEvInfo = m_HS.getUntrackedParameter<std::string>(
      "BranchEvt", "HFShowerLibraryEventInfos_hfshowerlib_HFShowerLibraryEventInfo");
  std::string branchPre = m_HS.getUntrackedParameter<std::string>("BranchPre", "HFShowerPhotons_hfshowerlib_");
  std::string branchPost = m_HS.getUntrackedParameter<std::string>("BranchPost", "_R.obj");
  verbose_ = m_HS.getUntrackedParameter<bool>("Verbosity", false);
  applyFidCut_ = m_HS.getParameter<bool>("ApplyFiducialCut");
  fileVersion_ = m_HS.getParameter<int>("FileVersion");

  if (pTreeName.find('.') == 0)
    pTreeName.erase(0, 2);
  const char* nTree = pTreeName.c_str();
  hf_ = TFile::Open(nTree);

  if (!hf_->IsOpen()) {
    edm::LogError("HFShower") << "HFShowerLibrary: opening " << nTree << " failed";
    throw cms::Exception("Unknown", "HFShowerLibrary") << "Opening of " << pTreeName << " fails\n";
  } else {
    edm::LogVerbatim("HFShower") << "HFShowerLibrary: opening " << nTree << " successfully";
  }

  newForm_ = (branchEvInfo.empty());
  TTree* event(nullptr);
  if (newForm_)
    event = (TTree*)hf_->Get("HFSimHits");
  else
    event = (TTree*)hf_->Get("Events");
  if (event) {
    TBranch* evtInfo(nullptr);
    if (!newForm_) {
      std::string info = branchEvInfo + branchPost;
      evtInfo = event->GetBranch(info.c_str());
    }
    if (evtInfo || newForm_) {
      loadEventInfo(evtInfo);
    } else {
      edm::LogError("HFShower") << "HFShowerLibrary: HFShowerLibrayEventInfo"
                                << " Branch does not exist in Event";
      throw cms::Exception("Unknown", "HFShowerLibrary") << "Event information absent\n";
    }
  } else {
    edm::LogError("HFShower") << "HFShowerLibrary: Events Tree does not "
                              << "exist";
    throw cms::Exception("Unknown", "HFShowerLibrary") << "Events tree absent\n";
  }

  std::stringstream ss;
  ss << "HFShowerLibrary: Library " << libVers_ << " ListVersion " << listVersion_ << " File version " << fileVersion_
     << " Events Total " << totEvents_ << " and " << evtPerBin_ << " per bin\n";
  ss << "HFShowerLibrary: Energies (GeV) with " << nMomBin_ << " bins\n";
  for (int i = 0; i < nMomBin_; ++i) {
    if (i / 10 * 10 == i && i > 0) {
      ss << "\n";
    }
    ss << "  " << pmom_[i] / CLHEP::GeV;
  }
  edm::LogVerbatim("HFShower") << ss.str();

  std::string nameBr = branchPre + emName + branchPost;
  emBranch_ = event->GetBranch(nameBr.c_str());
  if (verbose_)
    emBranch_->Print();
  nameBr = branchPre + hadName + branchPost;
  hadBranch_ = event->GetBranch(nameBr.c_str());
  if (verbose_)
    hadBranch_->Print();

  v3version_ = false;
  if (emBranch_->GetClassName() == std::string("vector<float>")) {
    v3version_ = true;
  }

  edm::LogVerbatim("HFShower") << " HFShowerLibrary:Branch " << emName << " has " << emBranch_->GetEntries()
                               << " entries and Branch " << hadName << " has " << hadBranch_->GetEntries()
                               << " entries\n HFShowerLibrary::No packing information - Assume x, y, z are not in "
                                  "packed form\n Maximum probability cut off "
                               << probMax_ << "  Back propagation of light probability " << backProb_
                               << " Flag for equalizing Time Shift for different eta " << equalizeTimeShift_;

  fibre_ = std::make_unique<HFFibre>(name, hcalConstant_, hps, p);
  photo_ = std::make_unique<HFShowerPhotonCollection>();

  //Radius (minimum and maximum)
  std::vector<double> rTable = hcalConstant_->getRTableHF();
  rMin_ = rTable[0];
  rMax_ = rTable[rTable.size() - 1];

  //Delta phi
  std::vector<double> phibin = hcalConstant_->getPhiTableHF();
  dphi_ = phibin[0];

  edm::LogVerbatim("HFShower") << "HFShowerLibrary: rMIN " << rMin_ / CLHEP::cm << " cm and rMax " << rMax_ / CLHEP::cm
                               << " (Half) Phi Width of wedge " << dphi_ / CLHEP::deg;

  //Special Geometry parameters
  gpar_ = hcalConstant_->getGparHF();
}

HFShowerLibrary::~HFShowerLibrary() {
  if (hf_)
    hf_->Close();
}

std::vector<HFShowerLibrary::Hit> HFShowerLibrary::getHits(const G4Step* aStep,
                                                           bool& isKilled,
                                                           double weight,
                                                           bool onlyLong) {
  auto const preStepPoint = aStep->GetPreStepPoint();
  auto const postStepPoint = aStep->GetPostStepPoint();
  auto const track = aStep->GetTrack();
  // Get Z-direction
  auto const aParticle = track->GetDynamicParticle();
  const G4ThreeVector& momDir = aParticle->GetMomentumDirection();
  const G4ThreeVector& hitPoint = preStepPoint->GetPosition();
  int parCode = track->GetDefinition()->GetPDGEncoding();

  // VI: for ions use internally pdg code of alpha in order to keep
  // consistency with previous simulation
  if (track->GetDefinition()->IsGeneralIon()) {
    parCode = 1000020040;
  }

#ifdef EDM_ML_DEBUG
  G4String partType = track->GetDefinition()->GetParticleName();
  const G4ThreeVector localPos = preStepPoint->GetTouchable()->GetHistory()->GetTopTransform().TransformPoint(hitPoint);
  double zoff = localPos.z() + 0.5 * gpar_[1];

  edm::LogVerbatim("HFShower") << "HFShowerLibrary::getHits " << partType << " of energy "
                               << track->GetKineticEnergy() / CLHEP::GeV << " GeV weight= " << weight
                               << " onlyLong: " << onlyLong << "  dir.orts " << momDir.x() << ", " << momDir.y() << ", "
                               << momDir.z() << "  Pos x,y,z = " << hitPoint.x() << "," << hitPoint.y() << ","
                               << hitPoint.z() << " (" << zoff << ")   sphi,cphi,stheta,ctheta  = " << sin(momDir.phi())
                               << "," << cos(momDir.phi()) << ", " << sin(momDir.theta()) << "," << cos(momDir.theta());
#endif

  double tSlice = (postStepPoint->GetGlobalTime()) / CLHEP::nanosecond;

  // use kinetic energy for protons and ions
  double pin = (track->GetDefinition()->GetBaryonNumber() > 0) ? preStepPoint->GetKineticEnergy()
                                                               : preStepPoint->GetTotalEnergy();

  return fillHits(hitPoint, momDir, parCode, pin, isKilled, weight, tSlice, onlyLong);
}

std::vector<HFShowerLibrary::Hit> HFShowerLibrary::fillHits(const G4ThreeVector& hitPoint,
                                                            const G4ThreeVector& momDir,
                                                            int parCode,
                                                            double pin,
                                                            bool& ok,
                                                            double weight,
                                                            double tSlice,
                                                            bool onlyLong) {
  std::vector<HFShowerLibrary::Hit> hit;
  ok = false;
  bool isEM = G4TrackToParticleID::isGammaElectronPositron(parCode);
  // shower is built only for gamma, e+- and stable hadrons
  if (!isEM && !G4TrackToParticleID::isStableHadron(parCode)) {
    return hit;
  }
  ok = true;

  // remove low-energy component
  const double threshold = 50 * MeV;
  if (pin < threshold) {
    return hit;
  }

  double pz = momDir.z();
  double zint = hitPoint.z();

  // if particle moves from interaction point or "backwards (halo)
  bool backward = (pz * zint < 0.) ? true : false;

  double sphi = sin(momDir.phi());
  double cphi = cos(momDir.phi());
  double ctheta = cos(momDir.theta());
  double stheta = sin(momDir.theta());

  if (isEM) {
    if (pin < pmom_[nMomBin_ - 1]) {
      interpolate(0, pin);
    } else {
      extrapolate(0, pin);
    }
  } else {
    if (pin < pmom_[nMomBin_ - 1]) {
      interpolate(1, pin);
    } else {
      extrapolate(1, pin);
    }
  }

  int nHit = 0;
  HFShowerLibrary::Hit oneHit;
  for (int i = 0; i < npe_; ++i) {
    double zv = std::abs(pe_[i].z());  // abs local z
#ifdef EDM_ML_DEBUG
    edm::LogVerbatim("HFShower") << "HFShowerLibrary: Hit " << i << " " << pe_[i] << " zv " << zv;
#endif
    if (zv <= gpar_[1] && pe_[i].lambda() > 0 && (pe_[i].z() >= 0 || (zv > gpar_[0] && (!onlyLong)))) {
      int depth = 1;
      if (onlyLong) {
      } else if (!backward) {  // fully valid only for "front" particles
        if (pe_[i].z() < 0)
          depth = 2;                 // with "front"-simulated shower lib.
      } else {                       // for "backward" particles - almost equal
        double r = G4UniformRand();  // share between L and S fibers
        if (r > 0.5)
          depth = 2;
      }

      // Updated coordinate transformation from local
      //  back to global using two Euler angles: phi and theta
      double pex = pe_[i].x();
      double pey = pe_[i].y();

      double xx = pex * ctheta * cphi - pey * sphi + zv * stheta * cphi;
      double yy = pex * ctheta * sphi + pey * cphi + zv * stheta * sphi;
      double zz = -pex * stheta + zv * ctheta;

      G4ThreeVector pos = hitPoint + G4ThreeVector(xx, yy, zz);
      zv = std::abs(pos.z()) - gpar_[4] - 0.5 * gpar_[1];
      G4ThreeVector lpos = G4ThreeVector(pos.x(), pos.y(), zv);

      zv = fibre_->zShift(lpos, depth, 0);  // distance to PMT !

      double r = pos.perp();
      double p = fibre_->attLength(pe_[i].lambda());
      double fi = pos.phi();
      if (fi < 0)
        fi += CLHEP::twopi;
      int isect = int(fi / dphi_) + 1;
      isect = (isect + 1) / 2;
      double dfi = ((isect * 2 - 1) * dphi_ - fi);
      if (dfi < 0)
        dfi = -dfi;
      double dfir = r * sin(dfi);
#ifdef EDM_ML_DEBUG
      edm::LogVerbatim("HFShower") << "HFShowerLibrary: Position shift " << xx << ", " << yy << ", " << zz << ": "
                                   << pos << " R " << r << " Phi " << fi << " Section " << isect << " R*Dfi " << dfir
                                   << " Dist " << zv;
#endif
      zz = std::abs(pos.z());
      double r1 = G4UniformRand();
      double r2 = G4UniformRand();
      double r3 = backward ? G4UniformRand() : -9999.;
      if (!applyFidCut_)
        dfir += gpar_[5];

#ifdef EDM_ML_DEBUG
      edm::LogVerbatim("HFShower") << "HFShowerLibrary: rLimits " << rInside(r) << " attenuation " << r1 << ":"
                                   << exp(-p * zv) << " r2 " << r2 << " r3 " << r3 << " rDfi " << gpar_[5] << " zz "
                                   << zz << " zLim " << gpar_[4] << ":" << gpar_[4] + gpar_[1] << "\n"
                                   << "  rInside(r) :" << rInside(r) << "  r1 <= exp(-p*zv) :" << (r1 <= exp(-p * zv))
                                   << "  r2 <= probMax :" << (r2 <= probMax_ * weight)
                                   << "  r3 <= backProb :" << (r3 <= backProb_)
                                   << "  dfir > gpar[5] :" << (dfir > gpar_[5])
                                   << "  zz >= gpar[4] :" << (zz >= gpar_[4])
                                   << "  zz <= gpar[4]+gpar[1] :" << (zz <= gpar_[4] + gpar_[1]);
#endif
      if (rInside(r) && r1 <= exp(-p * zv) && r2 <= probMax_ * weight && dfir > gpar_[5] && zz >= gpar_[4] &&
          zz <= gpar_[4] + gpar_[1] && r3 <= backProb_ && (depth != 2 || zz >= gpar_[4] + gpar_[0])) {
        double tdiff = (equalizeTimeShift_) ? (fibre_->tShift(lpos, depth, -1)) : (fibre_->tShift(lpos, depth, 1));
        oneHit.position = pos;
        oneHit.depth = depth;
        oneHit.time = (tSlice + (pe_[i].t()) + tdiff);
        hit.push_back(oneHit);

#ifdef EDM_ML_DEBUG
        edm::LogVerbatim("HFShower") << "HFShowerLibrary: Final Hit " << nHit << " position " << (hit[nHit].position)
                                     << " Depth " << (hit[nHit].depth) << " Time " << tSlice << ":" << pe_[i].t() << ":"
                                     << tdiff << ":" << (hit[nHit].time);
#endif
        ++nHit;
      }
#ifdef EDM_ML_DEBUG
      else
        edm::LogVerbatim("HFShower") << "HFShowerLibrary: REJECTED !!!";
#endif
      if (onlyLong && zz >= gpar_[4] + gpar_[0] && zz <= gpar_[4] + gpar_[1]) {
        r1 = G4UniformRand();
        r2 = G4UniformRand();
        if (rInside(r) && r1 <= exp(-p * zv) && r2 <= probMax_ && dfir > gpar_[5]) {
          double tdiff = (equalizeTimeShift_) ? (fibre_->tShift(lpos, 2, -1)) : (fibre_->tShift(lpos, 2, 1));
          oneHit.position = pos;
          oneHit.depth = 2;
          oneHit.time = (tSlice + (pe_[i].t()) + tdiff);
          hit.push_back(oneHit);
#ifdef EDM_ML_DEBUG
          edm::LogVerbatim("HFShower") << "HFShowerLibrary: Final Hit " << nHit << " position " << (hit[nHit].position)
                                       << " Depth " << (hit[nHit].depth) << " Time " << tSlice << ":" << pe_[i].t()
                                       << ":" << tdiff << ":" << (hit[nHit].time);
#endif
          ++nHit;
        }
      }
    }
  }

#ifdef EDM_ML_DEBUG
  edm::LogVerbatim("HFShower") << "HFShowerLibrary: Total Hits " << nHit << " out of " << npe_ << " PE";
#endif
  if (nHit > npe_ && !onlyLong) {
    edm::LogWarning("HFShower") << "HFShowerLibrary: Hit buffer " << npe_ << " smaller than " << nHit << " Hits";
  }
  return hit;
}

bool HFShowerLibrary::rInside(double r) { return (r >= rMin_ && r <= rMax_); }

void HFShowerLibrary::getRecord(int type, int record) {
  int nrc = record - 1;
  photon_.clear();
  photo_->clear();
  if (type > 0) {
    if (newForm_) {
      if (!v3version_) {
        hadBranch_->SetAddress(&photo_);
        int position = (fileVersion_ >= 2) ? nrc : (nrc + totEvents_);
        hadBranch_->GetEntry(position);
      } else {
        std::vector<float> t;
        std::vector<float>* tp = &t;
        hadBranch_->SetAddress(&tp);
        hadBranch_->GetEntry(nrc + totEvents_);
        unsigned int tSize = t.size() / 5;
        photo_->reserve(tSize);
        for (unsigned int i = 0; i < tSize; i++) {
          photo_->push_back(
              HFShowerPhoton(t[i], t[1 * tSize + i], t[2 * tSize + i], t[3 * tSize + i], t[4 * tSize + i]));
        }
      }
    } else {
      hadBranch_->SetAddress(&photon_);
      hadBranch_->GetEntry(nrc);
    }
  } else {
    if (newForm_) {
      if (!v3version_) {
        emBranch_->SetAddress(&photo_);
        emBranch_->GetEntry(nrc);
      } else {
        std::vector<float> t;
        std::vector<float>* tp = &t;
        emBranch_->SetAddress(&tp);
        emBranch_->GetEntry(nrc);
        unsigned int tSize = t.size() / 5;
        photo_->reserve(tSize);
        for (unsigned int i = 0; i < tSize; i++) {
          photo_->push_back(
              HFShowerPhoton(t[i], t[1 * tSize + i], t[2 * tSize + i], t[3 * tSize + i], t[4 * tSize + i]));
        }
      }
    } else {
      emBranch_->SetAddress(&photon_);
      emBranch_->GetEntry(nrc);
    }
  }
#ifdef EDM_ML_DEBUG
  int nPhoton = (newForm_) ? photo_->size() : photon_.size();
  edm::LogVerbatim("HFShower") << "HFShowerLibrary::getRecord: Record " << record << " of type " << type << " with "
                               << nPhoton << " photons";
  for (int j = 0; j < nPhoton; j++)
    if (newForm_)
      edm::LogVerbatim("HFShower") << "Photon " << j << " " << photo_->at(j);
    else
      edm::LogVerbatim("HFShower") << "Photon " << j << " " << photon_[j];
#endif
}

void HFShowerLibrary::loadEventInfo(TBranch* branch) {
  if (branch) {
    std::vector<HFShowerLibraryEventInfo> eventInfoCollection;
    branch->SetAddress(&eventInfoCollection);
    branch->GetEntry(0);
    edm::LogVerbatim("HFShower") << "HFShowerLibrary::loadEventInfo loads EventInfo Collection of size "
                                 << eventInfoCollection.size() << " records";

    totEvents_ = eventInfoCollection[0].totalEvents();
    nMomBin_ = eventInfoCollection[0].numberOfBins();
    evtPerBin_ = eventInfoCollection[0].eventsPerBin();
    libVers_ = eventInfoCollection[0].showerLibraryVersion();
    listVersion_ = eventInfoCollection[0].physListVersion();
    pmom_ = eventInfoCollection[0].energyBins();
  } else {
    edm::LogVerbatim("HFShower") << "HFShowerLibrary::loadEventInfo loads EventInfo from hardwired"
                                 << " numbers";

    nMomBin_ = 16;
    evtPerBin_ = (fileVersion_ == 0) ? 5000 : 10000;
    totEvents_ = nMomBin_ * evtPerBin_;
    libVers_ = (fileVersion_ == 0) ? 1.1 : 1.2;
    listVersion_ = 3.6;
    pmom_ = {2, 3, 5, 7, 10, 15, 20, 30, 50, 75, 100, 150, 250, 350, 500, 1000};
  }
  for (int i = 0; i < nMomBin_; i++)
    pmom_[i] *= CLHEP::GeV;
}

void HFShowerLibrary::interpolate(int type, double pin) {
#ifdef EDM_ML_DEBUG
  edm::LogVerbatim("HFShower") << "HFShowerLibrary:: Interpolate for Energy " << pin / CLHEP::GeV << " GeV with "
                               << nMomBin_ << " momentum bins and " << evtPerBin_ << " entries/bin -- total "
                               << totEvents_;
#endif
  int irc[2] = {0, 0};
  double w = 0.;
  double r = G4UniformRand();

  if (pin < pmom_[0]) {
    w = pin / pmom_[0];
    irc[1] = int(evtPerBin_ * r) + 1;
    irc[0] = 0;
  } else {
    for (int j = 0; j < nMomBin_ - 1; j++) {
      if (pin >= pmom_[j] && pin < pmom_[j + 1]) {
        w = (pin - pmom_[j]) / (pmom_[j + 1] - pmom_[j]);
        if (j == nMomBin_ - 2) {
          irc[1] = int(evtPerBin_ * 0.5 * r);
        } else {
          irc[1] = int(evtPerBin_ * r);
        }
        irc[1] += (j + 1) * evtPerBin_ + 1;
        r = G4UniformRand();
        irc[0] = int(evtPerBin_ * r) + 1 + j * evtPerBin_;
        if (irc[0] < 0) {
          edm::LogWarning("HFShower") << "HFShowerLibrary:: Illegal irc[0] = " << irc[0] << " now set to 0";
          irc[0] = 0;
        } else if (irc[0] > totEvents_) {
          edm::LogWarning("HFShower") << "HFShowerLibrary:: Illegal irc[0] = " << irc[0] << " now set to "
                                      << totEvents_;
          irc[0] = totEvents_;
        }
      }
    }
  }
  if (irc[1] < 1) {
    edm::LogWarning("HFShower") << "HFShowerLibrary:: Illegal irc[1] = " << irc[1] << " now set to 1";
    irc[1] = 1;
  } else if (irc[1] > totEvents_) {
    edm::LogWarning("HFShower") << "HFShowerLibrary:: Illegal irc[1] = " << irc[1] << " now set to " << totEvents_;
    irc[1] = totEvents_;
  }

#ifdef EDM_ML_DEBUG
  edm::LogVerbatim("HFShower") << "HFShowerLibrary:: Select records " << irc[0] << " and " << irc[1] << " with weights "
                               << 1 - w << " and " << w;
#endif
  pe_.clear();
  npe_ = 0;
  int npold = 0;
  for (int ir = 0; ir < 2; ir++) {
    if (irc[ir] > 0) {
      getRecord(type, irc[ir]);
      int nPhoton = (newForm_) ? photo_->size() : photon_.size();
      npold += nPhoton;
      for (int j = 0; j < nPhoton; j++) {
        r = G4UniformRand();
        if ((ir == 0 && r > w) || (ir > 0 && r < w)) {
          storePhoton(j);
        }
      }
    }
  }

  if ((npe_ > npold || (npold == 0 && irc[0] > 0)) && !(npe_ == 0 && npold == 0))
    edm::LogWarning("HFShower") << "HFShowerLibrary: Interpolation Warning =="
                                << " records " << irc[0] << " and " << irc[1] << " gives a buffer of " << npold
                                << " photons and fills " << npe_ << " *****";
#ifdef EDM_ML_DEBUG
  else
    edm::LogVerbatim("HFShower") << "HFShowerLibrary: Interpolation == records " << irc[0] << " and " << irc[1]
                                 << " gives a buffer of " << npold << " photons and fills " << npe_ << " PE";
  for (int j = 0; j < npe; j++)
    edm::LogVerbatim("HFShower") << "Photon " << j << " " << pe_[j];
#endif
}

void HFShowerLibrary::extrapolate(int type, double pin) {
  int nrec = int(pin / pmom_[nMomBin_ - 1]);
  double w = (pin - pmom_[nMomBin_ - 1] * nrec) / pmom_[nMomBin_ - 1];
  nrec++;
#ifdef EDM_ML_DEBUG
  edm::LogVerbatim("HFShower") << "HFShowerLibrary:: Extrapolate for Energy " << pin / CLHEP::GeV << " GeV with "
                               << nMomBin_ << " momentum bins and " << evtPerBin_ << " entries/bin -- "
                               << "total " << totEvents_ << " using " << nrec << " records";
#endif
  std::vector<int> irc(nrec);

  for (int ir = 0; ir < nrec; ir++) {
    double r = G4UniformRand();
    irc[ir] = int(evtPerBin_ * 0.5 * r) + (nMomBin_ - 1) * evtPerBin_ + 1;
    if (irc[ir] < 1) {
      edm::LogWarning("HFShower") << "HFShowerLibrary:: Illegal irc[" << ir << "] = " << irc[ir] << " now set to 1";
      irc[ir] = 1;
    } else if (irc[ir] > totEvents_) {
      edm::LogWarning("HFShower") << "HFShowerLibrary:: Illegal irc[" << ir << "] = " << irc[ir] << " now set to "
                                  << totEvents_;
      irc[ir] = totEvents_;
#ifdef EDM_ML_DEBUG
    } else {
      edm::LogVerbatim("HFShower") << "HFShowerLibrary::Extrapolation use irc[" << ir << "] = " << irc[ir];
#endif
    }
  }

  pe_.clear();
  npe_ = 0;
  int npold = 0;
  for (int ir = 0; ir < nrec; ir++) {
    if (irc[ir] > 0) {
      getRecord(type, irc[ir]);
      int nPhoton = (newForm_) ? photo_->size() : photon_.size();
      npold += nPhoton;
      for (int j = 0; j < nPhoton; j++) {
        double r = G4UniformRand();
        if (ir != nrec - 1 || r < w) {
          storePhoton(j);
        }
      }
#ifdef EDM_ML_DEBUG
      edm::LogVerbatim("HFShower") << "HFShowerLibrary: Record [" << ir << "] = " << irc[ir] << " npold = " << npold;
#endif
    }
  }
#ifdef EDM_ML_DEBUG
  edm::LogVerbatim("HFShower") << "HFShowerLibrary:: uses " << npold << " photons";
#endif

  if (npe_ > npold || npold == 0)
    edm::LogWarning("HFShower") << "HFShowerLibrary: Extrapolation Warning == " << nrec << " records " << irc[0] << ", "
                                << irc[1] << ", ... gives a buffer of " << npold << " photons and fills " << npe_
                                << " *****";
#ifdef EDM_ML_DEBUG
  else
    edm::LogVerbatim("HFShower") << "HFShowerLibrary: Extrapolation == " << nrec << " records " << irc[0] << ", "
                                 << irc[1] << ", ... gives a buffer of " << npold << " photons and fills " << npe_
                                 << " PE";
  for (int j = 0; j < npe; j++)
    edm::LogVerbatim("HFShower") << "Photon " << j << " " << pe_[j];
#endif
}

void HFShowerLibrary::storePhoton(int j) {
  if (newForm_)
    pe_.push_back(photo_->at(j));
  else
    pe_.push_back(photon_[j]);
  npe_++;
#ifdef EDM_ML_DEBUG
  edm::LogVerbatim("HFShower") << "HFShowerLibrary: storePhoton " << j << " npe " << npe_ << " " << pe_[npe_ - 1];
#endif
}
