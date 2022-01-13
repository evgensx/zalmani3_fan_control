/* main.c
 * Designed for an attiny13, uses 300 bytes of program space, fuses set using -U lfuse:w:0x7a:m -U hfuse:w:0xff:m
 * powered from 5V, source it from your computer's USB headers or something
 * PB1 goes through a 1K resistor to the fan's PWM input
 * OCR0A = 47, 9600000 / (8*(47+1)) = 25000, если OCR0A = 47, то OCR0B = 0 - 46, -1 и 47 5 вольт. 
 * Распиновка
                   __ __
    (D5/A0) PB5  1|o    |8  VCC
  + (D3/A3) PB3  2|     |7  PB2 (D2/A1)  - //switch buttons
    (D4/A2) PB4  3|     |6  PB1 (D1/PWM) mosfet npn
            GND  4|_____|5  PB0 (D0/PWM)
*/
#ifndef __AVR_ATtiny13A__
    #define __AVR_ATtiny13A__
#endif
#include <avr/io.h>
#include <util/delay.h>
#define F_CPU 9600000UL
#define TRUE 1
#define FALSE 0

void analogWrite(uint8_t val) {
    if (val == 0 || val >= 47) {
        TCCR0A &= ~_BV(COM0B1);     // Turn off PWM PB1
        if (val >= 47) {
            PORTB |= _BV(PORTB1);    // Set pin PB1 high
        }
    }
    else {
        TCCR0A |= _BV(COM0B1);
        PORTB &= ~(_BV(PORTB1));
        OCR0B = val;
    }
}

uint8_t var = 2;    // ввод переменной для выбора скорости
void switchChoice(uint8_t var) {
    switch (var) {
        case 1:
            analogWrite(0); // 0V
            break;
        case 2:
            analogWrite(14); // 1.5V
            break;
        case 3:
            analogWrite(23); // 2.5V
            break;
        case 4:
            analogWrite(32); // 3.5V
            break;
        case 5:
            analogWrite(47); // 5V
            break;
    }
}

int main(void) {
    // PWM SETUP
    TCCR0B |= (1 << CS01) | (1 << WGM02);
    // set timer 0 prescaler (CS01) to div 8, set fast pwm mode 7 (WGM02)
    TCCR0A |= (1 << WGM01) | (1 << WGM00);  // set fast pwm mode 7
    TCCR0A |= (1 << COM0B1);                // Clear OC0B on Compare Match, set OC0B at TOP
    TCCR0A &= ~_BV(COM0A1); // disable port PB0
    DDRB |= _BV(DDB1);                       // enable pwm output pin
    OCR0A = 47;                             // TOP value used to set 25khz pwm freq
    //OCR0B = val;                          // pwm duty cycle, 0 to 48 due to reduced pwm resolution
    switchChoice(var);                        // Выбор переключателя по умолчанию
    // подтягиваем ножки кнопок, в регистре DDR0B должны быть 0
    DDRB &= ~(_BV(DDB2) | _BV(DDB3)); // Set pins as input PB2-3
    PORTB |= _BV(PORTB2) | _BV(PORTB3);   // Enable pullup resistors PB2-3 

    uint8_t flagBtnUp = FALSE;                  // флаг отпущенной кнопки +
    uint8_t flagBtnDown = FALSE;                // флаг отпущеной кнопки -
    while (1) { // Loop
        _delay_ms(50);
        uint8_t btnUpState = !(PINB & _BV(PB3));         // считываем состояние кнопки +
        uint8_t btnDownState = !(PINB & _BV(PB2));       // считываем состояние кнопки -
        if (btnUpState && !flagBtnUp && var < 5) { // var < 5
            flagBtnUp = TRUE;
            ++var;
            switchChoice(var);
        }
        if (!btnUpState && flagBtnUp) {
            flagBtnUp = FALSE; // обработчик отпускания кнопки +
        }
        if (btnDownState && !flagBtnDown && var > 1) { // var > 1
            flagBtnDown = TRUE;
            var--;
            switchChoice(var);
        }
        if (!btnDownState && flagBtnDown) {
            flagBtnDown = FALSE; // обработчик отпускания кнопки -
        }
    }   // Закрывание Loop
}
