#!/bin/sh

total_pass=0
total_fail=0

assert_success=". ./assert_successful_exit.sh"
assert_fail=". ./assert_unsuccesful_exit.sh"

echo "============ TEST01.SH START ==============="

test_name="Stress test 100"
bin/stress_test 100 2&1> /dev/null
$assert_success

test_name="Stress test 1000"
bin/stress_test 1000 2&1> /dev/null
$assert_success

test_name="Stress test 10000"
bin/stress_test 10000 2&1> /dev/null
$assert_success

printf "==================== TEST01.SH end ====================\n"
test_type="test01.sh"
. ./finish.sh
