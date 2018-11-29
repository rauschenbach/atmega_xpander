#ifndef _XCHG_H
#define _XCHG_H


#ifndef u8
#define u8 unsigned char
#endif

#ifndef s8
#define s8 char
#endif

#ifndef c8
#define c8 char
#endif

#ifndef u16
#define u16 unsigned short
#endif


#ifndef s16
#define s16 short
#endif


#ifndef u32
#define u32 unsigned long
#endif


#ifndef s32
#define s32 long
#endif

#ifndef u64
#define u64 unsigned long long
#endif


#ifndef bool
#define bool u8
#endif


#ifdef true
#define true 1
#endif

#ifdef false
#define false 0
#endif

/* на вход, slave включается входом SS */
#define EXP_SS								PB0
#define EXP_SS_PORT					PORTB
#define EXP_SS_DIR						PINB

/* направление для slave - вход "SCLK" на порте PB1 */
#define EXP_CLK							PB1
#define EXP_CLK_PORT				PORTB
#define EXP_CLK_DIR					PINB

/* направление для slave - вход данных MOSI на порте PB2 */
#define EXP_MOSI							PB2
#define EXP_MOSI_PORT				PORTB
#define EXP_MOSI_DIR					PINB

/* направление для slave - выход данных MISO на порте PB3 */
#define EXP_MISO				PB3
#define EXP_MISO_PORT				PORTB
#define EXP_MISO_DIR				DDRB

/*   Внешнее прерывание */
#define EXP_IRQ							PE7
#define EXP_IRQ_PORT					PORTE
#define EXP_IRQ_DIR						DDRE



#endif				/* xchg.h  */
