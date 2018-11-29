#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <util/delay.h>
#include "xchg.h"

const char fusedata[] __attribute__ ((section (".fuse"))) = {0xE2, 0xB9, 0xFF};


/* ���������e SPI ��� Slave Mode. ������� �� 3 ����� SPI! */
ISR(SPI__STC_vect)
{
	static volatile u8 num = 0;
	static volatile u8 rx_cmd;
	static volatile u8 addr;
	static volatile u8 data;
	register volatile u8 rx_byte;
	register volatile u8 tx_byte;

 
	rx_byte = SPDR;		/* ������� */

	if (num == 0) {
		rx_cmd = rx_byte;	/* ������ ��� ������ */
		data = 0;
		num = 1;
		tx_byte = 0;
	} else if (num == 1 && rx_cmd == 0x00) {	/* ������ */
		// ������
		addr = rx_byte;
		data = *(u8 *) addr;
		tx_byte = data;	/* ��� ��������, ����� ������ ������������ */
		num = 3;
	} else if ((num == 1) && ((rx_cmd == 0x80) || (rx_cmd == 0x40) || (rx_cmd == 0x20) || (rx_cmd == 0x10))) {
		/* ������ ������, ���������, ����� ��� ������������ */
		addr = rx_byte;
		num = 2;
		tx_byte = 0;
	} else if (num == 2) {
		num = 0;
		data = rx_byte;

		/* ����� �� ������ ������ */
		if (rx_cmd == 0x80) {	/* ������  */
			*(u8 *) addr = data;
		} else if (rx_cmd == 0x40) {	/* ������������  */
			*(u8 *) addr ^= data;
		} else if (rx_cmd == 0x20) {	/* ���������  */
			*(u8 *) addr |= data;
		} else if (rx_cmd == 0x10) {	/* �����  */
			*(u8 *) addr &= ~data;
		}
		tx_byte = 0;	
	
		
	} else {
		num = 0;
	}

	SPDR = tx_byte;

	// ��������-���� �� ����� ��� ��������
	//	sleep_mode();
}

#if 0
/**
 * ������������ ������
 */
ISR(PCINT1_vect)
{
    asm("nop;");
}
#endif

/* ������������� SPI � ������ Slave, �� ����� ���� ����� ��������? 
 * ���������� ��������� ����� �������� �������� ����� */
static void SPI_slaveInit(void)
{
	volatile char temp;

	/* ���������� MISO �� �����, ��� ������ �� ���� (��� �� �������) */
	EXP_MISO_DIR |= (1 << EXP_MISO);

	/*  ����� �������� ���������� �� �� ������� ����� */
	EXP_IRQ_DIR |= (1 << EXP_IRQ);

	/* ��������� SPI:
	 * SPIE: SPI Interrupt Enable
	 * SPE: SPI Enable
	 * DORD: Data Order
	 * MSTR: Master (1)/Slave Select (0)
	 * CPOL: Clock Polarity = 0
	 * CPHA: Clock Phase = 0
	 * SPR1, SPR0: SPI Clock Rate Select
	 */
	SPCR = (1 << SPIE) | (1 << SPE) | (0 << CPOL) | (0 << CPHA);	/* 0 ����� */
	/*     SPCR = (1 << SPIE) | (1 << SPE) | (0 << CPOL) | (1 << CPHA); *//* 1 �����  */

	temp = SPSR;		/* �������� ������ */
	SPDR = 0;		/* � �������� ������ ����� ������ "0" - ������ ��� ������� ����� ������������ */
}



/* ���������� ����� �� ���� � �����. �� ������� ���� pc pd pe � pg */
static void PORTS_init(void)
{
	PRR |= 0x0F; /*  ��� �������� �������. �����������! ������� �������� 0 - ������� ����� */
	PRR &= ~((1 << PRSPI) |(1 << PRADC));	/*  ������� SPI */
#if 0
	/* ��������� ��� ���������  */
	EIMSK |= PCIE1;
	PCMSK1 |= PCINT8;
	set_sleep_mode(SLEEP_MODE_PWR_DOWN);
#endif         
	/* ������� ��������� �������� */
	PORTC |= 1 << PORTC1;
	DDRC |= 1 << PORTC1;
}


int main(void)
{
  	
	/* ������������� ������ */
	PORTS_init();

/* �������� ���������� */
	cli();
	
	/* �������� SPI �� ����� Slave */
	SPI_slaveInit();

	/* �������� ���������� */
     sei();
	 
	 while (1);
}
