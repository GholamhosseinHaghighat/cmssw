import FWCore.ParameterSet.Config as cms
from DQMServices.Core.DQMEDHarvester import DQMEDHarvester
from DQM.L1TMonitor.L1TStage2uGMT_cff import ignoreBins

# directory path shortening
ugmtDqmDir = 'L1T/L1TStage2uGMT'
ugmtMuCpyDqmDir = ugmtDqmDir+'/uGMTMuonCopies'
# input histograms
errHistNumStr = 'errorSummaryNum'
errHistDenStr = 'errorSummaryDen'

# Muons
l1tStage2uGMTMuonVsuGMTMuonCopy1RatioClient = DQMEDHarvester("L1TStage2RatioClient",
    monitorDir = cms.untracked.string(ugmtMuCpyDqmDir+'/uGMTMuonCopy1'),
    inputNum = cms.untracked.string(ugmtMuCpyDqmDir+'/uGMTMuonCopy1/'+errHistNumStr),
    inputDen = cms.untracked.string(ugmtMuCpyDqmDir+'/uGMTMuonCopy1/'+errHistDenStr),
    ratioName = cms.untracked.string('mismatchRatio'),
    ratioTitle = cms.untracked.string('Summary of mismatch rates between uGMT muons and uGMT muon copy 1'),
    yAxisTitle = cms.untracked.string('# mismatch / # total'),
    binomialErr = cms.untracked.bool(True),
    ignoreBin = cms.untracked.vint32()                                                         
)
## Era: Run3_2021; Ignore BX range mismatches. This is necessary because we only read out the central BX for the output copies.
from Configuration.Eras.Modifier_stage2L1Trigger_2021_cff import stage2L1Trigger_2021
stage2L1Trigger_2021.toModify(l1tStage2uGMTMuonVsuGMTMuonCopy1RatioClient, ignoreBin = cms.untracked.vint32(ignoreBins['OutputCopies']))

l1tStage2uGMTMuonVsuGMTMuonCopy2RatioClient = l1tStage2uGMTMuonVsuGMTMuonCopy1RatioClient.clone(
    monitorDir = ugmtMuCpyDqmDir+'/uGMTMuonCopy2',
    inputNum = ugmtMuCpyDqmDir+'/uGMTMuonCopy2/'+errHistNumStr,
    inputDen = ugmtMuCpyDqmDir+'/uGMTMuonCopy2/'+errHistDenStr,
    ratioTitle = 'Summary of mismatch rates between uGMT muons and uGMT muon copy 2'
)
l1tStage2uGMTMuonVsuGMTMuonCopy3RatioClient = l1tStage2uGMTMuonVsuGMTMuonCopy1RatioClient.clone(
    monitorDir = ugmtMuCpyDqmDir+'/uGMTMuonCopy3',
    inputNum = ugmtMuCpyDqmDir+'/uGMTMuonCopy3/'+errHistNumStr,
    inputDen = ugmtMuCpyDqmDir+'/uGMTMuonCopy3/'+errHistDenStr,
    ratioTitle = 'Summary of mismatch rates between uGMT muons and uGMT muon copy 3'
)
l1tStage2uGMTMuonVsuGMTMuonCopy4RatioClient = l1tStage2uGMTMuonVsuGMTMuonCopy1RatioClient.clone(
    monitorDir = ugmtMuCpyDqmDir+'/uGMTMuonCopy4',
    inputNum = ugmtMuCpyDqmDir+'/uGMTMuonCopy4/'+errHistNumStr,
    inputDen = ugmtMuCpyDqmDir+'/uGMTMuonCopy4/'+errHistDenStr,
    ratioTitle = 'Summary of mismatch rates between uGMT muons and uGMT muon copy 4'
)
l1tStage2uGMTMuonVsuGMTMuonCopy5RatioClient = l1tStage2uGMTMuonVsuGMTMuonCopy1RatioClient.clone(
    monitorDir = ugmtMuCpyDqmDir+'/uGMTMuonCopy5',
    inputNum = ugmtMuCpyDqmDir+'/uGMTMuonCopy5/'+errHistNumStr,
    inputDen = ugmtMuCpyDqmDir+'/uGMTMuonCopy5/'+errHistDenStr,
    ratioTitle = 'Summary of mismatch rates between uGMT muons and uGMT muon copy 5'
)
# RegionalMuonCands
l1tStage2BmtfOutVsuGMTInRatioClient = l1tStage2uGMTMuonVsuGMTMuonCopy1RatioClient.clone(
    monitorDir = ugmtDqmDir+'/BMTFoutput_vs_uGMTinput',
    inputNum = ugmtDqmDir+'/BMTFoutput_vs_uGMTinput/'+errHistNumStr,
    inputDen = ugmtDqmDir+'/BMTFoutput_vs_uGMTinput/'+errHistDenStr,
    ratioTitle = 'Summary of mismatch rates between BMTF output muons and uGMT input muons from BMTF',
    ignoreBin =  ignoreBins['Bmtf']
)
l1tStage2OmtfOutVsuGMTInRatioClient = l1tStage2uGMTMuonVsuGMTMuonCopy1RatioClient.clone(
    monitorDir = ugmtDqmDir+'/OMTFoutput_vs_uGMTinput',
    inputNum = ugmtDqmDir+'/OMTFoutput_vs_uGMTinput/'+errHistNumStr,
    inputDen = ugmtDqmDir+'/OMTFoutput_vs_uGMTinput/'+errHistDenStr,
    ratioTitle = 'Summary of mismatch rates between OMTF output muons and uGMT input muons from OMTF',
    ignoreBin = ignoreBins['Omtf']
)
l1tStage2EmtfOutVsuGMTInRatioClient = l1tStage2uGMTMuonVsuGMTMuonCopy1RatioClient.clone(
    monitorDir = ugmtDqmDir+'/EMTFoutput_vs_uGMTinput',
    inputNum = ugmtDqmDir+'/EMTFoutput_vs_uGMTinput/'+errHistNumStr,
    inputDen = ugmtDqmDir+'/EMTFoutput_vs_uGMTinput/'+errHistDenStr,
    ratioTitle = 'Summary of mismatch rates between EMTF output muons and uGMT input muons from EMTF',
    ignoreBin = ignoreBins['Emtf']
)

# sequences
l1tStage2uGMTMuonCompClient = cms.Sequence(
    l1tStage2uGMTMuonVsuGMTMuonCopy1RatioClient
  + l1tStage2uGMTMuonVsuGMTMuonCopy2RatioClient
  + l1tStage2uGMTMuonVsuGMTMuonCopy3RatioClient
  + l1tStage2uGMTMuonVsuGMTMuonCopy4RatioClient
  + l1tStage2uGMTMuonVsuGMTMuonCopy5RatioClient
)

l1tStage2uGMTRegionalMuonCandCompClient = cms.Sequence(
    l1tStage2BmtfOutVsuGMTInRatioClient
  + l1tStage2OmtfOutVsuGMTInRatioClient
  + l1tStage2EmtfOutVsuGMTInRatioClient
)

l1tStage2uGMTClient = cms.Sequence(
    l1tStage2uGMTMuonCompClient
  + l1tStage2uGMTRegionalMuonCandCompClient
)

