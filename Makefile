PATH := /opt/gcc-linaro-4.9-2015.02-3-x86_64_aarch64-elf/bin:$(PATH)

ARMGNU ?= aarch64-elf

BUILDDIR = build
EXPORTDIR = export

LIB_FILES := $(shell find src/driver -name '*.c')
LIB_SOURCES := $(LIB_FILES:.c=.o)
LIB_OBJS := $(foreach src, $(LIB_SOURCES), $(BUILDDIR)/$(src))

TEST_CFILES := $(shell find src/test -name '*.c')
TEST_CPPFILES := $(shell find src/test -name '*.cpp')
TEST_SOURCES := $(TEST_CFILES:.c=.o) $(TEST_CPPFILES:.cpp=.o)
TEST_OBJS := $(BUILDDIR)/boot.o $(foreach src, $(TEST_SOURCES), $(BUILDDIR)/$(src))

#####################################################
#				Building for target
#####################################################

ASS_FLAGS = --warn --fatal-warnings
C_FLAGS = -Wall -O2 -nostdlib -nostartfiles -fno-exceptions -ffreestanding -Isrc
CXX_FLAGS = $(C_FLAGS) -fno-rtti -std=c++11


all: $(BUILDDIR)/test_bl.srec export

$(BUILDDIR)/boot.o: boot.s
	@echo "[ASM] $<"
	@mkdir -p "$(dir $@)"
	@$(ARMGNU)-as $(ASS_FLAGS) boot.s -o $(BUILDDIR)/boot.o

$(BUILDDIR)/%.o: %.c
	@echo "[C] $<"
	@mkdir -p "$(dir $@)"
	@$(ARMGNU)-gcc $(C_FLAGS) -Wa,-adhln -g -c $< -o $@ > $(basename $@).asm
	
$(BUILDDIR)/%.o: %.cpp
	@echo "[CXX] $<"
	@mkdir -p "$(dir $@)"
	@$(ARMGNU)-g++ $(CXX_FLAGS) -Wa,-adhln -g -c $< -o $@ > $(basename $@).asm

$(BUILDDIR)/libhifiberry.a: $(LIB_OBJS)
	@echo "[LIB] $<"
	@$(ARMGNU)-ar rcs $@ $(LIB_OBJS)

export: $(BUILDDIR)/libhifiberry.a
	@mkdir -p $(EXPORTDIR)
	@echo "[EXPORT] $(EXPORTDIR)/libhifiberry.a"
	@cp $< $@
	@echo "[EXPORT] $(EXPORTDIR)/hifiberry.h"
	@cp src/driver/hifiberrydacplus.h $(EXPORTDIR)/hifiberry.h
	
$(BUILDDIR)/test_bl.srec: $(BUILDDIR)/libhifiberry.a $(TEST_OBJS)
	@echo "[SREC] $<"
	@$(ARMGNU)-ld $(TEST_OBJS) -L$(BUILDDIR) -lhifiberry  -T memmap_bl -o $(BUILDDIR)/test_bl.elf
	@$(ARMGNU)-objcopy --srec-forceS3 $(BUILDDIR)/test_bl.elf -O srec $(BUILDDIR)/test_bl.srec
	@$(ARMGNU)-objdump -D $(BUILDDIR)/test_bl.elf > $(BUILDDIR)/test_bl.list
	@$(ARMGNU)-nm -n $(BUILDDIR)/test_bl.elf > $(BUILDDIR)/test_bl.map


#####################################################
#				Generate documentation
#####################################################

DOXYGENDIR = doxy

# Generate documentation using doxygen
doxygen:
	@doxygen Doxyfile


#####################################################
# 					Cleaning up
#####################################################

clean: clean_build clean_doxygen clean_export

clean_build:
	rm -rf $(BUILDDIR)

clean_doxygen:
	rm -rf $(DOXYGENDIR)

clean_export:
	rm -rf $(EXPORTDIR)
