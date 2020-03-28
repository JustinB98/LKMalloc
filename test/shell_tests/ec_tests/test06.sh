#!/bin/sh

total_pass=0
total_fail=0

assert_success=". shell_tests/assert_successful_exit.sh"
assert_fail=". shell_tests/assert_unsuccessful_exit.sh"

assert_malloc_flags="bin/assert_valid_lkmalloc_flags"

echo "============ TEST06.SH START ==============="

test_name="LKM_PROT_BEFORE and LKM_PROT_AFTER non page size allocation"
$assert_malloc_flags 4095 0x18 0
$assert_success

test_name="LKM_PROT_BEFORE and LKM_PROT_AFTER non page size allocation 2"
$assert_malloc_flags 8193 0x18 0
$assert_success

test_name="LKM_PROT_BEFORE and LKM_PROT_AFTER 1 page size"
$assert_malloc_flags 4096 0x18 1
$assert_success

test_name="LKM_PROT_BEFORE and LKM_PROT_AFTER 2 page size"
$assert_malloc_flags 8192 0x18 1
$assert_success

test_name="LKM_PROT_BEFORE and LKM_PROT_AFTER 3 page size"
$assert_malloc_flags 16384 0x18 1
$assert_success

printf "==================== TEST06.SH END ====================\n"
test_type="test06.sh"
. shell_tests/finish.sh
