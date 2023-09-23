del /Q C:\GENDK\BLAZE_ENGINE\out\res\*.*
del /Q C:\GENDK\BLAZE_ENGINE\out\src\*.*
del /Q C:\GENDK\BLAZE_ENGINE\out\watchers\*.*
del /Q C:\GENDK\BLAZE_ENGINE\out\rom.bin
del /Q C:\GENDK\BLAZE_ENGINE\out\rom.out
del /Q C:\GENDK\BLAZE_ENGINE\out\rom_head.bin
del /Q C:\GENDK\BLAZE_ENGINE\out\rom_head.o
del /Q C:\GENDK\BLAZE_ENGINE\out\sega.o
del /Q C:\GENDK\BLAZE_ENGINE\out\sysbol.txt
C:\sgdk\bin\make -f C:\sgdk\makefile.gen
C:\GAMES\Bizhawk\EmuHawk C:\GENDK\BLAZE_ENGINE\out\rom.bin