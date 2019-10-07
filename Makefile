CC = sdcc
SRC = $(wildcard *.c) $(wildcard */*.c)

BOARD ?= stc15w104

TARGET = stckeyer

ifeq ($(BOARD),stc15w104)
	STCCODESIZE ?= 4089
	STCIRAMSIZE ?= 128
	CFLAGS ?= -mmcs51 --code-size $(STCCODESIZE) --iram-size $(STCIRAMSIZE) --xram-size 0 --std-sdcc99 --Werror -MD
	FLASHTOOL ?= stcgal
	FLASHFLAGS ?= -P stc15 -b 1200 -l 1200
endif

ifeq ($(BOARD),stm8s103f3)
	CFLAGS ?= -mstm8 --std-sdcc99 --Werror -MD
	FLASHTOOL ?= stm8flash
	FLASHFLAGS ?= -c stlinkv2 -p stm8s103f3 -w
endif

.PHONY: all clean flash

all: build/tnyfsmos build/$(TARGET).hex

build/tnyfsmos:
	mkdir -p $@

build/$(TARGET).hex: build/$(TARGET).ihx
	packihx $^ > $@

build/$(TARGET).ihx: $(SRC:%.c=build/%.rel)
	$(CC) -o $@ $(CFLAGS) $^

build/%.rel: %.c
	$(CC) -c -o $@ $(CFLAGS) $<

-include $(SRC:%.c=build/%.d)

flash: build/$(TARGET).hex
	$(FLASHTOOL) $(FLASHFLAGS) $<

clean:
	rm -rf build/
