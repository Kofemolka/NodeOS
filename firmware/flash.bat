d:\projects\esptool\esptool.py --port %1 erase_flash
d:\projects\esptool\esptool.py --port %1 write_flash --flash_mode dio --flash_size 4MB --flash_freq 40m --no-compress 0x00000 %2