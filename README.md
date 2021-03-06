# LKMalloc

```
LKMalloc/
├── Makefile
├── README
├── include/
│   ├── binary_tree.h
│   ├── linked_list.h
│   ├── lk_data.h
│   ├── lk_record.h
│   └── lkmalloc.h
├── lib/
│   ├── lkmalloc.a
│   └── lkmalloc.h
├── src/
│   ├── binary_tree.c
│   ├── linked_list.c
│   ├── lk_data.c
│   ├── lk_record.c
│   └── lkmalloc.c
└── test/
	├── Makefile
	├── include/
	│   ├── lkmalloc.h
	│   └── test_utils.h
	├── lib/
	│   └── lkmalloc.a
	├── shell_tests/
	│   ├── assert_exit_code.sh
	│   ├── assert_successful_exit.sh
	│   ├── assert_unsuccessful_exit.sh
	│   ├── ec_tests/
	│   │   ├── test05.sh
	│   │   └── test06.sh
	│   ├── finish.sh
	│   └── reg_tests/
	│       ├── test01.sh
	│       ├── test02.sh
	│       ├── test03.sh
	│       └── test04.sh
	└── src/
		├── alloc_with_flags_and_modify.c
		├── assert_approx_free.c
		├── assert_memory_block.c
		├── assert_orphan_free.c
		├── assert_valid_lkfree_flags.c
		├── assert_valid_lkmalloc_flags.c
		├── assert_valid_lkreport_flags.c
		├── double_free_test.c
		├── shift_and_free.c
		└── stress_test.c
```

*Note: When initally git cloned, some directories listed above will not exist. Running make or make tests will generate them (directories such as lib)*

# Project structure:

* `include` - lkmalloc headers. lkmalloc.h is the only one that will be exported
  * `binary_tree.h` - Binary tree header. Very basic implementation
  * `linked_list.h` - Specialized implementation of a linked list
  * `lk_data.h` - Contains functions to insert and get records
  * `lk_record.h` - Contains struct definition of a record
  * `lkmalloc.h` - Contains defines for flags and macros to call the internal lkmalloc functions
* `src` - lkmalloc source files
  * `binary_tree.c` - Binary tree implementation (not red-black, just a basic one)
  * `linked_list.c` - Linked list implementation with `O(1)` insertion (more on that later)
  * `lk_data.c` - An interface for using the data structures
  * `lk_record.c` - Getters and setters for the record struct (struct is located in the header)
  * `lkmalloc.c` - The implementation which deals with calling `lk_data.c`, `malloc`, `free`, `mmap`, and `munmap`
* `test` - A test enviroment
  * `lib` - Where `lkmalloc.a` will reside
  * `shell_tests` - Contains shell tests that will execute the c code (source in `test/src,` exe in `test/bin)
   ` * `reg_tests` - Tests for regular program. Only 2 tests will not work with the extra credit program. They are the ones that test that the extra credit flags don't work in the regular program. This means if the extra credit program is tested with the regular tests, `test04.sh` will have 2 failed tests
	  * `test01.sh` - Tests lkmalloc
	  * `test02.sh` - Tests lkfree
	  * `test03.sh` - Tests lkreport
	  * `test04.sh` - Tests argument validation to each function
	* `ec_tests` - Tests for extra credit program
	  * `test05.sh` - Asserts segfaults happen with accessing `PROT_NONE` protection. Also with complex flags.
	  * `test06.sh` - Tests argument validation for the `EXTRA_CREDIT` flags
  * `src` - Contains source c code for testing. These programs take command line arguments to modify behavior of the tests
    * `alloc_with_flags_and_modify.c` - Takes size to malloc, malloc flags, free flags and buffer offset
		* Creates a malloc with specified size and flags, modifies a single char at `buf + offset`. Useful for `LKM_PROT_AFTER` and `LKM_PROT_BEFORE`
	* `assert_approx_free.c` - Takes number of frees to perform `LKF_APPROX`
	  * Basically mallocs a ptr, advance the buffer, then free using `LKF_APPROX`. Does this "free" amount of times. Tests the reports generated using `LKR_APPROX`. If successful, program exits with 0, 1 otherwise.
	* `assert_memory_block` - Takes size to malloc, malloc flags, offset, byte and len
	  * Mallocs a ptr of size `size` with `malloc flags`. Then asserts each byte from `[ptr + offset, ptr + offset + len)` is equal to "byte". Useful for `LKM_UNDER` and `LKM_OVER`.
	* `assert_orphan_free.c` - Takes amount of times to free
	  * Just tries to free NULL `free` amount of times. Then asserts that `lkreport` returns `free` amount of reports when given the `LKR_ORPHAN_FREE` flag.
	* `assert_valid_lkfree_flags.c` - Takes free_flags and assert_success
	  * Allocates some memory using lkmalloc with `LKM_REG` flag. Then tries to free with arguments.
	    * If assert_success is 0, then an assert_fail will be performed and exit with 0 if lkfree returned with `-EINVAL`.
		* If assert_success is non-zero, then an assert_success will be performed and exit with 0 if lkfree did not return with `-EINVAL`.
	* `assert_valid_lkmalloc_flags.c` - Takes size to malloc, malloc flags and assert_success
	  * Does the same as assert_valid_lkfree_flags.c except for lkmalloc and the size for the malloc request is taken. Assert_success means the same here as it did with assert_valid_lkfree_flags.
	* `assert_valid_lkreport_flags.c` - Takes report flags and assert_success
	  * Does the exact same thing as assert_valid_lkfree_flags except with lkreport
	* `double_free_test.c` - Takes times_to_free
	  * Mallocs a pointer then tries to free it "times_to_free" amount of times. Then checks the reports generated by lkreport with the LKR_DOUBLE_FREE flag.
	    * If times_to_free is 1, then the records printed is asserted to be 0, since 1 free will create a match. No double frees
		* If times_to_free is greater than 1, then there should be "times_to_free" amount of records. "times_to_free" - 1 free records and 1 malloc record (malloc records affected by double free will be printed)
	* `shift_and_free.c` - Takes malloc_flags, free_flags and offset
	  * Mallocs a pointer using lkmalloc with "malloc_flags" and tries to free the pointer offsetted by "offset" with "free_flags" passed into lkfree.
	* `stress_test.c` - Takes size (size should be a multiple of 4)
	  1. Mallocs a block of memory of `size * sizeof(size_t *)` to be used as an array. Then mallocs a `size_t*` type to each block of this new memory (basically creating a `size_t*` array).
	  2. Separates each block into quarters and frees the second one, then the last one, then the third one, then the first one. This will test the underlying data structure.

# Makefile
The Makefile is designed to create object files from the source files in the `src` directory, and bundle them up in an archive file (`lkmalloc.a`). There is also a way to test with the rule "tests".

## Makefile Rules
* `all` - Creates the library. This means compiling all source .c files into object files, then archiving them
* `create_lib` - Same as all
* `lib/lkmalloc.h` - Copies `include/lkheader.h` into lib/lkmalloc.h using cp
* `clean` - Uses `rm -rf` to delete the build directory (where `.d` and `.o` files are) and the lib directory
* `ec` - Same as all but compiles with `-DEXTRA_CREDIT`. Used to create the extra credit version of the library
* `depend` - Creates `.d` files of each source file without compiling into `.o` files
* `install_into_test` - Creates the archive file if not all created and copies the archive file and places it in the lib directory in the test environment as well as copying lkmalloc.h and placing it in the include directory of the test environment. The result is the test environment has the files it needs to start working with the lkmalloc API.
* `run_reg_tests` - Install the library (without compiling if library exists). Then calls the clean rule in the test environment and then runs all the regular tests. Useful for running only the regular tests
* `run_ec_tests` - Install the library (without compiling if library exists). Calls the clean rule in the test environment and then runs all the extra credit tests. Useful for only running the extra credit tests.
* `tests` - Recompiles archive file and installs into test environment then runs all the regular tests. Then recompiles archive file again but with `EXTRA_CREDIT` defined. Then clean the test environment then runs the extra credit tests. This produces a lot of output and is useful for only a glance or in use with `less(1)`. Use `run_reg_tests` or `run_ec_tests` to see the output better.
  * Recommended to use `make clean run_reg_tests` to run only the regular tests
  * Recommended to use `make clean ec run_ec_tests` to run only the extra credit tests
* `build|lib|test/lib` - Just to create them since they'll have no files after using git clone
* `%.o` - Object files are compiled with `CFLAGS`, which is `-g` `-Wall` and `-Werror`. If `ec` rule was called, then `-DEXTRA_CREDIT` will also be included in `CFLAGS`.
* `%.d` - Dependencies files are created using `-M` flag with `gcc` and the target is renamed to `build/%.o`
* `lkmalloc.a` - Created with `ar rcs` and uses all `.o` files created from the source files in `src`

## Makefile Rules in Test environment
This Makefile was designed to create a binary based on a source file.
There will be as many source files as binary files.
The binary files are compiled with the command `gcc -g -Wall -Werror -I include -o bin/code src/code.c lib/lkmalloc.a` assuming the src file was code.c. For some reason I didn't have to use `-l` or `-L` flags.
Passing the `lkmalloc.a` as another source worked out fine.
	* `all` - Creates bin directory and all the binary files based off a c file
	* `clean` - Removes bin directory
	* `run_test_directory` - Not a rule but a function. Takes in the shell scripts to run and runs them while keeping track which ones passed (zero exit code). Prints how many passed vs how many there were at the end
	* `run_reg_tests` - Runs all tests located in `shell_tests/reg_tests`
	* `run_ec_tests` - Runs all extra tests located in `shell_tests/ec_tests`
	* `executables` - Depends on a corresponding .c file and compiles with `gcc` without a `-c` flag. File will be named `bin/` (c source file name)

# LKMalloc Data Structure Implementation
lkmalloc uses binary trees and a linked list to keep track of records
* Active Records
	* A binary tree which stores all the active malloc records. Uses the pointer the user is given as a key. Chose a binary tree because records are expected to go in and out of this structure so we need a fast data structure to handle searching, insertion and deletion quickly. A better implementation would be a red black binary tree. However, this is NOT the implementation found in `src/binary_tree.c`
* Completed Records
  * Binary tree that stores malloc records that have been fufilled.
	  The key is the pointer the user is given. When a free request comes in and a malloc record matches it, then it is removed from the active records tree and placed in the completed records tree.
	  If a the pointer is not found in the active records tree, then the completed records tree is searched and if found, then it's a double free, else an orphaned free.
	  When a record is completed and put into this tree, the previous node is replaced with the new data. This is because if malloc returns a pointer and then is freed, and then malloc is called again and returns that same pointer, then it needs to be replaced in the completed records tree.
* All Records
  * Doubly Linked list that stores all the records ever generated by the `lk*` functions. This linked list needs to be fast at inserted so it's modified to insert a new node at the first of the list. 
	  In other words, setting a new node's next as the root then making the new node as the root. 
	  This creates a backwards tree, so an end pointer needs to be kept too, so the first time a node is created, make that node the "end" node.
	  Then when iterating, start at this "end" node and go backwards. Since all records are added as time goes on, this will create a way to iterate through all the records sorted by timestamp in `O(n)` times.
	  There is no reason to remove anything in this structure so there's no edge cases of removing nodes.
	  Right now, it's useless to store the records that returned an error, but this is future proofing, in case lkreport has a future flag that will display all records that returned an error

# LKMalloc Return Values
* `0` on success
* `-EINVAL` if invalid flags
* `-ENOMEM` if caller's request could not be satified by either `malloc` or `mmap` (`mmap` only for extra credit)
* `-ENFILE` if record could not be created

# LKMalloc Invalid Arguments

* Pointer passed is `NULL`
* Flags contains a bit higher than the highest defined flag (`LKM_UNDER` for regular program, `LKM_BEFORE` for extra credit
* For the extra credit program, if flags both `LKM_PROT_BEFORE` and `LKM_PROT_AFTER` is defined and size is not a multiple of page size
	* NOTE - If `LKM_OVER` or `LKM_UNDER` is in flags with the extra credit flags, then the size must be `page size multiple - 8` to make room for the redzone. If `LKM_OVER` and `LKM_UNDER` is is flags, then the size will be `a page size multiple - 16` (`8*2` for 2 redzones)
	* These tests are located in `test/shell_tests/ec_tests/test05.sh`

# LKFree Return Values
* `0` on succcess
* `-EINVAL` if invalid flags
* `-ENOENT` if free record couldn't be found
* `-ENFILE` if record could not be created

# LKFree Invalid Arguments
* Pointer passed is NULL.
* `LKF_WARN` is passed but `LKF_APPROX` isn't
* `LKF_ERROR` is passed but `LKF_UNKNOWN` or `LKF_WARN` wasn't passed
* Flags passed has a bit higher than the highest defined flag (`LKF_ERROR` for now)

# LKReport Return Values
* `0` or positive number on success. This return value means the number of records
* `-errno` on printing errors

# LKReport Invalid Arguments
* If fd is not a valid file descriptor
* If flags contains both `LKR_APPROX` and `LKR_BAD_FREE` (since `LKR_BAD_FREE` will do what `LKR_APPROX` does plus print the malloc records)
* If flags contains a bit higher than the highest defined flag (`LKR_APPROX` for now)
* `LKR_NONE` is not an invalid arguments and will just return `0` upon seeing it

# LKReport Record Displaying Rules
LKReport will display the following records with corresponding flags:
## Malloc Records
* `LKR_SERIOUS` - Malloc records that weren't freed
* `LKR_MATCH` - Malloc records that were freed EXACTLY once. This means if the records was freed more than once, it's not a perfect match. This was chosen because it's better to not print it to alarm the user that the record was not printed
* `LKR_DOUBLE_FREE` - Malloc records that were freed more than once (double free is serious and all records that were freed more than once is a serious error)
* `LKR_APPROX` - Malloc records that were freed using `LKF_APPROX`

## Free Records
* `LKR_MATCH` - Free records that found the passed pointer in the active records tree
* `LKR_BAD_FREE` - Free records that were freed using `LKR_APPROX` and were not perfect matches
* `LKR_APPROX` - Free records that were freed using `LKR_APPROX` and were not perfect matches
* `LKR_ORPHAN_FREE` - Free records that did not found the pointer in both the active records tree and completed records tree. NULL is considered an orphaned free.
* `LKR_DOUBLE_FREE` - Free records that were found in the completed records tree

# Other Notes

* Regarding the extra credit section, mmap would only be called once as opposed to calling twice (or three times) to grab both the user block and the protected block. This is because (I don't believe) `mmap(2)` is guaranteed to return a block at the specified address. When I tried this originally, I could not get it to work (maybe I was doing something wrong). What happens in the program as of now is using `mmap(2)` to get the entire block needed, then using `mprotect(2)`, set the region to `PROT_NONE`.
* LKReport will report at the end of the program execution (using `atexit(3)`) with the default flags `LKR_SERIOUS`, `LKR_BAD_FREE`, `LKR_ORPHAN_FREE` and `LKR_DOUBLE_FREE`. Basically all the flags that show something is wrong. They are printed to stderr.
* `lkmalloc.h` has `#include <stdlib.h>` in order to get `u_int` definition. lkmalloc also has the extra credit flags already defined, however, they can't be used in the regular program. This is because I couldn't find a way to run the preprocessor on `#ifdef EXTRA_CREDIT` and not get rid of the flags
* If the pointer passed to lkmalloc is pointing to garbage, then printing that pointer will cause valgrind to complain.
* The times are gotten using `gettimeofday(2)`. Printed as `secs.usecs`
* If malloc fails for creating the data structure, a message is printed then `exit(EXIT_FAILURE)` is called because there's not much else to do.
* If malloc fails for creating malloc, `-ENFILE` is returned. This is because if `-ENOMEM` is returned, the caller might think that malloc failed for creating their space so they might take the wrong actions. Mainly used to differentiate between the two errors.
* If malloc fails for a caller's request, then `-ENOMEM` is returned, but the record is still put in the all records list to keep the option for printing records with error return values in the future.
* If `gettimeofday(2)` fails, the program will continue. Using `perror`, a message will be printed out to stderr and the timeval structure is zeroed out using `memset(3)`.
* `atexit(3)` is used to clean up the program. If the `atexit(3)` fails, then `exit(EXIT_FAILURE)` is called because it would be considered a fatal error if the program could not clean up it's memory and data structure. A message will be printed to stderr.
