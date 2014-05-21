
#ifndef __HARDWARE_PROFILE_H
#define __HARDWARE_PROFILE_H

#include "GenericTypeDefs.h"
#include "Compiler.h"

#define GetSystemClock()        80000000
#define GetPeripheralClock()    GetSystemClock()
#define GetInstructionClock()   (GetSystemClock() / 2)

// ENC28J60 I/O pins
#define ENC_CS_TRIS         (TRISAbits.TRISA9)
#define ENC_CS_IO           (PORTAbits.RA9)
#define ENC_RST_TRIS        (TRISAbits.TRISA4)
#define ENC_RST_IO          (PORTAbits.RA4)

#define ENC_SPI_IF          (IFS0bits.SPI1IF)
#define ENC_SSPBUF          (SPI1BUF)
#define ENC_SPISTAT         (SPI1STAT)
#define ENC_SPISTATbits     (SPI1STATbits)
#define ENC_SPICON1         (SPI1CON1)
#define ENC_SPICON1bits     (SPI1CON1bits)
#define ENC_SPICON2         (SPI1CON2)		 

#define UBRG			U2BRG
#define UMODE			U2MODE
#define USTA			U2STA
#define BusyUART()		BusyUART2()
#define CloseUART()		CloseUART2()
#define ConfigIntUART(a)        ConfigIntUART2(a)
#define DataRdyUART()           DataRdyUART2()
#define OpenUART(a,b,c)         OpenUART2(a,b,c)
#define ReadUART()		ReadUART2()
#define WriteUART(a)            WriteUART2(a)
#define getsUART(a,b,c)         getsUART2(a,b,c)
#define getcUART()		getcUART2()
#define putcUART(a)		do{while(BusyUART()); WriteUART(a); while(BusyUART()); }while(0)
#define putrsUART(a)            putrsUART2(a)

#endif
