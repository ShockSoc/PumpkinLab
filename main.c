/*
 * This is the code to drive the LED. It should run on any TI MSP430. 
 * For more information, have a look at the TI Launchpad series. 
 */

#include <msp430.h>
#include <stdint.h>
#include <stdlib.h>

#define FLICKER_MIN_BRIGHTNESS 10
#define FLICKER_FREQUENCY 1000
#define FLICKER_VIOLENCE 4
#define FLICKER_MAX_BRIGHTNESS 100

#define FADE_SPEED 1
#define FADE_MAX 100

#define MODE_FLICKER 0
#define MODE_FADE 1
#define MODE_STEADY 2
#define MODE_NUMBER_OF_MODES 3

#define PUSH_BUTTON BIT3
#define LED BIT0

signed int volatile brightness;
unsigned int direction = 0;
unsigned int mode = 2;

int main(void) {
    unsigned int i;
    WDTCTL = WDTPW + WDTHOLD;

    /* Timer config */
    CCTL0 = CCIE;
    TACTL = TASSEL_2 + MC_1 + ID_3;
    CCR0 = FLICKER_FREQUENCY;

    /* Pin Config */
    P1DIR |= 0x01 | (0x1 << 6);
    P1OUT = 0;
    P1OUT |= PUSH_BUTTON; /* Pullup on pin 3 */
    P1REN |= PUSH_BUTTON; /* Pullup/down enable for pin 3 */

    /* External Interrupt setup */
    //Set up interrupts?
    P1IES |= PUSH_BUTTON;
    P1IE |= PUSH_BUTTON;
    P1IFG &= ~PUSH_BUTTON;

    _BIS_SR(GIE);

    while (1) {
        for (i = 0; i < 200; i++) {
            if (i < brightness) {
                P1OUT |= LED;
            } else {
                P1OUT &= ~LED;
            }

        }
    }

    return 0;
}

/* Timer ISR */
#pragma vector=TIMER0_A0_VECTOR

__interrupt void timer_A
        (
                void
        ) {
    int r;
    direction &= 0x1;

    if (mode == MODE_FLICKER) {
        if (direction) {
            brightness += FLICKER_VIOLENCE;
        } else {
            brightness -= FLICKER_VIOLENCE;
        }

        if (brightness < FLICKER_MIN_BRIGHTNESS) {
            brightness = FLICKER_MIN_BRIGHTNESS;
        }

        if (brightness >= FLICKER_MAX_BRIGHTNESS) {
            brightness = FLICKER_MAX_BRIGHTNESS;
        }

        r = rand();
        if ((r % 10) == 0) {
            direction = (~direction) & 0x1;
        }
    } else if (mode == MODE_FADE) {
        if (brightness <= 0 || brightness > FADE_MAX) {
            direction = (~direction) & 0x1;
        }

        if (direction) {
            brightness += FADE_SPEED;
        } else {
            brightness -= FADE_SPEED;
        }

    } else {
        brightness = 200;
    }

}

/* Pushbutton ISR */
#pragma vector=PORT1_VECTOR

__interrupt void Port_1
        (
                void
        ) {
    int i;
    brightness = 0;
    direction = 0;
    mode = (mode + 1) % MODE_NUMBER_OF_MODES;
    while (P1IN & PUSH_BUTTON);
    for (i = 0; i < 20000; i++);
    P1IFG &= ~PUSH_BUTTON;
}


