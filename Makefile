CC := gcc
RM := rm
SH := sh
AR := ar
CP := cp
MAKE := make
SRCD := src
LIBD := lib
INCD := include
BIND := .
BLDD := build
TSTD := test
MODULENAME := lkmalloc
LKMALHDR := $(MODULENAME).h
HEADERF := $(INCD)/$(LKMALHDR)
LIBHDRF := $(LIBD)/$(LKMALHDR)
INCF := -I $(INCD)
CFLAGS := -g -Wall -Werror
DEPFLAGS := -M
ARCFLAGS := rcs
ARCNAME = $(MODULENAME).a
ARCF := $(LIBD)/$(ARCNAME)
ECFLAGS := -DEXTRA_CREDIT
ALL_SRCF := $(wildcard $(SRCD)/*.c)
ALL_OBJF := $(patsubst $(SRCD)/%.c, $(BLDD)/%.o, $(ALL_SRCF))
ALL_EXISTING_DEPS := $(wildcard $(BLDD)/*.d)
ALL_DEPS := $(patsubst $(SRCD)/%.c, $(BLDD)/%.d, $(ALL_SRCF))

.PHONY: all clean depend tests run_reg_tests run_ec_tests ec create_lib install_into_test

all: create_lib

create_lib: $(BLDD) $(LIBD) $(LIBHDRF) $(ARCF)

$(LIBHDRF): $(HEADERF)
	$(CP) $< $@

clean:
	$(RM) -rf $(BLDD) $(LIBD)

ec: CFLAGS += $(ECFLAGS)
ec: all

depend: $(BLDD) $(ALL_DEPS)

install_into_test: create_lib
	$(CP) $(LIBHDRF) $(TSTD)/$(INCD)/$(LKMALHDR)
	$(CP) $(ARCF) $(TSTD)/$(LIBD)/$(ARCNAME)

tests: clean install_into_test
	$(MAKE) -C $(TSTD) clean all run_reg_tests
	$(MAKE) clean ec install_into_test
	$(MAKE) -C $(TSTD) clean all run_ec_tests

$(BLDD) $(LIBD) $(TSTD)/$(LIBD):
	mkdir -p $@

-include $(ALL_EXISTING_DEPS)

$(BLDD)/%.o: $(SRCD)/%.c
	$(CC) $(INCF) $(CFLAGS) -c -o $@ $<

$(BLDD)/%.d: $(SRCD)/%.c
	$(CC) $(INCF) $(DEPFLAGS) -MT $(BLDD)/$*.o -o $@ $<

$(ARCF): $(ALL_OBJF)
	$(AR) $(ARCFLAGS) $@ $^
