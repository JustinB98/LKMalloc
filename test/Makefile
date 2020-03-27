CC := gcc
RM := rm
SH := sh
SRCD := src
INCD := include
BIND := bin
LIBD := lib
LKMAL := lkmalloc
LKMALH := $(LKMAL).h
LKMALA := $(LKMAL).a
LKMALAF := $(LIBD)/$(LKMALA)
LDFLAGS := -l $(LIBD)
CFLAGS := -g -Wall -Werror -I $(include)
ALL_SRCF := $(wildcard $(SRCD)/*.c)
ALL_EXEF := $(patsubst $(SRCD)/%.c, $(BIND)/%, $(ALL_SRCF))
ALL_TESTS := $(wildcard test*.sh)

all: $(BLDD) $(ALL_EXEF)

clean:
	$(RM) -rf $(BIN)

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

run_tests:
	@printf "===================== STARTING TESTS ====================\n\n"
	@$(call run_test_directory,$(ALL_TESTS))
	@printf "===================== END TESTS ====================\n\n"

$(BIND)/%: $(SRCD)/%.c
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^
