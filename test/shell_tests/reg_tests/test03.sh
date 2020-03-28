#!/bin/sh

total_pass=0
total_fail=0

assert_success=". shell_tests/assert_successful_exit.sh"
assert_fail=". shell_tests/assert_unsuccessful_exit.sh"

assert_orphan_free="bin/assert_orphan_free"
assert_approx_free="bin/assert_approx_free"

echo "============ TEST03.SH START ==============="

test_name="Assert 10 orphan records"
$assert_orphan_free 10 > /dev/null 2>&1
$assert_success

test_name="Assert 100 LKR_ORPHAN_FREE records"
$assert_orphan_free 100 > /dev/null 2>&1
$assert_success

test_name="Assert 10 LKR_APPROX records"
$assert_approx_free 10 > /dev/null 2>&1
$assert_success

test_name="Assert 100 LKR_APPROX records"
$assert_approx_free 100 > /dev/null 2>&1
$assert_success

printf "==================== TEST03.SH END ====================\n"
. shell_tests/finish.sh
