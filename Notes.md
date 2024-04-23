## Breadboard Pinout

FPGA   | FMC     | ESDI Pin  | ESDI Name
-------|---------|-----------|-------------------------
 L20   | LA02_P  |  J1,22    | READY
 L17   | LA04_P  |  J1,20    | INDEX
 J16   | LA07_P  |  J1,16    | SECTOR
 A13   | LA11_P  |  J1,12    | ATTENTION
 D16   | LA15_P  |  J1,10    | TRANSFER ACK
 D12   | LA19_P  |  J1,8     | CONFIG/STATUS DATA
 B10   | LA21_P  |  J2,17/18 | READ DATA
 B6    | LA24_P  |  J2,10/11 | READ REFERENCE CLOCK
 F16   | LA00_N  |  J1,18    | HEAD SELECT 2(1)
 K18   | LA03_N  |  J1,14    | HEAD SELECT 2(0)
 E17   | LA08_N  |  J1,34    | COMMAND DATA
 F18   | LA12_N  |  J1,32    | READ GATE
 C17   | LA16_N  |  J1,28    | DRIVE SELECT 2(1)
 E12   | LA20_N  |  J1,24    | TRANSFER REQ
 H12   | LA22_N  |  J1,4     | HEAD SELECT 2(2)
 C6    | LA25_N  |  J1,2     | HEAD SELECT 2(3)


# IBM PS/2 Controller

The last head of each cylinder is missing the last 4 sectors

byte 0    Sync = a1
byte 1    LSB of sector count
byte 2    MSB of sector count ...

flags (byte 5):
0x40 First Sector of Track
0x80 Last Sector of Track
0x20 Last head of cylinder
