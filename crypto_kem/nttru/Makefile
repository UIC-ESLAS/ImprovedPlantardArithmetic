OPENCM3DIR  = ./libopencm3
OPENCM3NAME = opencm3_stm32f4
OPENCM3FILE = $(OPENCM3DIR)/lib/lib$(OPENCM3NAME).a
LDSCRIPT    = ldscripts/stm32f405x6.ld

PREFIX     ?= arm-none-eabi
CC          = $(PREFIX)-gcc
LD          = $(PREFIX)-gcc
OBJCOPY     = $(PREFIX)-objcopy
SIZE        = $(PREFIX)-size
MKDIR       = mkdir -p

ARCH_FLAGS  = -mthumb -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=fpv4-sp-d16 -flto
DEFINES     = -DSTM32F4

FLAG1 ?=
FLAG2 ?=
FLAG3 ?=
FLAG4 ?=

CFLAGS     += -O3 \
              -Wall -Wextra -Wimplicit-function-declaration \
              -Wredundant-decls -Wmissing-prototypes -Wstrict-prototypes \
              -Wundef -Wshadow \
              -I$(OPENCM3DIR)/include \
              -fno-common $(ARCH_FLAGS) -MD $(DEFINES) $(FLAG1) $(FLAG2) $(FLAG3) $(FLAG4)

LDFLAGS    += --static -Wl,--start-group -lc -lgcc -lnosys -Wl,--end-group \
              -T$(LDSCRIPT) -nostartfiles -Wl,--gc-sections \
               $(ARCH_FLAGS) -L$(OPENCM3DIR)/lib -lm -l$(OPENCM3NAME)

COMMONPATH = common
TESTPATH   = test
BINPATH    = bin
ELFPATH    = elf
OBJPATH    = obj
IMPLEMENTATION_PATH=.

COMMONINCLUDES = -I$(COMMONPATH) -I$(IMPLEMENTATION_PATH)

COMMONSOURCES = $(COMMONPATH)/randombytes.c $(COMMONPATH)/aes.c $(COMMONPATH)/aes-publicinputs.c $(COMMONPATH)/aes-publicinputs.S $(COMMONPATH)/aes-encrypt.S $(COMMONPATH)/aes-keyschedule.S $(COMMONPATH)/sha2.c $(COMMONPATH)/crypto_hashblocks_sha512.c $(COMMONPATH)/crypto_hashblocks_sha512_inner32.s $(COMMONPATH)/hal-stm32f4.c
IMPLEMENTATION_SOURCES = $(wildcard $(IMPLEMENTATION_PATH)/*.c) $(wildcard $(IMPLEMENTATION_PATH)/*.S)
IMPLEMENTATION_HEADERS = $(IMPLEMENTATION_PATH)/*.h


.PHONY: clean all speed test stack #hashing
all: speed test stack # hashing

test:    $(BINPATH)/NTTRU_test.bin 

speed: $(BINPATH)/NTTRU_speed.bin

stack: $(BINPATH)/NTTRU_stack.bin

$(BINPATH)/%.bin: $(ELFPATH)/%.elf
	$(MKDIR) $(BINPATH)
	$(SIZE) $(ELFPATH)/$(*).elf
	$(OBJCOPY) -S -Obinary $(ELFPATH)/$(*).elf $(BINPATH)/$(*).bin

$(ELFPATH)/%.elf: $(TESTPATH)/speed.c $(TESTPATH)/test.c  $(TESTPATH)/stack.c $(OPENCM3FILE) \
	$(COMMONSOURCES) $(IMPLEMENTATION_SOURCES) $(IMPLEMENTATION_HEADERS) $(OPENCM3FILE)
	$(MKDIR) $(ELFPATH)
	$(CC) -o $@ $(CFLAGS) \
	$(TESTPATH)/$(shell echo $@ | grep -o -P '(?<=_).*(?=.elf)').c \
	$(COMMONSOURCES) $(IMPLEMENTATION_SOURCES) $(COMMONINCLUDES) $(LDFLAGS)

obj/%.o: $(IMPLEMENTATION_PATH)/%.c $(IMPLEMENTATION_HEADERS)
	mkdir -p obj
	$(CC) -o $@ -c $(CFLAGS) -I$(IMPLEMENTATION_PATH) $(COMMONINCLUDES) $<

obj/%.o: $(IMPLEMENTATION_PATH)/%.S $(IMPLEMENTATION_HEADERS)
	mkdir -p obj
	$(CC) -o $@ -c $(CFLAGS) -I$(IMPLEMENTATION_PATH) $(COMMONINCLUDES) $<

$(OPENCM3FILE):
	@if [ -z $(shell ls -A $(OPENCM3DIR)) ] ; then \
		printf "######## ERROR ########\n"; \
		printf "\tlibopencm3 is not initialized.\n"; \
		printf "\tPlease run (in the root directory):\n"; \
		printf "\t$$ git submodule init\n"; \
		printf "\t$$ git submodule update\n"; \
		printf "\tbefore running make.\n"; \
		printf "######## ERROR ########\n"; \
		exit 1; \
		fi
	make -C $(OPENCM3DIR)

runAll:	runSpeed runTest runStack  #runHashing runSize

runSpeed:
	python3 $(TESTPATH)/read_guest.py $(BINPATH)/NTTRU_speed.bin

runTest:
	python3 $(TESTPATH)/read_guest.py $(BINPATH)/NTTRU_test.bin

runStack:
	python3 $(TESTPATH)/read_guest.py $(BINPATH)/NTTRU_stack.bin

clean:
	-rm -rf $(BINPATH)
	-rm -rf $(ELFPATH)