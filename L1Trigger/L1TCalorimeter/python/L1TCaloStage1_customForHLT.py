# customization fragments to run L1Emulator with hltGetConfiguration
#

import FWCore.ParameterSet.Config as cms
import os

##############################################################################

def customiseL1EmulatorFromRaw(process):
    # customization fragment to run full emulator chain (TPGs and L1 Muon,Calo and GT emulators) 
    # starting from a RAW file assuming that "RawToDigi_cff" and "SimL1Emulator_cff" 
    # have already been loaded
    
    # (GMT digis produced by same module as the GT digis, as GT and GMT have common unpacker)
    ## process.simRpcTechTrigDigis.RPCDigiLabel = 'muonRPCDigis'

    # RCT
    # HCAL input would be from hcalDigis if hack not needed
    from L1Trigger.Configuration.SimL1Emulator_cff import simRctDigis
    simRctDigis.ecalDigis = cms.VInputTag( cms.InputTag( 'ecalDigis:EcalTriggerPrimitives' ) )
    simRctDigis.hcalDigis = cms.VInputTag( cms.InputTag( 'simHcalTriggerPrimitiveDigis' ) )

    # stage 1 itself
    from L1Trigger.L1TCalorimeter.L1TCaloStage1_cff import rctUpgradeFormatDigis
    ## process.load('L1Trigger.L1TCalorimeter.L1TCaloStage1_cff')
    rctUpgradeFormatDigis.regionTag = cms.InputTag("simRctDigis")
    rctUpgradeFormatDigis.emTag = cms.InputTag("simRctDigis")

    process.reEmulCaloChain = cms.Sequence(
        process.L1TRerunHCALTP_FromRAW +
        process.ecalDigis +
        process.simRctDigis +
        process.L1TCaloStage1
        )
    
    ### 2015 L1 Muon Emulator
    from L1Trigger.DTTrackFinder.dttfDigis_cfi import dttfDigis
    process.dttfReEmulDigis       = dttfDigis.clone()
    process.dttfReEmulDigis.DTDigi_Source  = cms.InputTag("dttfDigis")
    process.dttfReEmulDigis.CSCStub_Source = cms.InputTag("csctfReEmulTrackDigis")

    from L1Trigger.RPCTrigger.rpcTriggerDigis_cfi import rpcTriggerDigis
    process.rpcTriggerReEmulDigis = rpcTriggerDigis.clone()

    ## remove for 720pre7 and beyond
    patternDirectory = "L1Trigger/L1TCommon/data/rpc_patterns/xml/"

    process.load("L1TriggerConfig.RPCTriggerConfig.RPCConeDefinition_cff")
    process.load("L1TriggerConfig.RPCTriggerConfig.L1RPCConfig_cff")
    process.load("L1Trigger.RPCTrigger.RPCConeConfig_cff")
    process.rpcconf.filedir = cms.untracked.string(patternDirectory)
    process.es_prefer_rpcPats = cms.ESPrefer("RPCTriggerConfig","rpcconf")
    ### end

    from SLHCUpgradeSimulations.Configuration.muonCustoms import customise_csc_L1Emulator_sim
    from L1Trigger.CSCTrackFinder.csctfDigis_cfi import csctfDigis
    customise_csc_L1Emulator_sim(process) 

    process.csctfReEmulTrackDigis = process.simCsctfTrackDigis.clone()
    process.csctfReEmulDigis      = csctfDigis.clone()

    process.csctfReEmulTrackDigis.DTproducer  = cms.untracked.InputTag("dttfDigis")
    process.csctfReEmulDigis.CSCTrackProducer = cms.untracked.InputTag("csctfReEmulTrackDigis")

    process.csctfReEmulTrackDigis.SectorProcessor.PTLUT.PtMethod = cms.untracked.uint32(34) # no triple ganging in ME11a
    process.csctfReEmulTrackDigis.SectorProcessor.gangedME1a = cms.untracked.bool(False)
    process.csctfReEmulTrackDigis.SectorProcessor.firmwareSP = cms.uint32(20140515) #core 20120730
    process.csctfReEmulTrackDigis.SectorProcessor.initializeFromPSet = cms.bool(True) 

    process.csctfReEmulSequence = cms.Sequence(
        process.simCscTriggerPrimitiveDigis
        * process.csctfReEmulTrackDigis
        * process.csctfReEmulDigis
    )

    process.load('L1TriggerConfig.GMTConfigProducers.L1MuGMTParameters_cfi')
    process.L1MuGMTParameters.MergeMethodPtBrl=cms.string("byCombi")
    process.L1MuGMTParameters.MergeMethodPtFwd=cms.string("byCombi")
    process.L1MuGMTParameters.VersionSortRankEtaQLUT = cms.uint32(1043)
    process.L1MuGMTParameters.VersionLUTs = cms.uint32(1)

    from L1Trigger.GlobalMuonTrigger.gmtDigis_cfi import gmtDigis
    process.gmtReEmulDigis  = gmtDigis.clone()

    process.gmtReEmulDigis.DTCandidates   = cms.InputTag("dttfReEmulDigis","DT")
    process.gmtReEmulDigis.CSCCandidates  = cms.InputTag("csctfReEmulDigis","CSC")
    process.gmtReEmulDigis.RPCbCandidates = cms.InputTag("rpcTriggerReEmulDigis","RPCb")
    process.gmtReEmulDigis.RPCfCandidates = cms.InputTag("rpcTriggerReEmulDigis","RPCf")
    process.gmtReEmulDigis.MipIsoData     = cms.InputTag("none")
    
    process.load('L1TriggerConfig.GMTConfigProducers.L1MuGMTParameters_cfi')
    process.L1MuGMTParameters.MergeMethodPtBrl=cms.string("byMinPt")
    process.L1MuGMTParameters.MergeMethodPtFwd=cms.string("byMinPt")
    process.L1MuGMTParameters.VersionSortRankEtaQLUT = cms.uint32(275)
    process.L1MuGMTParameters.VersionLUTs = cms.uint32(1) 
    process.es_prefer_gmtConfig = cms.ESPrefer("L1MuGMTParametersProducer","L1MuGMTParameters")

    process.reEmulMuonChain = cms.Sequence(
        process.rpcTriggerReEmulDigis
        *process.csctfReEmulSequence
        *process.dttfReEmulDigis
        *process.gmtReEmulDigis
        )

    ## remove for 720pre7 and beyond
    ## use new muon LUTs
    dttfFile='sqlite:dttf_config.db'
    process.GlobalTag.toGet.extend(
        cms.VPSet(cms.PSet(record = cms.string("L1MuDTEtaPatternLutRcd"),
                           tag = cms.string("L1MuDTEtaPatternLut_CRAFT09_hlt"),
                           connect = cms.untracked.string(dttfFile)
                       ),
                  cms.PSet(record = cms.string("L1MuDTExtLutRcd"),
                           tag = cms.string("L1MuDTExtLut_CRAFT09_hlt"),
                           connect = cms.untracked.string(dttfFile)
                       ),
                  cms.PSet(record = cms.string("L1MuDTPhiLutRcd"),
                           tag = cms.string("L1MuDTPhiLut_CRAFT09_hlt"),
                           connect = cms.untracked.string(dttfFile)
                       ),
                  cms.PSet(record = cms.string("L1MuDTPtaLutRcd"),
                           tag = cms.string("L1MuDTPtaLut_CRAFT09_hlt"),
                           connect = cms.untracked.string(dttfFile)
                       ),
                  cms.PSet(record = cms.string("L1MuDTQualPatternLutRcd"),
                           tag = cms.string("L1MuDTQualPatternLut_CRAFT09_hlt"),
                           connect = cms.untracked.string(dttfFile)
                       )
             )
    )
    ## end

    # GT
    from L1Trigger.Configuration.SimL1Emulator_cff import simGtDigis
    simGtDigis.GmtInputTag = 'gmtReEmulDigis'
    simGtDigis.GctInputTag = 'caloStage1LegacyFormatDigis'
    simGtDigis.TechnicalTriggersInputTags = cms.VInputTag( )

    # run Calo TPGs, L1 GCT, technical triggers, L1 GT
    SimL1Emulator = cms.Sequence(
        process.reEmulCaloChain +
        process.reEmulMuonChain +
        process.simGtDigis )

    # replace the SimL1Emulator in all paths and sequences
    for iterable in process.sequences.itervalues():
        iterable.replace( process.SimL1Emulator, SimL1Emulator)
    for iterable in process.paths.itervalues():
        iterable.replace( process.SimL1Emulator, SimL1Emulator)
    for iterable in process.endpaths.itervalues():
        iterable.replace( process.SimL1Emulator, SimL1Emulator)
    process.SimL1Emulator = SimL1Emulator

    return process

##############################################################################
