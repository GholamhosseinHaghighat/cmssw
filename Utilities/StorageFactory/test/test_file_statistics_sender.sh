#!/bin/bash

# Pass in name and status
function die { echo $1: status $2 ;  exit $2; }

LOCAL_TEST_DIR=${CMSSW_BASE}/src/Utilities/StorageFactory/test
LOCAL_TMP_DIR=${CMSSW_BASE}/tmp/${SCRAM_ARCH}

pushd ${LOCAL_TMP_DIR}

#setup files used in tests
cmsRun ${LOCAL_TEST_DIR}/make_test_files_cfg.py &> make_test_files.log || die "cmsRun make_test_files_cfg.py" $?
rm make_test_files.log
cmsRun ${LOCAL_TEST_DIR}/make_2nd_file_cfg.py &> make_2nd_file.log || die "cmsRun make_2nd_file_cfg.py" $?
rm make_2nd_file.log

cmsRun ${LOCAL_TEST_DIR}/test_single_file_statistics_sender_cfg.py &> test_single_file_statistics_sender.log || die "cmsRun test_single_file_statistics_sender_cfg.py" $?
grep -q '"file_lfn":"file:stat_sender_first.root"' test_single_file_statistics_sender.log || die "no StatisticsSenderService output for single file" 1
rm test_single_file_statistics_sender.log

cmsRun ${LOCAL_TEST_DIR}/test_multiple_files_file_statistics_sender_cfg.py &> test_multiple_files_file_statistics_sender.log || die "cmsRun test_multiple_files_file_statistics_sender_cfg.py" $?
grep -q '"file_lfn":"file:stat_sender_b.root"' test_multiple_files_file_statistics_sender.log || die "no StatisticsSenderService output for file b in multiple files" 1
grep -q '"file_lfn":"file:stat_sender_c.root"' test_multiple_files_file_statistics_sender.log || die "no StatisticsSenderService output for file c in multiple files" 1
grep -q '"file_lfn":"file:stat_sender_d.root"' test_multiple_files_file_statistics_sender.log || die "no StatisticsSenderService output for file d in multiple files" 1
grep -q '"file_lfn":"file:stat_sender_e.root"' test_multiple_files_file_statistics_sender.log || die "no StatisticsSenderService output for file e in multiple files" 1
rm test_multiple_files_file_statistics_sender.log

cmsRun ${LOCAL_TEST_DIR}/test_multi_file_statistics_sender_cfg.py &> test_multi_file_statistics_sender.log || die "cmsRun test_multi_file_statistics_sender_cfg.py" $?
grep -q '"file_lfn":"file:stat_sender_first.root"' test_multi_file_statistics_sender.log || die "no StatisticsSenderService output for file first in multi file" 1
grep -q '"file_lfn":"file:stat_sender_second.root"' test_multi_file_statistics_sender.log || die "no StatisticsSenderService output for file second in multi file" 1
rm test_multi_file_statistics_sender.log

cmsRun ${LOCAL_TEST_DIR}/test_secondary_file_statistics_sender_cfg.py &> test_secondary_file_statistics_sender.log || die "cmsRun test_secondary_file_statistics_sender_cfg.py" $?
grep -q '"file_lfn":"file:stat_sender_first.root"' test_secondary_file_statistics_sender.log || die "no StatisticsSenderService output for file 'first' in secondary files"  1
grep -q '"file_lfn":"file:stat_sender_b.root"' test_secondary_file_statistics_sender.log || die "no StatisticsSenderService output for file 'b' in secondary files"  1
grep -q '"file_lfn":"file:stat_sender_c.root"' test_secondary_file_statistics_sender.log || die "no StatisticsSenderService output for file 'c' in secondary files"  1
grep -q '"file_lfn":"file:stat_sender_d.root"' test_secondary_file_statistics_sender.log || die "no StatisticsSenderService output for file 'd' in secondary files"  1
grep -q '"file_lfn":"file:stat_sender_e.root"' test_secondary_file_statistics_sender.log || die "no StatisticsSenderService output for file 'e' in secondary files"  1
rm test_secondary_file_statistics_sender.log

popd