#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <util/delay.h>
#include "xchg.h"

const char fusedata[] __attribute__ ((section (".fuse"))) = {0xE2, 0xB9, 0xFF};


/* Прерываниe SPI для Slave Mode. Команда за 3 цикла SPI! */
ISR(SPI__STC_vect)
{
	static volatile u8 num = 0;
	static volatile u8 rx_cmd;
	static volatile u8 addr;
	static volatile u8 data;
	register volatile u8 rx_byte;
	register volatile u8 tx_byte;

 
	rx_byte = SPDR;		/* команда */

	if (num == 0) {
		rx_cmd = rx_byte;	/* чтение или запись */
		data = 0;
		num = 1;
		tx_byte = 0;
	} else if (num == 1 && rx_cmd == 0x00) {	/* чтение */
		// читаем
		addr = rx_byte;
		data = *(u8 *) addr;
		tx_byte = data;	/* для передачи, когда сервер протактирует */
		num = 3;
	} else if ((num == 1) && ((rx_cmd == 0x80) || (rx_cmd == 0x40) || (rx_cmd == 0x20) || (rx_cmd == 0x10))) {
		/* запись полная, установка, сброс или переключение */
		addr = rx_byte;
		num = 2;
		tx_byte = 0;
	} else if (num == 2) {
		num = 0;
		data = rx_byte;

		/* Пишем по адресу данные */
		if (rx_cmd == 0x80) {	/* Запись  */
			*(u8 *) addr = data;
		} else if (rx_cmd == 0x40) {	/* Переключение  */
			*(u8 *) addr ^= data;
		} else if (rx_cmd == 0x20) {	/* Установка  */
			*(u8 *) addr |= data;
		} else if (rx_cmd == 0x10) {	/* Сброс  */
			*(u8 *) addr &= ~data;
		}
		tx_byte = 0;	
	
		
	} else {
		num = 0;
	}

	SPDR = tx_byte;

	// Отладить-пока не знаем как работает
	//	sleep_mode();
}

#if 0
/**
 * Пробуждающий вектор
 */
ISR(PCINT1_vect)
{
    asm("nop;");
}
#endif

/* Инициализация SPI в режиме Slave, по какой фазе будет работать? 
 * прерывания возникают ПОСЛЕ передачи мастером байта */
static void SPI_slaveInit(void)
{
	volatile char temp;

	/* Установить MISO на выход, все другие на вход (уже по дефолту) */
	EXP_MISO_DIR |= (1 << EXP_MISO);

	/*  Выход внешнего прерывания то же сделаем здесь */
	EXP_IRQ_DIR |= (1 << EXP_IRQ);

	/* Настройка SPI:
	 * SPIE: SPI Interrupt Enable
	 * SPE: SPI Enable
	 * DORD: Data Order
	 * MSTR: Master (1)/Slave Select (0)
	 * CPOL: Clock Polarity = 0
	 * CPHA: Clock Phase = 0
	 * SPR1, SPR0: SPI Clock Rate Select
	 */
	SPCR = (1 << SPIE) | (1 << SPE) | (0 << CPOL) | (0 << CPHA);	/* 0 режим */
	/*     SPCR = (1 << SPIE) | (1 << SPE) | (0 << CPOL) | (1 << CPHA); *//* 1 режим  */

	temp = SPSR;		/* Почистим статус */
	SPDR = 0;		/* В регистре данных будет лежать "0" - мастер его получит когда протактирует */
}



/* Установить порты на вход и выход. по крайней мере pc pd pe и pg */
static void PORTS_init(void)
{
	PRR |= 0x0F; /*  Все выключим сначала. Обязательно! запишем значение 0 - включим нулем */
	PRR &= ~((1 << PRSPI) |(1 << PRADC));	/*  Включим SPI */
#if 0
	/* Добавлено для засыпания  */
	EIMSK |= PCIE1;
	PCMSK1 |= PCINT8;
	set_sleep_mode(SLEEP_MODE_PWR_DOWN);
#endif         
	/* Включим генератор единицей */
	PORTC |= 1 << PORTC1;
	DDRC |= 1 << PORTC1;
}


int main(void)
{
  	
	/* инициализацыя портов */
	PORTS_init();

/* Запретим прерывания */
	cli();
	
	/* Настроим SPI на режим Slave */
	SPI_slaveInit();

	/* разрешим прерывания */
     sei();
	 
	 while (1);
}
