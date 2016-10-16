# st-link-dumper
Dumper firmware for stm32f103x. It will read flash contents from 0x08000000 to 0x08004000 and dump it over UART2 (TX=PA2). Default baud is 9600.

Firmware start is marked with bytes `AB CD` - these two bytes need to be removed from the dump.

Precompiled binaries are located in `bin` directory. There are two binaries: `dump_0x4000.bin` will dump the first 16k of flash (bootloader section) and `dump_0x20000.bin` will dump 128k.
These binaries are _encrypted_ so that you can upload them directly to the programmer.
