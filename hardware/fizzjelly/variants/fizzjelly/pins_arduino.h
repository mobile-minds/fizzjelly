/*
  pins_arduino.h - Pin definition functions for MM FizzJelly
  Part of Arduino - http://www.arduino.cc/

  Copyright (c) 2007 David A. Mellis

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General
  Public License along with this library; if not, write to the
  Free Software Foundation, Inc., 59 Temple Place, Suite 330,
  Boston, MA  02111-1307  USA

  $Id: wiring.h 249 2007-02-03 16:52:51Z mellis $
*/
#ifndef Pins_Arduino_h
#define Pins_Arduino_h

#include <avr/pgmspace.h> 


// Workaround for wrong definitions in "iom32u4.h".
// This should be fixed in the AVR toolchain.
#undef UHCON
#undef UHINT
#undef UHIEN
#undef UHADDR
#undef UHFNUM
#undef UHFNUML
#undef UHFNUMH
#undef UHFLEN
#undef UPINRQX
#undef UPINTX
#undef UPNUM
#undef UPRST
#undef UPCONX
#undef UPCFG0X
#undef UPCFG1X
#undef UPSTAX
#undef UPCFG2X
#undef UPIENX
#undef UPDATX
#undef TCCR2A
#undef WGM20
#undef WGM21
#undef COM2B0
#undef COM2B1
#undef COM2A0
#undef COM2A1
#undef TCCR2B
#undef CS20
#undef CS21
#undef CS22
#undef WGM22
#undef FOC2B
#undef FOC2A
#undef TCNT2
#undef TCNT2_0
#undef TCNT2_1
#undef TCNT2_2
#undef TCNT2_3
#undef TCNT2_4
#undef TCNT2_5
#undef TCNT2_6
#undef TCNT2_7
#undef OCR2A
#undef OCR2_0
#undef OCR2_1
#undef OCR2_2
#undef OCR2_3
#undef OCR2_4
#undef OCR2_5
#undef OCR2_6
#undef OCR2_7
#undef OCR2B
#undef OCR2_0
#undef OCR2_1
#undef OCR2_2
#undef OCR2_3
#undef OCR2_4
#undef OCR2_5
#undef OCR2_6
#undef OCR2_7

#define IO1             1
#define IO2             2
#define IO3             3
#define IO4             4
#define IO5             5
#define IO6             6
#define IO7             7
#define IO8             8

#define STATUSLED       9
#define FUNCTIONKEY     0

#define INPUTVOLTAGE    A0

#define NUM_DIGITAL_PINS  22
#define NUM_ANALOG_INPUTS 8


/*
#define TX_RX_LED_INIT	DDRD |= (1<<5), DDRB |= (1<<0)
#define TXLED0			PORTD |= (1<<5)
#define TXLED1			PORTD &= ~(1<<5)
#define RXLED0			PORTB |= (1<<0)
#define RXLED1			PORTB &= ~(1<<0)
*/

#define TX_RX_LED_INIT	DDRD |= (1<<4), DDRD |= (1<<6)
#define TXLED0			PORTD &= ~(1<<4)
#define TXLED1			PORTD |= (1<<4)
#define RXLED0			PORTD &= ~(1<<6)
#define RXLED1			PORTD |= (1<<6)

//static const uint8_t SDA = 2;
//static const uint8_t SCL = 3;

// Map SPI port to 'new' pins D14..D17
//static const uint8_t SS   = 17;
static const uint8_t MOSI = 11;
static const uint8_t MISO = 10;
static const uint8_t SCK  = 12;

// Mapping of analog pins as digital I/O
static const uint8_t A0 = 0;    // IO1
static const uint8_t A1 = 1;    // IO1
static const uint8_t A2 = 2;    // IO2
static const uint8_t A3 = 3;    // IO3
static const uint8_t A4 = 4;    // IO4
static const uint8_t A6 = 6;    // IO5
static const uint8_t A7 = 7;    // IO6
static const uint8_t A8 = 8;    // IO7

#define digitalPinToPCICR(p)    ((((p) >= 8 && (p) <= 11) || ((p) >= 14 && (p) <= 17) || ((p) >= A8 && (p) <= A10)) ? (&PCICR) : ((uint8_t *)0))
#define digitalPinToPCICRbit(p) 0
#define digitalPinToPCMSK(p)    ((((p) >= 8 && (p) <= 11) || ((p) >= 14 && (p) <= 17) || ((p) >= A8 && (p) <= A10)) ? (&PCMSK0) : ((uint8_t *)0))
#define digitalPinToPCMSKbit(p) ( ((p) >= 8 && (p) <= 11) ? (p) - 4 : ((p) == 14 ? 3 : ((p) == 15 ? 1 : ((p) == 16 ? 2 : ((p) == 17 ? 0 : (p - A8 + 4))))))

//	__AVR_ATmega32U4__ has an unusual mapping of pins to channels
extern const uint8_t PROGMEM analog_pin_to_channel_PGM[];
#define analogPinToChannel(P)  ( pgm_read_byte( analog_pin_to_channel_PGM + (P) ) )

#ifdef ARDUINO_MAIN

// On the Arduino board, digital pins are also used
// for the analog output (software PWM).  Analog input
// pins are a separate set.

// ATMEL ATMEGA32U4 / Mobile Minds FizzJelly 
//
// D0				PE6       Function Key **
// 
// D1				PF7       IO1
// D2				PF6       IO2
// D3       PF5       IO3
// D4       PF4       IO4
// D5       PC6       IO5
// D6       PB6       IO6 *
// D7       PB5       IO7 *
// D8       PD7       IO8
//
// D9       PC7       STATUS LED 
//
// D10      PB1       EXPANSION 0 (ICSP-CLK)  *
// D11      PB3       EXPANSION 1 (ICSP-MISO) *
// D12      PB2       EXPANSION 2 (ICSP-MOSI) *
// D13      PD0       GPS-RX **
// D14      PD1       GPS-TX **
// D15      PD5       GSM-CTS 
// D16      PB7       GSM-RTS 
// D17      PD2       GSM-RX **
// D18      PD3       GSM-TX **
// D19      PB0       GSM-POWER
// D20      PB4       GSM-POWERKEY
// D21      PF0       INPUT VOLTAGE
//
// A0       ---       ADC0
// A1       D1        ADC7
// A2       D2        ADC6
// A3       D3        ADC5
// A4       D4        ADC4
// A6       D6        ADC13
// A7       D7        ADC12
// A8       D8        ADC10
//
//
//  * Pin change Interrupt capable
//  ** External Interrupt capable
//

//
// New pins D14..D17 to map SPI port to digital pins
//
// MISO     D11       PB3
// SCK      D10       PB1
// MOSI     D12       PB2
// 
// TXLED    PD4       
// RXLED    PD6       
// HWB      PE2       HWB

// these arrays map port names (e.g. port B) to the
// appropriate addresses for various functions (e.g. reading
// and writing)
const uint16_t PROGMEM port_to_mode_PGM[] = {
	NOT_A_PORT,
	NOT_A_PORT,
	(uint16_t) &DDRB,
	(uint16_t) &DDRC,
	(uint16_t) &DDRD,
	(uint16_t) &DDRE,
	(uint16_t) &DDRF,
};

const uint16_t PROGMEM port_to_output_PGM[] = {
	NOT_A_PORT,
	NOT_A_PORT,
	(uint16_t) &PORTB,
	(uint16_t) &PORTC,
	(uint16_t) &PORTD,
	(uint16_t) &PORTE,
	(uint16_t) &PORTF,
};

const uint16_t PROGMEM port_to_input_PGM[] = {
	NOT_A_PORT,
	NOT_A_PORT,
	(uint16_t) &PINB,
	(uint16_t) &PINC,
	(uint16_t) &PIND,
	(uint16_t) &PINE,
	(uint16_t) &PINF,
};

const uint8_t PROGMEM digital_pin_to_port_PGM[] = {
	PE, // D0 - PE6
  
	PF,	// D1 - PF7
	PF, // D2 - PF6
	PF,	// D3 - PF5
	PF,	// D4 - PF4
	PC, // D5 - PC6
	PB, // D6 - PB6
	PB, // D7 - PB5
	PD, // D8 - PD7
  
  PC, // D9 - PC7
  
	PB,	// D10 - PB1
	PB, // D11 - PB3
	PB,	// D12 - PB2
  
	PD, // D13 - PD0
	PD, // D14 - PD1
  
	PD,	// D15 - PD5
	PB,	// D16 - PB7
	PD,	// D17 - PD2
	PD,	// D18 - PD3
  PB,	// D19 - PB0
	PB, // D20 - PB4
  PF, // D21 / A0 - PF0
};

const uint8_t PROGMEM digital_pin_to_bit_mask_PGM[] = {
	_BV(6), // D0 - PE6
	_BV(7),	// D1 - PF7
	_BV(6), // D2 - PF6
	_BV(5),	// D3 - PF5
	_BV(4),	// D4 - PF4
	_BV(6), // D5 - PC6
	_BV(6), // D6 - PB6
	_BV(5), // D7 - PB5
	_BV(7), // D8 - PD7
	
	_BV(7), // D9 - PC7
  
  _BV(1),	// D10 - PB1
	_BV(3), // D11 - PB3
	_BV(2),	// D12 - PB2

	_BV(0), // D13 - PD0
	_BV(1), // D14 - PD1
	
	_BV(5),	// D15 - PD5
	_BV(7),	// D16 - PB7
	_BV(2),	// D17 - PD2
	_BV(3),	// D18 - PD3
	_BV(0),	// D19 - PB0
	_BV(4), // D20 - PB4
  _BV(0), // D21 / A0 - PF0
};

const uint8_t PROGMEM digital_pin_to_timer_PGM[] = {
	NOT_ON_TIMER,	
	
  NOT_ON_TIMER,
	NOT_ON_TIMER,
	NOT_ON_TIMER,
  NOT_ON_TIMER,
  TIMER3A,          // IO5 - D5
  TIMER1B,          // IO6 - D6
  TIMER1A,          // IO7 - D7
  TIMER4D,          // IO8 - D8
  
  TIMER4A,          // Status LED - D9
  NOT_ON_TIMER,
  NOT_ON_TIMER,
  NOT_ON_TIMER,
  NOT_ON_TIMER,
  NOT_ON_TIMER,
  NOT_ON_TIMER,
  NOT_ON_TIMER,
  NOT_ON_TIMER,
  NOT_ON_TIMER,
  NOT_ON_TIMER,
  NOT_ON_TIMER,
};

const uint8_t PROGMEM analog_pin_to_channel_PGM[] = {
  0,	// A0     ADC0      PF0       INPUT VOLTAGE
	7,	// A1     ADC7      PF7       IO1
	6,	// A2     ADC6      PF6       IO2
	5,	// A3     ADC5      PF5       IO3
	4,	// A4     ADC4      PF4       IO4
	13,	// A6     ADC13     PD7       IO6
	12,	// A7     ADC12     PB5       IO7
	10,	// A8     ADC10     PB6       IO8
};

#endif /* ARDUINO_MAIN */
#endif /* Pins_Arduino_h */
