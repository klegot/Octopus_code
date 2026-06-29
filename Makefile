# --- Настройки проекта ---
TARGET     = main
MCU        = attiny414

# --- Настройки программатора ---
PORT       = /dev/ttyUSB0
PROGRAMMER = serialupdi

# --- Направления к папкам ---
INC_DIR    = Inc
SRC_DIR    = Src
BUILD_DIR  = build

.PHONY: all flash clean

all: clean
	mkdir -p $(BUILD_DIR)
	# 1. Компиляция драйвера в объектный файл (.o)
	/opt/avr-gcc/bin/avr-gcc -mmcu=$(MCU) -Os -Wall -I$(INC_DIR) -c $(SRC_DIR)/ws2812b_driver.c -o $(BUILD_DIR)/ws2812b_driver.o
	
	# 2. Компиляция главного файла main.c в объектный файл (.o)
	/opt/avr-gcc/bin/avr-gcc -mmcu=$(MCU) -Os -Wall -I$(INC_DIR) -c $(SRC_DIR)/main.c -o $(BUILD_DIR)/main.o
	
	# 3. Линовка объектных файлов и создание исполняемого .elf файла
	/opt/avr-gcc/bin/avr-gcc -mmcu=$(MCU) $(BUILD_DIR)/main.o $(BUILD_DIR)/ws2812b_driver.o -o $(BUILD_DIR)/$(TARGET).elf
	
	# 4. Конвертация готового .elf в .hex формат
	/opt/avr-gcc/bin/avr-objcopy -j .text -j .data -O ihex $(BUILD_DIR)/$(TARGET).elf $(BUILD_DIR)/$(TARGET).hex
	#
	# !!!!!!!!!!!!
	# Успех братан, ебашь это говно на камень
	# !!!!!!!!!!!!

flash:
	# 5. Прошивка платы
	avrdude -p $(MCU) -c $(PROGRAMMER) -P $(PORT) -U flash:w:$(BUILD_DIR)/$(TARGET).hex:i

clean:
	rm -rf $(BUILD_DIR)
