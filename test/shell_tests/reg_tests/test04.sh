#!/bin/sh

total_pass=0
total_fail=0

assert_success=". shell_tests/assert_successful_exit.sh"
assert_fail=". shell_tests/assert_unsuccessful_exit.sh"

assert_malloc_flags="bin/assert_valid_lkmalloc_flags"
assert_free_flags="bin/assert_valid_lkfree_flags"
assert_report_flags="bin/assert_valid_lkreport_flags"

echo "============ TEST04.SH START ==============="

test_name="Assert Invalid Malloc Flags (Large Number)"
$assert_malloc_flags 10 0xfff000 0 > /dev/null 2>&1
$assert_success

test_name="Assert Invalid Free Flags (Large Number)"
$assert_free_flags 0xfff000 0 > /dev/null 2>&1
$assert_success

test_name="Assert Invalid Report Flags (Large Number)"
$assert_free_flags 0xfff000 0 > /dev/null 2>&1
$assert_success

printf "==================== TEST04.SH END ====================\n"
test_type="test04.sh"
. shell_tests/finish.sh
