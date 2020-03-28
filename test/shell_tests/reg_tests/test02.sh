#!/bin/sh

total_pass=0
total_fail=0

assert_success=". shell_tests/assert_successful_exit.sh"
assert_fail=". shell_tests/assert_unsuccessful_exit.sh"

# Double free takes care of everything. Just have to make sure it exists successfully
double_free="bin/double_free_test"
shift_and_free="bin/shift_and_free"

echo "============ TEST02.SH START ==============="

test_name="Double Free 1"
$double_free 1 > /dev/null 2>&1
$assert_success

test_name="Double Free 10"
$double_free 10 > /dev/null 2>&1
$assert_success

test_name="Double Free 1000"
$double_free 1000 > /dev/null 2>&1
$assert_success

test_name="Free With LKF_APPROX"
$shift_and_free 10 0 0x3 5 2> test_file > /dev/null
$assert_success

test_name="Print with Warning Test"
grep "WARNING" test_file > /dev/null 2>&1
$assert_success

test_name="Freeing With LKF_APPROX redzone under"
$shift_and_free 10 0x4 0x1 -8 2> test_file > /dev/null
$assert_success

test_name="Freeing With LKF_APPROX under redzone under printed warning"
grep "WARNING" test_file > /dev/null 2>&1
$assert_success

test_name="Freeing With LKF_APPROX redzone over"
$shift_and_free 10 0x2 0x1 17 2> test_file > /dev/null
$assert_success

test_name="Freeing With LKF_APPROX under redzone over printed warning"
grep "WARNING" test_file > /dev/null 2>&1
$assert_success

test_name="lkmalloc exits with LKF_APPROX and LKF_ERROR"
$shift_and_free 10 0 0xb 5 2> test_file > /dev/null
$assert_fail

test_name="lkmalloc printed a message with LKF_APPROX and LKF_ERROR with bad free"
grep "Terminating" test_file > /dev/null 2>&1
$assert_success

printf "==================== TEST02.SH END ====================\n"
test_type="test02.sh"
rm test_file
. shell_tests/finish.sh
