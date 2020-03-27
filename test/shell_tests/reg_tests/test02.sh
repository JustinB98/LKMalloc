#!/bin/sh

total_pass=0
total_fail=0

assert_success=". shell_tests/assert_successful_exit.sh"
assert_fail=". shell_tests/assert_unsuccessful_exit.sh"

# Double free takes care of everything. Just have to make sure it exists successfully
double_free="bin/double_free_test"

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

printf "==================== TEST02.SH END ====================\n"
test_type="test02.sh"
. shell_tests/finish.sh
