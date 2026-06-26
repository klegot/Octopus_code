TARGET = octopus_test
BUILD_DIR = build

MCU = attiny44
F_CPU = 3333333UL

PORT = /dev/ttyUSB0
PROGRAMMER = serialupdi

CC = avr-gcc
OBJCOPY = avr-objcopy
SIZE = avr-size
AVRDUDE = avrdude

CFLAGS = -mmcu=$(MCU) -DF_CPU=$(F_CPU) -O2 -Wall -std=gnu99

.PHONY: all clean flash

all: $(BUILD_DIR)/$(TARGET).hex

$(BUILD_DIR)/$(TARGET).elf: octopus_test.c
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) $< -o $@
	@echo ""
	$(SIZE) --format=avr --mcu=$(MCU) $@
	@echo ""

$(BUILD_DIR)/$(TARGET).hex: $(BUILD_DIR)/$(TARGET).elf
	$(OBJCOPY) -O ihex -R .eeprom $< $@

flash: $(BUILD_DIR)/$(TARGET).hex
	$(AVRDUDE) -p attiny414 -c $(PROGRAMMER) -P $(PORT) -U flash:w:$<:i

clean:
	rm -rf $(BUILD_DIR)
