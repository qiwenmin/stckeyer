/*
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

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
