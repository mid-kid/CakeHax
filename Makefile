# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.

rwildcard = $(foreach d, $(wildcard $1*), $(filter $(subst *, %, $2), $d) $(call rwildcard, $d/, $2))
getSize = $(lastword $(shell arm-none-eabi-size -A --target=binary $1))

# This should be set externally
name ?= Cakes.dat
filepath ?=
dir_out ?= .

CC := arm-none-eabi-gcc
AS := arm-none-eabi-as
OC := arm-none-eabi-objcopy

dir_source := source
dir_build := build
dir_rop3ds := rop3ds

ARM9FLAGS := -mcpu=arm946e-s -march=armv5te
ARM11FLAGS := -mcpu=mpcore
ASFLAGS := -mlittle-endian
CFLAGS := -marm $(ASFLAGS) -O2 -std=c11 -MMD -MP -fshort-wchar -Wall -Wextra -Wno-main -DLAUNCHER_PATH='"$(filepath)$(name)"'
LDFLAGS := -nostartfiles -flto -fwhole-program

get_objects = $(patsubst $(dir_source)/%.s, $(dir_build)/%.o, \
			  $(patsubst $(dir_source)/%.c, $(dir_build)/%.o, $1))

objects := $(call get_objects, $(wildcard $(dir_source)/*.s $(dir_source)/*.c))

objects_payload_table := $(dir_build)/payload/jump_table.o
objects_payload_arm9 := $(call get_objects, \
				   $(call rwildcard, $(dir_source)/payload/arm9, *.s *.c))

versions := mset_4x mset_4x_dg mset_4x_ndg mset_6x

rops := $(foreach ver, $(versions), $(dir_build)/$(ver)/rop.dat)

PAYLOAD_TABLE := $(dir_build)/payload/jump_table.bin
PAYLOAD_ARM9 := $(dir_build)/payload/arm9/main.bin

.SECONDARY:

.PHONY: all
all: launcher

.PHONY: launcher
launcher: $(dir_out)/$(name)

.PHONY: bigpayload
bigpayload: $(dir_build)/bigpayload.built

.PHONY: clean
clean:
	rm -rf $(dir_out)/$(name) $(dir_build)
	@$(MAKE) -C $(dir_rop3ds) clean

# Big payload
$(dir_build)/bigpayload.built: $(dir_out)/$(name) $(PAYLOAD_TABLE) $(PAYLOAD_ARM9)
	dd if=$(PAYLOAD_TABLE) of=$< bs=512 seek=144
	cat $(PAYLOAD_ARM9) >> $<
	@touch $@

# Throw everything together
$(dir_out)/$(name): $(rops) $(dir_build)/mset/main.bin $(dir_build)/spider/main.bin
	touch $@
	dd if=$(dir_build)/spider/main.bin of=$@ bs=512 seek=0
	dd if=$(dir_build)/mset_4x/rop.dat of=$@ bs=512 seek=32
	dd if=$(dir_build)/mset_4x_dg/rop.dat of=$@ bs=512 seek=34
	dd if=$(dir_build)/mset_4x_ndg/rop.dat of=$@ bs=512 seek=36
	dd if=$(dir_build)/mset_6x/rop.dat of=$@ bs=512 seek=40
	dd if=$(dir_build)/mset/main.bin of=$@ bs=512 seek=64

# MSET ROPs
$(dir_build)/mset_%/rop.dat: rop_param = MSET_$(shell echo $* | tr a-z A-Z)
$(dir_build)/mset_%/rop.dat:
	@make -C rop3ds LoadCodeMset.dat ASFLAGS="-D$(rop_param) -DARM_CODE_OFFSET=0x8000"
	@mkdir -p "$(@D)"
	@mv rop3ds/LoadCodeMset.dat $@

# Create bin from elf
$(dir_build)/%.bin: $(dir_build)/%.elf
	$(OC) -S -O binary $< $@

$(dir_build)/payload/jump_table.elf: $(objects_payload_table)
	$(LINK.o) $(OUTPUT_OPTION) $^

# Different flags for different things
$(dir_build)/payload/arm9/main.elf: ASFLAGS := $(ARM9FLAGS) $(ASFLAGS)
$(dir_build)/payload/arm9/main.elf: CFLAGS := $(ARM9FLAGS) $(CFLAGS)
$(dir_build)/payload/arm9/main.elf: $(objects_payload_arm9)
	# FatFs requires libgcc for __aeabi_uidiv
	$(LINK.o) -T linker_payload.ld $(OUTPUT_OPTION) $^

# The arm11 payloads
$(dir_build)/%/main.elf: ASFLAGS := $(ARM11FLAGS) $(ASFLAGS)

$(dir_build)/mset/main.elf: CFLAGS := -DENTRY_MSET $(ARM11FLAGS) $(CFLAGS)
$(dir_build)/mset/main.elf: $(patsubst $(dir_build)/%, $(dir_build)/mset/%, $(objects))
	$(LINK.o) -T linker_mset.ld $(OUTPUT_OPTION) $^

$(dir_build)/spider/main.elf: CFLAGS := -DENTRY_SPIDER $(ARM11FLAGS) $(CFLAGS)
$(dir_build)/spider/main.elf: $(patsubst $(dir_build)/%, $(dir_build)/spider/%, $(objects))
	$(LINK.o) -T linker_spider.ld $(OUTPUT_OPTION) $^

# Fatfs requires to be built in thumb
$(dir_build)/payload/arm9/fatfs/%.o: $(dir_source)/payload/arm9/fatfs/%.c
	@mkdir -p "$(@D)"
	$(COMPILE.c) -mthumb -mthumb-interwork -Wno-unused-function $(OUTPUT_OPTION) $<

$(dir_build)/payload/arm9/fatfs/%.o: $(dir_source)/payload/arm9/fatfs/%.s
	@mkdir -p "$(@D)"
	$(COMPILE.s) -mthumb -mthumb-interwork $(OUTPUT_OPTION) $<

$(dir_build)/payload/%.o: $(dir_source)/payload/%.c
	@mkdir -p "$(@D)"
	$(COMPILE.c) -flto $(OUTPUT_OPTION) $<

.SECONDEXPANSION:
$(dir_build)/payload/%.o: $(dir_source)/payload/%.s
	@mkdir -p "$(@D)"
	$(COMPILE.s) $(OUTPUT_OPTION) $<

.SECONDEXPANSION:
$(dir_build)/%.o: $(dir_source)/$$(notdir $$*).c $(PAYLOAD_TABLE) $(PAYLOAD_ARM9)
	@mkdir -p "$(@D)"
	$(COMPILE.c) -flto $(OUTPUT_OPTION) -DPAYLOAD_TABLE_SIZE=$(call getSize,$(PAYLOAD_TABLE)) -DPAYLOAD_ARM9_SIZE=$(call getSize,$(PAYLOAD_ARM9)) $<

.SECONDEXPANSION:
$(dir_build)/%.o: $(dir_source)/$$(notdir $$*).s
	@mkdir -p "$(@D)"
	$(COMPILE.s) $(OUTPUT_OPTION) $<

include $(call rwildcard, $(dir_build), *.d)
