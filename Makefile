rwildcard = $(foreach d, $(wildcard $1*), $(filter $(subst *, %, $2), $d) $(call rwildcard, $d/, $2))

# This should be set externally
name ?= Cakes.dat
path ?=
dir_out ?= .

CC := arm-none-eabi-gcc
AS := arm-none-eabi-as
LD := arm-none-eabi-ld
OC := arm-none-eabi-objcopy

dir_source := source
dir_build := build

ARM9FLAGS := -mcpu=arm946e-s -march=armv5te
ARM11FLAGS := -mcpu=mpcore
ASFLAGS := -mlittle-endian
CFLAGS := -marm $(ASFLAGS) -O2 -std=c11 -MMD -MP -fno-builtin -fshort-wchar -Wall -Wextra -Wno-main -DLAUNCHER_PATH='"$(path)$(name)"'

get_objects = $(patsubst $(dir_source)/%.s, $(dir_build)/%.o, \
			  $(patsubst $(dir_source)/%.c, $(dir_build)/%.o, $1))

objects := $(call get_objects, $(wildcard $(dir_source)/*.s $(dir_source)/*.c))

objects_payload := $(call get_objects, \
				   $(call rwildcard, $(dir_source)/payload, *.s *.c))

versions_eju := mset_4x mset_4x_dg mset_6x spider_4x spider_5x spider_9x
versions_cn :=  spider_42_cn spider_45_cn spider_5x_cn spider_9x_cn
versions_kr :=  spider_4x_kr spider_5x_kr spider_9x_kr
versions_tw :=  spider_4x_tw spider_5x_tw spider_9x_tw
versions := $(versions_eju) $(versions_cn) $(versions_kr) $(versions_tw)

rops := $(foreach ver, $(versions), $(dir_build)/$(ver)/rop.dat)

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

# Big payload
$(dir_build)/bigpayload.built: $(dir_out)/$(name) $(dir_build)/payload/main.bin
	dd if=$(dir_build)/payload/main.bin of=$(dir_out)/$(name) bs=512 seek=640
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
	dd if=$(dir_build)/spider_42_cn/rop.dat of=$@ bs=512 seek=240
	dd if=$(dir_build)/spider_45_cn/rop.dat of=$@ bs=512 seek=272
	dd if=$(dir_build)/spider_5x_cn/rop.dat of=$@ bs=512 seek=304
	dd if=$(dir_build)/spider_9x_cn/rop.dat of=$@ bs=512 seek=336
	dd if=$(dir_build)/spider_4x_kr/rop.dat of=$@ bs=512 seek=368
	dd if=$(dir_build)/spider_5x_kr/rop.dat of=$@ bs=512 seek=400
	dd if=$(dir_build)/spider_9x_kr/rop.dat of=$@ bs=512 seek=432
	dd if=$(dir_build)/spider_4x_tw/rop.dat of=$@ bs=512 seek=464
	dd if=$(dir_build)/spider_5x_tw/rop.dat of=$@ bs=512 seek=496
	dd if=$(dir_build)/spider_9x_tw/rop.dat of=$@ bs=512 seek=528

$(dir_build)/spider_%/rop.dat: rop_param = SPIDER_$(shell echo $* | tr a-z A-Z)
$(dir_build)/spider_%/rop.dat: $(dir_build)/spider_%/main.bin
	@make -C rop3ds rop.dat ASFLAGS="-D$(rop_param) -DARM_CODE=../$<"
	@mv rop3ds/rop.dat $@

$(dir_build)/mset_%/rop.dat: rop_param = MSET_$(shell echo $* | tr a-z A-Z)
$(dir_build)/mset_%/rop.dat: $(dir_build)/mset_%/main.bin
	@make -C rop3ds rop.dat ASFLAGS="-D$(rop_param) -DARM_CODE=../$<"
	@mv rop3ds/rop.dat $@

# Create bin from elf
$(dir_build)/%/main.bin: $(dir_build)/%/main.elf
	$(OC) -S -O binary $< $@

# Different flags for different things
$(dir_build)/payload/main.elf: ASFLAGS := $(ARM9FLAGS) $(ASFLAGS)
$(dir_build)/payload/main.elf: CFLAGS := $(ARM9FLAGS) $(CFLAGS)
$(dir_build)/payload/main.elf: $(objects_payload)
	# FatFs requires libgcc for __aeabi_uidiv
	$(CC) -nostartfiles $(LDFLAGS) -T linker_payload.ld $(OUTPUT_OPTION) $^

# The arm11 payload
PERCENT = %

$(dir_build)/%/main.elf: version_param = $(shell echo $(*D) | tr a-z A-Z)
$(dir_build)/%/main.elf: ASFLAGS := $(ARM11FLAGS) $(ASFLAGS)
$(dir_build)/%/main.elf: TCFLAGS := $(CFLAGS)

$(dir_build)/mset_%/main.elf: CFLAGS = -DENTRY_MSET -DENTRY_$(version_param) \
										 $(ARM11FLAGS) $(TCFLAGS)
.SECONDEXPANSION:
$(dir_build)/mset_%/main.elf: $$(patsubst $(dir_build)/$$(PERCENT), $(dir_build)/mset_$$*/$$(PERCENT), $(objects))
	$(LD) $(LDFLAGS) -T linker_mset.ld $(OUTPUT_OPTION) $^

$(dir_build)/spider_%/main.elf: CFLAGS = -DENTRY_SPIDER -DENTRY_$(version_param) \
								 $(ARM11FLAGS) $(TCFLAGS)
.SECONDEXPANSION:
$(dir_build)/spider_%/main.elf: $$(patsubst $(dir_build)/$$(PERCENT), $(dir_build)/spider_$$*/$$(PERCENT), $(objects))
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
