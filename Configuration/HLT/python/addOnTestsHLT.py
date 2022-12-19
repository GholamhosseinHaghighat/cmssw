def addOnTestsHLT():

        addOnTestsHLT = {
                    'hlt_mc_Fake' : ['cmsDriver.py TTbar_8TeV_TuneCUETP8M1_cfi -s GEN,SIM,DIGI,L1,DIGI2RAW --mc --scenario=pp -n 10 --conditions auto:run1_mc_Fake --relval 9000,50 --datatier "GEN-SIM-RAW" --eventcontent RAWSIM --customise=HLTrigger/Configuration/CustomConfigs.L1T --fileout file:RelVal_Raw_Fake_MC.root',
                               'HLTrigger/Configuration/test/OnLine_HLT_Fake.py',
                               'cmsDriver.py RelVal -s HLT:Fake,RAW2DIGI,L1Reco,RECO --mc --scenario=pp -n 10 --conditions auto:run1_mc_Fake --relval 9000,50 --datatier "RAW-HLT-RECO" --eventcontent FEVTDEBUGHLT --customise=HLTrigger/Configuration/CustomConfigs.L1THLT --customise=HLTrigger/Configuration/CustomConfigs.HLTRECO --processName=HLTRECO --filein file:RelVal_Raw_Fake_MC.root --fileout file:RelVal_Raw_Fake_MC_HLT_RECO.root'],
                    'hlt_mc_Fake1': ['cmsDriver.py TTbar_13TeV_TuneCUETP8M1_cfi -s GEN,SIM,DIGI,L1,DIGI2RAW --mc --scenario=pp -n 10 --conditions auto:run2_mc_Fake1 --relval 9000,50 --datatier "GEN-SIM-RAW" --eventcontent RAWSIM --customise=HLTrigger/Configuration/CustomConfigs.L1T --era Run2_25ns --fileout file:RelVal_Raw_Fake1_MC.root',
                               'HLTrigger/Configuration/test/OnLine_HLT_Fake1.py',
                               'cmsDriver.py RelVal -s HLT:Fake1,RAW2DIGI,L1Reco,RECO --mc --scenario=pp -n 10 --conditions auto:run2_mc_Fake1 --relval 9000,50 --datatier "RAW-HLT-RECO" --eventcontent FEVTDEBUGHLT --customise=HLTrigger/Configuration/CustomConfigs.L1THLT --customise=HLTrigger/Configuration/CustomConfigs.HLTRECO --era Run2_25ns --processName=HLTRECO --filein file:RelVal_Raw_Fake1_MC.root --fileout file:RelVal_Raw_Fake1_MC_HLT_RECO.root'],
                    'hlt_mc_Fake2': ['cmsDriver.py TTbar_13TeV_TuneCUETP8M1_cfi -s GEN,SIM,DIGI,L1,DIGI2RAW --mc --scenario=pp -n 10 --conditions auto:run2_mc_Fake2 --relval 9000,50 --datatier "GEN-SIM-RAW" --eventcontent RAWSIM --customise=HLTrigger/Configuration/CustomConfigs.L1T --era Run2_2016 --fileout file:RelVal_Raw_Fake2_MC.root',
                               'HLTrigger/Configuration/test/OnLine_HLT_Fake2.py',
                               'cmsDriver.py RelVal -s HLT:Fake2,RAW2DIGI,L1Reco,RECO --mc --scenario=pp -n 10 --conditions auto:run2_mc_Fake2 --relval 9000,50 --datatier "RAW-HLT-RECO" --eventcontent FEVTDEBUGHLT --customise=HLTrigger/Configuration/CustomConfigs.L1THLT --customise=HLTrigger/Configuration/CustomConfigs.HLTRECO --era Run2_2016 --processName=HLTRECO --filein file:RelVal_Raw_Fake2_MC.root --fileout file:RelVal_Raw_Fake2_MC_HLT_RECO.root'],
                    'hlt_mc_GRun' : ['cmsDriver.py TTbar_13TeV_TuneCUETP8M1_cfi -s GEN,SIM,DIGI,L1,DIGI2RAW --mc --scenario=pp -n 10 --conditions auto:run3_mc_GRun --relval 9000,50 --datatier "GEN-SIM-RAW" --eventcontent RAWSIM --customise=HLTrigger/Configuration/CustomConfigs.L1T --era Run3 --fileout file:RelVal_Raw_GRun_MC.root',
                               'HLTrigger/Configuration/test/OnLine_HLT_GRun.py',
                               'cmsDriver.py RelVal -s HLT:GRun,RAW2DIGI,L1Reco,RECO --mc --scenario=pp -n 10 --conditions auto:run3_mc_GRun --relval 9000,50 --datatier "RAW-HLT-RECO" --eventcontent FEVTDEBUGHLT --customise=HLTrigger/Configuration/CustomConfigs.L1THLT --customise=HLTrigger/Configuration/CustomConfigs.HLTRECO --era Run3 --processName=HLTRECO --filein file:RelVal_Raw_GRun_MC.root --fileout file:RelVal_Raw_GRun_MC_HLT_RECO.root'],
                    'hlt_mc_HIon' : ['cmsDriver.py TTbar_13TeV_TuneCUETP8M1_cfi -s GEN,SIM,DIGI,L1,DIGI2RAW --mc --scenario=pp -n 10 --conditions auto:run3_mc_HIon --relval 9000,50 --datatier "GEN-SIM-RAW" --eventcontent RAWSIM --customise=HLTrigger/Configuration/CustomConfigs.L1T --era Run3_pp_on_PbPb --fileout file:RelVal_Raw_HIon_MC.root',
                               'HLTrigger/Configuration/test/OnLine_HLT_HIon.py',
                               'cmsDriver.py RelVal -s HLT:HIon,RAW2DIGI,L1Reco,RECO --mc --scenario=pp -n 10 --conditions auto:run3_mc_HIon --relval 9000,50 --datatier "RAW-HLT-RECO" --eventcontent FEVTDEBUGHLT --customise=HLTrigger/Configuration/CustomConfigs.L1THLT --customise=HLTrigger/Configuration/CustomConfigs.HLTRECO --era Run3_pp_on_PbPb --processName=HLTRECO --filein file:RelVal_Raw_HIon_MC.root --fileout file:RelVal_Raw_HIon_MC_HLT_RECO.root'],
                    'hlt_mc_PIon' : ['cmsDriver.py TTbar_13TeV_TuneCUETP8M1_cfi -s GEN,SIM,DIGI,L1,DIGI2RAW --mc --scenario=pp -n 10 --conditions auto:run3_mc_PIon --relval 9000,50 --datatier "GEN-SIM-RAW" --eventcontent RAWSIM --customise=HLTrigger/Configuration/CustomConfigs.L1T --era Run3 --fileout file:RelVal_Raw_PIon_MC.root',
                               'HLTrigger/Configuration/test/OnLine_HLT_PIon.py',
                               'cmsDriver.py RelVal -s HLT:PIon,RAW2DIGI,L1Reco,RECO --mc --scenario=pp -n 10 --conditions auto:run3_mc_PIon --relval 9000,50 --datatier "RAW-HLT-RECO" --eventcontent FEVTDEBUGHLT --customise=HLTrigger/Configuration/CustomConfigs.L1THLT --customise=HLTrigger/Configuration/CustomConfigs.HLTRECO --era Run3 --processName=HLTRECO --filein file:RelVal_Raw_PIon_MC.root --fileout file:RelVal_Raw_PIon_MC_HLT_RECO.root'],
                    'hlt_mc_PRef' : ['cmsDriver.py TTbar_13TeV_TuneCUETP8M1_cfi -s GEN,SIM,DIGI,L1,DIGI2RAW --mc --scenario=pp -n 10 --conditions auto:run3_mc_PRef --relval 9000,50 --datatier "GEN-SIM-RAW" --eventcontent RAWSIM --customise=HLTrigger/Configuration/CustomConfigs.L1T --era Run3 --fileout file:RelVal_Raw_PRef_MC.root',
                               'HLTrigger/Configuration/test/OnLine_HLT_PRef.py',
                               'cmsDriver.py RelVal -s HLT:PRef,RAW2DIGI,L1Reco,RECO --mc --scenario=pp -n 10 --conditions auto:run3_mc_PRef --relval 9000,50 --datatier "RAW-HLT-RECO" --eventcontent FEVTDEBUGHLT --customise=HLTrigger/Configuration/CustomConfigs.L1THLT --customise=HLTrigger/Configuration/CustomConfigs.HLTRECO --era Run3 --processName=HLTRECO --filein file:RelVal_Raw_PRef_MC.root --fileout file:RelVal_Raw_PRef_MC_HLT_RECO.root'],
                    'hlt_data_Fake' : ['cmsDriver.py RelVal -s L1REPACK:GT1 --data --scenario=pp -n 10 --conditions auto:run1_hlt_Fake --relval 9000,50 --datatier "RAW" --eventcontent RAW --customise=HLTrigger/Configuration/CustomConfigs.L1T --fileout file:RelVal_Raw_Fake_DATA.root --filein /store/data/Run2012A/MuEG/RAW/v1/000/191/718/14932935-E289-E111-830C-5404A6388697.root',
                               'HLTrigger/Configuration/test/OnLine_HLT_Fake.py',
                               'cmsDriver.py RelVal -s HLT:Fake,RAW2DIGI,L1Reco,RECO --data --scenario=pp -n 10 --conditions auto:run1_data_Fake --relval 9000,50 --datatier "RAW-HLT-RECO" --eventcontent FEVTDEBUGHLT --customise=HLTrigger/Configuration/CustomConfigs.L1THLT --customise=HLTrigger/Configuration/CustomConfigs.HLTRECO --processName=HLTRECO --filein file:RelVal_Raw_Fake_DATA.root --fileout file:RelVal_Raw_Fake_DATA_HLT_RECO.root'],
                    'hlt_data_Fake1': ['cmsDriver.py RelVal -s L1REPACK:GCTGT --data --scenario=pp -n 10 --conditions auto:run2_hlt_Fake1 --relval 9000,50 --datatier "RAW" --eventcontent RAW --customise=HLTrigger/Configuration/CustomConfigs.L1T --era Run2_25ns --fileout file:RelVal_Raw_Fake1_DATA.root --filein /store/data/Run2015D/MuonEG/RAW/v1/000/256/677/00000/80950A90-745D-E511-92FD-02163E011C5D.root',
                               'HLTrigger/Configuration/test/OnLine_HLT_Fake1.py',
                               'cmsDriver.py RelVal -s HLT:Fake1,RAW2DIGI,L1Reco,RECO --data --scenario=pp -n 10 --conditions auto:run2_data_Fake1 --relval 9000,50 --datatier "RAW-HLT-RECO" --eventcontent FEVTDEBUGHLT --customise=HLTrigger/Configuration/CustomConfigs.L1THLT --customise=HLTrigger/Configuration/CustomConfigs.HLTRECO --era Run2_25ns --processName=HLTRECO --filein file:RelVal_Raw_Fake1_DATA.root --fileout file:RelVal_Raw_Fake1_DATA_HLT_RECO.root'],
                    'hlt_data_Fake2': ['cmsDriver.py RelVal -s L1REPACK:Full --data --scenario=pp -n 10 --conditions auto:run2_hlt_Fake2 --relval 9000,50 --datatier "RAW" --eventcontent RAW --customise=HLTrigger/Configuration/CustomConfigs.L1T --era Run2_2016 --fileout file:RelVal_Raw_Fake2_DATA.root --filein /store/data/Run2016B/JetHT/RAW/v1/000/272/762/00000/C666CDE2-E013-E611-B15A-02163E011DBE.root',
                               'HLTrigger/Configuration/test/OnLine_HLT_Fake2.py',
                               'cmsDriver.py RelVal -s HLT:Fake2,RAW2DIGI,L1Reco,RECO --data --scenario=pp -n 10 --conditions auto:run2_data_Fake2 --relval 9000,50 --datatier "RAW-HLT-RECO" --eventcontent FEVTDEBUGHLT --customise=HLTrigger/Configuration/CustomConfigs.L1THLT --customise=HLTrigger/Configuration/CustomConfigs.HLTRECO --era Run2_2016 --processName=HLTRECO --filein file:RelVal_Raw_Fake2_DATA.root --fileout file:RelVal_Raw_Fake2_DATA_HLT_RECO.root'],
                    'hlt_data_GRun' : ['cmsDriver.py RelVal -s L1REPACK:Full --data --scenario=pp -n 10 --conditions auto:run3_hlt_GRun --relval 9000,50 --datatier "RAW" --eventcontent RAW --customise=HLTrigger/Configuration/CustomConfigs.L1T --era Run3 --fileout file:RelVal_Raw_GRun_DATA.root --filein /store/data/Run2022B/HLTPhysics/RAW/v1/000/355/456/00000/69b26b27-4bd1-4524-bc18-45f7b9b5e076.root',
                               'HLTrigger/Configuration/test/OnLine_HLT_GRun.py',
                               'cmsDriver.py RelVal -s HLT:GRun,RAW2DIGI,L1Reco,RECO --data --scenario=pp -n 10 --conditions auto:run3_data_GRun --relval 9000,50 --datatier "RAW-HLT-RECO" --eventcontent FEVTDEBUGHLT --customise=HLTrigger/Configuration/CustomConfigs.L1THLT --customise=HLTrigger/Configuration/CustomConfigs.HLTRECO --customise=HLTrigger/Configuration/CustomConfigs.customiseGlobalTagForOnlineBeamSpot --era Run3 --processName=HLTRECO --filein file:RelVal_Raw_GRun_DATA.root --fileout file:RelVal_Raw_GRun_DATA_HLT_RECO.root'],
                    'hlt_data_HIon' : ['cmsDriver.py RelVal -s L1REPACK:Full --data --scenario=pp -n 10 --conditions auto:run3_hlt_HIon --relval 9000,50 --datatier "RAW" --eventcontent RAW --customise=HLTrigger/Configuration/CustomConfigs.L1T --era Run3_pp_on_PbPb --fileout file:RelVal_Raw_HIon_DATA.root --filein /store/hidata/HIRun2022A/HITestRaw0/RAW/v1/000/362/321/00000/f467ee64-fc64-47a6-9d8a-7ca73ebca2bd.root --customise=HLTrigger/Configuration/CustomConfigs.customiseL1TforHIonRepackedRAW',
                               'HLTrigger/Configuration/test/OnLine_HLT_HIon.py',
                               'cmsDriver.py RelVal -s HLT:HIon,RAW2DIGI,L1Reco,RECO --data --scenario=pp -n 10 --conditions auto:run3_data_HIon --relval 9000,50 --datatier "RAW-HLT-RECO" --eventcontent FEVTDEBUGHLT --customise=HLTrigger/Configuration/CustomConfigs.L1THLT --customise=HLTrigger/Configuration/CustomConfigs.HLTRECO --customise=HLTrigger/Configuration/CustomConfigs.customiseGlobalTagForOnlineBeamSpot --era Run3_pp_on_PbPb --processName=HLTRECO --filein file:RelVal_Raw_HIon_DATA.root --fileout file:RelVal_Raw_HIon_DATA_HLT_RECO.root'],
                    'hlt_data_PIon' : ['cmsDriver.py RelVal -s L1REPACK:Full --data --scenario=pp -n 10 --conditions auto:run3_hlt_PIon --relval 9000,50 --datatier "RAW" --customise=HLTrigger/Configuration/CustomConfigs.L1T --era Run3 --eventcontent RAW --fileout file:RelVal_Raw_PIon_DATA.root --filein /store/data/Run2022B/HLTPhysics/RAW/v1/000/355/456/00000/69b26b27-4bd1-4524-bc18-45f7b9b5e076.root',
                               'HLTrigger/Configuration/test/OnLine_HLT_PIon.py',
                               'cmsDriver.py RelVal -s HLT:PIon,RAW2DIGI,L1Reco,RECO --data --scenario=pp -n 10 --conditions auto:run3_data_PIon --relval 9000,50 --datatier "RAW-HLT-RECO" --eventcontent FEVTDEBUGHLT --customise=HLTrigger/Configuration/CustomConfigs.L1THLT --customise=HLTrigger/Configuration/CustomConfigs.HLTRECO --customise=HLTrigger/Configuration/CustomConfigs.customiseGlobalTagForOnlineBeamSpot --era Run3 --processName=HLTRECO --filein file:RelVal_Raw_PIon_DATA.root --fileout file:RelVal_Raw_PIon_DATA_HLT_RECO.root'],
                    'hlt_data_PRef' : ['cmsDriver.py RelVal -s L1REPACK:Full --data --scenario=pp -n 10 --conditions auto:run3_hlt_PRef --relval 9000,50 --datatier "RAW" --customise=HLTrigger/Configuration/CustomConfigs.L1T --era Run3 --eventcontent RAW --fileout file:RelVal_Raw_PRef_DATA.root --filein /store/data/Run2022B/HLTPhysics/RAW/v1/000/355/456/00000/69b26b27-4bd1-4524-bc18-45f7b9b5e076.root',
                               'HLTrigger/Configuration/test/OnLine_HLT_PRef.py',
                               'cmsDriver.py RelVal -s HLT:PRef,RAW2DIGI,L1Reco,RECO --data --scenario=pp -n 10 --conditions auto:run3_data_PRef --relval 9000,50 --datatier "RAW-HLT-RECO" --eventcontent FEVTDEBUGHLT --customise=HLTrigger/Configuration/CustomConfigs.L1THLT --customise=HLTrigger/Configuration/CustomConfigs.HLTRECO --customise=HLTrigger/Configuration/CustomConfigs.customiseGlobalTagForOnlineBeamSpot --era Run3 --processName=HLTRECO --filein file:RelVal_Raw_PRef_DATA.root --fileout file:RelVal_Raw_PRef_DATA_HLT_RECO.root'],
                     }

        return addOnTestsHLT
