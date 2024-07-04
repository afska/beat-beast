#ifndef RUMBLE_H
#define RUMBLE_H

#define IO_RW ((volatile unsigned short*)0x80000C8)
#define IO_DIRECTION ((volatile unsigned short*)0x80000C6)
#define IO_DATA ((volatile unsigned short*)0x80000C4)

inline void RUMBLE_start() {
  *IO_DATA = (*IO_DATA) | 0b1000;  // I/O bit 3 => HIGH
}

inline void RUMBLE_stop() {
  *IO_DATA = (*IO_DATA) & 0b11110111;  // I/O bit 3 => LOW
}

inline void RUMBLE_init() {
  *IO_DIRECTION = (*IO_DIRECTION) | 0b1000;  // I/O bit 3 => OUTPUT
  *IO_RW = 1;                                // I/O => READ/WRITE
  RUMBLE_stop();
}

#endif  // RUMBLE_H
