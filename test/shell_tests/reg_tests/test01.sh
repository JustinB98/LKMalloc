#!/bin/sh

total_pass=0
total_fail=0

assert_success=". shell_tests/assert_successful_exit.sh"
assert_fail=". shell_tests/assert_unsuccessful_exit.sh"

stress_test="bin/stress_test"
assert_block="bin/assert_memory_block"

echo "============ TEST01.SH START ==============="

test_name="Stress test 100"
$stress_test 100 > /dev/null 2>&1
$assert_success

test_name="Stress test 100"
$stress_test 100 > /dev/null 2>&1
$assert_success

test_name="Stress test 1000"
$stress_test 1000 > /dev/null 2>&1
$assert_success

test_name="LKM_INIT test 10 bytes"
$assert_block 10 0x1 0 0 10 > /dev/null 2>&1
$assert_success

test_name="LKM_INIT test 100 bytes"
$assert_block 100 0x1 0 0 100> /dev/null 2>&1
$assert_success

test_name="LKM_UNDER test"
$assert_block 10 0x4 -8 0x6b 8 > /dev/null 2>&1
$assert_success

test_name="LKM_OVER test"
$assert_block 10 0x2 10 0x5a 8 > /dev/null 2>&1
$assert_success

test_name="LKM_UNDER and LKM_OVER check UNDER test"
$assert_block 10 0x6 -8 0x6b 8 > /dev/null 2>&1
$assert_success

test_name="LKM_UNDER and LKM_OVER check OVER test"
$assert_block 10 0x6 10 0x5a 8 > /dev/null 2>&1
$assert_success

test_name="LKM_INIT test size 10"
$assert_block 10 0x1 0 0x0 10 > /dev/null 2>&1
$assert_success

test_name="LKM_INIT test size 1000"
$assert_block 1000 0x1 0 0x0 1000 > /dev/null 2>&1
$assert_success

printf "==================== TEST01.SH END ====================\n"
test_type="test01.sh"
. shell_tests/finish.sh
