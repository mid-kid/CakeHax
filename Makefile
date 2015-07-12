rwildcard = $(foreach d, $(wildcard $1*), $(filter $(subst *, %, $2), $d) $(call rwildcard, $d/, $2))

# This should be set externally
name ?= Cakes.dat
path ?=
dir_out ?= .

CC := arm-none-eabi-gcc
AS := arm-none-eabi-as
LD := arm-none-eabi-ld
OC := arm-none-eabi-objcopy

PYTHON2 := python
PYTHON_VER_MAJOR := $(word 2, $(subst ., , $(shell python --version 2>&1)))
ifneq ($(PYTHON_VER_MAJOR), 2)
	PYTHON2 := python2
endif

dir_source := source
dir_build := build
dir_tools := p3ds

ARM9FLAGS := -mcpu=arm946e-s -march=armv5te
ARM11FLAGS := -mcpu=mpcore
ASFLAGS := -mlittle-endian
CFLAGS := -marm $(ASFLAGS) -O2 -std=c11 -MMD -MP -fno-builtin -fshort-wchar -Wall -Wextra -Wno-main -DLAUNCHER_PATH='"$(path)$(name)"'

get_objects = $(patsubst $(dir_source)/%.s, $(dir_build)/%.o, \
			  $(patsubst $(dir_source)/%.c, $(dir_build)/%.o, $1))

objects := $(call get_objects, $(wildcard $(dir_source)/*.s $(dir_source)/*.c))

objects_mset_4x := $(patsubst $(dir_build)/%, $(dir_build)/mset_4x/%, \
				   $(objects))
objects_mset_4x_dg := $(patsubst $(dir_build)/%, $(dir_build)/mset_4x_dg/%, \
					  $(objects))
objects_mset_6x := $(patsubst $(dir_build)/%, $(dir_build)/mset_6x/%, \
					  $(objects))
objects_spider_4x := $(patsubst $(dir_build)/%, $(dir_build)/spider_4x/%, \
					 $(objects))
objects_spider_5x := $(patsubst $(dir_build)/%, $(dir_build)/spider_5x/%, \
					 $(objects))
objects_spider_9x := $(patsubst $(dir_build)/%, $(dir_build)/spider_9x/%, \
					 $(objects))

objects_payload := $(call get_objects, \
				   $(call rwildcard, $(dir_source)/payload, *.s *.c))

versions := mset_4x mset_4x_dg mset_6x spider_4x spider_5x spider_9x

rops := $(foreach ver, $(versions), $(dir_build)/$(ver)/rop.dat)

.PHONY: all
all: launcher

.PHONY: launcher
launcher: $(dir_out)/$(name)

.PHONY: bigpayload
bigpayload: $(dir_build)/bigpayload.built

.PHONY: clean
clean:
	rm -rf $(dir_out)/$(name) $(dir_build)

# Big payload
$(dir_build)/bigpayload.built: $(dir_out)/$(name) $(dir_build)/payload/main.bin
	dd if=$(dir_build)/payload/main.bin of=$(dir_out)/$(name) bs=512 seek=256
	@touch $@

# Throw everything together
$(dir_out)/$(name): $(rops)
	touch $@
	dd if=$(dir_build)/mset_4x/rop.dat of=$@
	dd if=$(dir_build)/mset_4x_dg/rop.dat of=$@ bs=512 seek=80
	dd if=$(dir_build)/mset_6x/rop.dat of=$@ bs=512 seek=112
	dd if=$(dir_build)/spider_4x/rop.dat of=$@ bs=512 seek=144
	dd if=$(dir_build)/spider_5x/rop.dat of=$@ bs=512 seek=176
	dd if=$(dir_build)/spider_9x/rop.dat of=$@ bs=512 seek=208

$(dir_build)/spider_%/rop.dat: rop_param = SPIDER_$(shell echo $* | tr a-z A-Z)
$(dir_build)/spider_%/rop.dat: $(dir_build)/spider_%/rop.dat.dec
	$(PYTHON2) $(dir_tools)/spider-encrypt.py $< $@
$(dir_build)/spider_%/rop.dat.dec: $(dir_build)/spider_%/main.bin
	$(PYTHON2) $(dir_tools)/build-rop.py $(rop_param) $< $@

$(dir_build)/mset_%/rop.dat: rop_param = MSET_$(shell echo $* | tr a-z A-Z)
$(dir_build)/mset_%/rop.dat: $(dir_build)/mset_%/main.bin
	$(PYTHON2) $(dir_tools)/build-rop.py $(rop_param) $< $@

# Create bin from elf
$(dir_build)/%/main.bin: $(dir_build)/%/main.elf
	$(OC) -S -O binary $< $@

# Different flags for different things
$(dir_build)/payload/main.elf: ASFLAGS := $(ARM9FLAGS) $(ASFLAGS)
$(dir_build)/payload/main.elf: CFLAGS := $(ARM9FLAGS) $(CFLAGS)
$(dir_build)/payload/main.elf: $(objects_payload)
	# FatFs requires libgcc for __aeabi_uidiv
	$(CC) -nostartfiles $(LDFLAGS) -T linker_payload.ld $(OUTPUT_OPTION) $^

$(dir_build)/mset_4x/main.elf: ASFLAGS := $(ARM11FLAGS) $(ASFLAGS)
$(dir_build)/mset_4x/main.elf: CFLAGS := -DENTRY_MSET -DENTRY_MSET_4x \
										 $(ARM11FLAGS) $(CFLAGS)
$(dir_build)/mset_4x/main.elf: $(objects_mset_4x)
	$(LD) $(LDFLAGS) -T linker_mset.ld $(OUTPUT_OPTION) $^

$(dir_build)/mset_4x_dg/main.elf: ASFLAGS := $(ARM11FLAGS) $(ASFLAGS)
$(dir_build)/mset_4x_dg/main.elf: CFLAGS := -DENTRY_MSET -DENTRY_MSET_4x_DG \
								  $(ARM11FLAGS) $(CFLAGS)
$(dir_build)/mset_4x_dg/main.elf: $(objects_mset_4x_dg)
	$(LD) $(LDFLAGS) -T linker_mset.ld $(OUTPUT_OPTION) $^

$(dir_build)/mset_6x/main.elf: ASFLAGS := $(ARM11FLAGS) $(ASFLAGS)
$(dir_build)/mset_6x/main.elf: CFLAGS := -DENTRY_MSET -DENTRY_MSET_6x \
								  $(ARM11FLAGS) $(CFLAGS)
$(dir_build)/mset_6x/main.elf: $(objects_mset_6x)
	$(LD) $(LDFLAGS) -T linker_mset.ld $(OUTPUT_OPTION) $^

$(dir_build)/spider_4x/main.elf: ASFLAGS := $(ARM11FLAGS) $(ASFLAGS)
$(dir_build)/spider_4x/main.elf: CFLAGS := -DENTRY_SPIDER -DENTRY_SPIDER_4x \
								 $(ARM11FLAGS) $(CFLAGS)
$(dir_build)/spider_4x/main.elf: $(objects_spider_4x)
	$(LD) $(LDFLAGS) -T linker_spider.ld $(OUTPUT_OPTION) $^

$(dir_build)/spider_5x/main.elf: ASFLAGS := $(ARM11FLAGS) $(ASFLAGS)
$(dir_build)/spider_5x/main.elf: CFLAGS := -DENTRY_SPIDER -DENTRY_SPIDER_5x \
								 $(ARM11FLAGS) $(CFLAGS)
$(dir_build)/spider_5x/main.elf: $(objects_spider_5x)
	$(LD) $(LDFLAGS) -T linker_spider.ld $(OUTPUT_OPTION) $^

$(dir_build)/spider_9x/main.elf: ASFLAGS := $(ARM11FLAGS) $(ASFLAGS)
$(dir_build)/spider_9x/main.elf: CFLAGS := -DENTRY_SPIDER -DENTRY_SPIDER_9x \
								 $(ARM11FLAGS) $(CFLAGS)
$(dir_build)/spider_9x/main.elf: $(objects_spider_9x)
	$(LD) $(LDFLAGS) -T linker_spider.ld $(OUTPUT_OPTION) $^

# Fatfs requires to be built in thumb
$(dir_build)/payload/fatfs/%.o: $(dir_source)/payload/fatfs/%.c
	@mkdir -p "$(@D)"
	$(COMPILE.c) -mthumb -mthumb-interwork -Wno-unused-function $(OUTPUT_OPTION) $<

$(dir_build)/payload/fatfs/%.o: $(dir_source)/payload/fatfs/%.s
	@mkdir -p "$(@D)"
	$(COMPILE.s) -mthumb -mthumb-interwork $(OUTPUT_OPTION) $<
	
$(dir_build)/payload/%.o: $(dir_source)/payload/%.c
	@mkdir -p "$(@D)"
	$(COMPILE.c) $(OUTPUT_OPTION) $<

$(dir_build)/payload/%.o: $(dir_source)/payload/%.s
	@mkdir -p "$(@D)"
	$(COMPILE.s) $(OUTPUT_OPTION) $<

.SECONDEXPANSION:
$(dir_build)/%.o: $(dir_source)/$$(notdir $$*).c
	@mkdir -p "$(@D)"
	$(COMPILE.c) $(OUTPUT_OPTION) $<

.SECONDEXPANSION:
$(dir_build)/%.o: $(dir_source)/$$(notdir $$*).s
	@mkdir -p "$(@D)"
	$(COMPILE.s) $(OUTPUT_OPTION) $<

include $(call rwildcard, $(dir_build), *.d)
