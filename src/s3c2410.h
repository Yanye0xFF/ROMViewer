#ifndef _S3C2410_H
#define _S3C2410_H

// delay depends on cpu clock
// this project cpu freq is 75Mhz
// 625us/tick
#define delay(t) { volatile unsigned int i = (t); while (i--); }

// remapped addresses of registers
#define GPFDAT ((unsigned *)0x07A00054)
#define GPGCON ((unsigned *)0x07A00060)
#define GPGDAT ((unsigned *)0x07A00064)
#define MAGIC  ((unsigned *)0x0003FFF0)

#endif
