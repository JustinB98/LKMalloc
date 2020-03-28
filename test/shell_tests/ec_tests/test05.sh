#!/bin/sh

total_pass=0
total_fail=0

assert_success=". shell_tests/assert_successful_exit.sh"
assert_fail=". shell_tests/assert_unsuccessful_exit.sh"
assert_exit_code=". shell_tests/assert_exit_code.sh"

alloc_with_flags_and_modify="bin/alloc_with_flags_and_modify"
assert_memory_block="bin/assert_memory_block"

printf "==================== TEST05.SH START ====================\n"

test_name="LKM_PROT_AFTER SIGSEGV less than page size (4096)"
error_code=139
$alloc_with_flags_and_modify 10 0x8 0 10 > /dev/null 2>&1
$assert_exit_code

test_name="LKM_PROT_BEFORE SIGSEGV less than page size (4096)"
error_code=139
$alloc_with_flags_and_modify 10 0x10 0 -1 > /dev/null 2>&1
$assert_exit_code

test_name="LKM_PROT_AFTER SIGSEGV with more than page size"
error_code=139
$alloc_with_flags_and_modify 4097 0x8 0 4097 > /dev/null 2>&1
$assert_exit_code

test_name="LKM_PROT_BEFORE SIGSEGV with more than page size"
error_code=139
$alloc_with_flags_and_modify 4097 0x10 0 -1 > /dev/null 2>&1

test_name="LKM_PROT_AFTER less than page size (4096)"
$alloc_with_flags_and_modify 10 0x8 0 9 > /dev/null 2>&1
$assert_success

test_name="LKM_PROT_BEFORE less than page size (4096)"
$alloc_with_flags_and_modify 10 0x10 0 0 > /dev/null 2>&1
$assert_success

test_name="LKM_PROT_AFTER with more than page size"
$alloc_with_flags_and_modify 4097 0x8 0 4096 > /dev/null 2>&1
$assert_success

test_name="LKM_PROT_BEFORE SIGSEGV with more than page size"
$alloc_with_flags_and_modify 4097 0x10 0 0 > /dev/null 2>&1
$assert_success

test_name="LKM_PROT_BEFORE and LKM_PROT_AFTER with page size"
$alloc_with_flags_and_modify 4096 0x18 0 3000 > /dev/null 2>&1
$assert_success

test_name="LKM_PROT_BEFORE and LKM_PROT_AFTER with page_size*2"
$alloc_with_flags_and_modify 8192 0x18 0 8191 > /dev/null 2>&1
$assert_success

test_name="LKM_PROT_BEFORE and LKM_PROT_AFTER with page_size access before"
error_code=139
$alloc_with_flags_and_modify 4096 0x18 0 -1 > /dev/null 2>&1
$assert_exit_code

test_name="LKM_PROT_BEFORE and LKM_PROT_AFTER with page_size access after"
error_code=139
$alloc_with_flags_and_modify 4096 0x18 0 4096 > /dev/null 2>&1
$assert_exit_code

test_name="LKM_UNDER, LKM_PROT_BEFORE and LKM_PROT_AFTER"
$assert_memory_block 4088 0x1c -8 0x6b 8 > /dev/null 2>&1
$assert_success

test_name="LKM_OVER, LKM_PROT_BEFORE and LKM_PROT_AFTER"
$assert_memory_block 4088 0x1a 4088 0x5a 8 > /dev/null 2>&1
$assert_success

test_name="LKM_OVER, LKM_UNDER, LKM_PRO_BEFORE and LKM_PROT_AFTER checking under"
$assert_memory_block 4080 0x1f -8 0x6b 8 > /dev/null 2>&1
$assert_success

test_name="LKM_OVER, LKM_UNDER, LKM_PRO_BEFORE and LKM_PROT_AFTER checking over"
$assert_memory_block 4080 0x1f 4080 0x5a 8 > /dev/null 2>&1
$assert_success

test_name="LKM_OVER, LKM_UNDER, LKM_PRO_BEFORE and LKM_PROT_AFTER SIGSEGV before"
error_code=139
$alloc_with_flags_and_modify 4080 0x1f 0 -9 > /dev/null 2>&1
$assert_exit_code

test_name="LKM_OVER, LKM_UNDER, LKM_PRO_BEFORE and LKM_PROT_AFTER SIGSEGV after"
error_code=139
$alloc_with_flags_and_modify 4080 0x1f 0 4096 > /dev/null 2>&1
$assert_exit_code

printf "==================== TEST05.SH END ====================\n"
test_type="test05.sh"
. shell_tests/finish.sh

