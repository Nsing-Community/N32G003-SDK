.\SREC\srec_cat .\Objects\N32G003_SelfTest.hex -intel -crop 0x08000000 0x08007000 -fill 0x00 0x08000000 0x08007000 ^
-crc32-l-e 0x08007000 -o .\Objects\N32G003_SelfTest_CRC.hex -intel
