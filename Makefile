CC = sdcc
SRC = $(wildcard *.c) $(wildcard */*.c)

BOARD ?= stc15w104

TARGET ?= stckeyer

ifeq ($(BOARD),stc15w104)
	STCCODESIZE ?= 4089
	STCIRAMSIZE ?= 128
	CFLAGS ?= -mmcs51 --code-size $(STCCODESIZE) --iram-size $(STCIRAMSIZE) --xram-size 0 --std-sdcc99 --Werror -MD --disable-warning 190
	ifeq ($(TARGET),stckeyer-compact)
		CFLAGS += -DCOMPACT_CODE
	endif
	FLASHTOOL ?= stcgal
	FLASHFLAGS ?= -P stc15 -b 1200 -l 1200
endif

#ifeq ($(BOARD),stm8s103f3)
#	CFLAGS ?= -mstm8 --std-sdcc99 --Werror -MD
#	FLASHTOOL ?= stm8flash
#	FLASHFLAGS ?= -c stlinkv2 -p stm8s103f3 -w
#endif

.PHONY: all clean flash

all: build/$(TARGET)/tnyfsmos build/$(TARGET)/$(TARGET).hex

build/$(TARGET)/tnyfsmos:
	mkdir -p $@

build/$(TARGET)/$(TARGET).hex: build/$(TARGET)/$(TARGET).ihx
	packihx $^ > $@

build/$(TARGET)/$(TARGET).ihx: $(SRC:%.c=build/$(TARGET)/%.rel)
	$(CC) -o $@ $(CFLAGS) $^

build/$(TARGET)/%.rel: %.c
	$(CC) -c -o $@ $(CFLAGS) $<

-include $(SRC:%.c=build/$(TARGET)/%.d)

flash: build/$(TARGET)/$(TARGET).hex
	$(FLASHTOOL) $(FLASHFLAGS) $<

clean:
	rm -rf build/
