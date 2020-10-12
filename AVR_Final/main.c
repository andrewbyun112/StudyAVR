/*
 * 0617_UART_Piezo.c
 *
 * Created: 2020-06-17 오후 4:06:01
 * Author : PKNU
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdio.h>

volatile unsigned char TX_flag = 0;
volatile char TX_data = 0;
volatile unsigned char Time_STOP = 0;
volatile unsigned char cnt = 0;

unsigned int DoReMi[8] = {523, 587, 659, 698, 783, 880, 987, 1046}; //음계

void putch(unsigned char data)
{
	while((UCSR0A & 0x20) == 0);
	UDR0 = data;
	UCSR0A |= 0x20;
}

unsigned char getch()
{
	unsigned char data;
	while((UCSR0A & 0x80) == 0);
	data = UDR0;
	UCSR0A |= 0x80;
	return data;
}

void putch_Str(char *str)
{
	unsigned char i = 0;
	while(str[i]!='\0')
	putch(str[i++]);
}

//CDS 동작 함수
int CDS()
{
	unsigned int AdData = 0;
	unsigned char ADC_Str[10];
	while(1)
	{
		if(Time_STOP == 0)
		{
			ADCSRA |= 0x40;
			while((ADCSRA & 0x10) == 0x00);
			AdData = ADC;
			putch_Str("\n\r CDS ADC_data : ");
			sprintf(ADC_Str,"%d",ADC);
			putch_Str(ADC_Str);
			_delay_ms(500);
		}
		
	}
}

// 메인 함수
int main(void)
{
	unsigned char LED_Data = 0x01, i = 0;
	DDRC = 0xFF;
	unsigned char RX_data = 0;
    DDRE = 0x08; //포트E PE3을 출력 나머지는 입력포트로 설정
	DDRB = 0x20; //Motor1_EN을 출력포트로 설정
	DDRD = 0xF0; //Step 0~3을 출력포트로 설정
	PORTB &= ~0x20; //M1 Disable 
	UCSR0A = 0x00;
	UCSR0B = 0x18; //RX, TX enable
	UCSR0C = 0x06; //비동기 방식
	UBRR0H = 0x00;
	UBRR0L = 0x03; //115200bps
	
	TCCR3A = 0x00; //WGM3(1:0) = "00"
	TCCR3B = 0x19; //WGM3(3:2) = "11", CS3(2:0) = "001", 1분주 사용
	TCCR3C = 0x00; 
	TCNT3 = 0x0000; //타이머3 카운터 초기화
	
	EICRB = (1<<ISC41)|(1<<ISC40);
	EIMSK = (1<<INT4);
	EIFR = (1<<INTF4);
	
	sei();
	
	unsigned char text[] = "\r\n++++Menu++++\r\n L : LED\r\n C : CDS\r\n 1 ~ 8 : PIANO\r\n S : Step Motor(F : Forward Direction, R : Reverse Direction)\r\n push button : Stop\r\n";
	//unsigned char i = 0;
	
	
	ADMUX = 0x00;
	ADCSRA = 0x87;
	DDRF = 0x02;
	PORTF = 0x02;
	
	while(text[i] != '\0')
	{
		putch(text[i++]);
	}
	
    while (1) 
    {
		RX_data = getch();
		putch(getch());
		
//////////////////// LED ////////////////////
		while(RX_data == 'l') // 'L' 눌렀을 때
		{
			LED_Data = 0x01;
			for(i=0; i<8; i++)
			{
				if(Time_STOP == 0)
				{
					PORTC = LED_Data;
					LED_Data <<= 1;
					_delay_ms(500);
				}
				
			}
			LED_Data = 0x40;
			for(i=0; i<8; i++)
			{
				if(Time_STOP == 0)
				{
					PORTC = LED_Data;
					LED_Data >>= 1;
					_delay_ms(500);
				}
				
			}
		}
		
//////////////////// PIANO ////////////////////
		if(RX_data == '1')
		{
			ICR3 = 7372800/DoReMi[0]/15;
			TCCR3A = 0x40;
			_delay_ms(300);
			TCCR3A = 0x00;
		}
		if(RX_data == '2')
		{
			ICR3 = 7372800/DoReMi[1]/15;
			TCCR3A = 0x40;
			_delay_ms(300);
			TCCR3A = 0x00;
		}
		if(RX_data == '3')
		{
			ICR3 = 7372800/DoReMi[2]/15;
			TCCR3A = 0x40;
			_delay_ms(300);
			TCCR3A = 0x00;
		}
		if(RX_data == '4')
		{
			ICR3 = 7372800/DoReMi[3]/15;
			TCCR3A = 0x40;
			_delay_ms(300);
			TCCR3A = 0x00;
		}
		if(RX_data == '5')
		{
			ICR3 = 7372800/DoReMi[4]/15;
			TCCR3A = 0x40;
			_delay_ms(300);
			TCCR3A = 0x00;
		}
		if(RX_data == '6')
		{
			ICR3 = 7372800/DoReMi[5]/15;
			TCCR3A = 0x40;
			_delay_ms(300);
			TCCR3A = 0x00;
		}
		if(RX_data == '7')
		{
			ICR3 = 7372800/DoReMi[6]/15;
			TCCR3A = 0x40;
			_delay_ms(300);
			TCCR3A = 0x00;
		}
		if(RX_data == '8')
		{
			ICR3 = 7372800/DoReMi[7]/15;
			TCCR3A = 0x40;
			_delay_ms(300);
			TCCR3A = 0x00;
		}
		
//////////////////// 스텝모터 ////////////////////
		if(RX_data == 's') //'S' 눌렀을 때
		{
			
				unsigned char RX_data1 = 0;
				RX_data1 = getch();
				putch(getch());
				if(RX_data1 == 'f')
				{
					while(1)
					{
						if(Time_STOP == 0)
						{
						PORTD = 0x80;
						_delay_ms(10);
						PORTD = 0x40;
						_delay_ms(10);
						PORTD = 0x20;
						_delay_ms(10);
						PORTD = 0x10;
						_delay_ms(10);
						}
					}
				}
				if(RX_data1 == 'r')
				{
					while(1)
					{
						if(Time_STOP == 0)
						{
						PORTD = 0x10;
						_delay_ms(10);
						PORTD = 0x20;
						_delay_ms(10);
						PORTD = 0x40;
						_delay_ms(10);
						PORTD = 0x80;
						_delay_ms(10);
						}
					}
				}
			
		}
		
//////////////////// CDS ////////////////////
		if(RX_data == 'c')
		{
			CDS();
		}
    }
}

SIGNAL(INT4_vect)
{
	cli();
	if(Time_STOP == 0)
	{
		Time_STOP = 1;
	}
	else
	{
		Time_STOP = 0;
	}
	sei();	
}

