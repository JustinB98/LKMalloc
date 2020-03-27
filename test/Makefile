CC := gcc
RM := rm
SH := sh
SRCD := src
INCD := include
BIND := bin
LIBD := lib
SHLTSTD := shell_tests
REGTSTD := $(SHLTSTD)/reg_tests
ECTSTD := $(SHLTSTD)/ec_tests
LKMAL := lkmalloc
LKMALH := $(LKMAL).h
LKMALA := $(LKMAL).a
LKMALAF := $(LIBD)/$(LKMALA)
CFLAGS := -g -Wall -Werror -I $(INCD)
ALL_SRCF := $(wildcard $(SRCD)/*.c)
ALL_EXEF := $(patsubst $(SRCD)/%.c, $(BIND)/%, $(ALL_SRCF))
ALL_REG_TESTS := $(wildcard $(REGTSTD)/test*.sh)
ALL_EC_TESTS := $(wildcard $(ECTSTD)/test*.sh)

all: $(BIND) $(ALL_EXEF)

clean:
	$(RM) -rf $(BIND)

$(BIND):
	mkdir -p $@

run_test_directory = \
		total_tests=0 ; \
		total_passed=0 ; \
		for test_file in $(1) ; do \
			total_tests=$$((total_tests+1)) ; \
			$(SH) $$test_file ; \
			exit=$$? ; \
			if [ $$exit -eq 0 ]; then \
				total_passed=$$((total_passed+1)) ; \
			fi \
		done ; \
		if [ $$total_tests -eq $$total_passed ]; then \
			printf "\033[0;32m" ; \
		else \
			printf "\033[0;31m" ; \
		fi ; \
		printf "\n$$total_passed / $$total_tests test files passed" ; \
		printf "\033[0m" ;

run_reg_tests: all
	@printf "===================== STARTING REG TESTS ====================\n\n"
	@$(call run_test_directory,$(ALL_REG_TESTS))
	@printf "\n\n===================== END REG TESTS ====================\n"

run_ec_tests: all
	@printf "===================== STARTING EC TESTS ====================\n\n"
	@$(call run_test_directory,$(ALL_EC_TESTS))
	@printf "\n\n===================== END EC TESTS ====================\n"

$(BIND)/%: $(SRCD)/%.c
	$(CC) $(CFLAGS) -o $@ $^ $(LKMALAF)
