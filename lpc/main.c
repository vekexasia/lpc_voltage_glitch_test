#include <stdio.h>

#include "LPC175x_6x_hal.h"

__attribute__((section(".crp"))) const uint32_t __CRP_WORD_END__ = 0x87654321;

#define LED_PIN (1 << 0) // GPIO0.0
#define LED2_PIN (1 << 1) // GPIO0.3
#define SIGNAL_PIN (1 << 26) // GPIO0.26


#define BAUD_RATE 115200

void setup_system_clock_internal(void) { // does not work
    LPC_SC->CLKSRCSEL = 0x00; // Select Internal RC oscillator as PLL0 clock source
    LPC_SC->PLL0CFG = (0 << 0) | (0 << 16); // Set MSEL (bits 0-4) to 0 and NSEL (bits 16-23) to 0
    LPC_SC->PLL0FEED = 0xAA;
    LPC_SC->PLL0FEED = 0x55;

    LPC_SC->PLL0CON = 0x01; // Enable PLL0
    LPC_SC->PLL0FEED = 0xAA;
    LPC_SC->PLL0FEED = 0x55;
    while (!(LPC_SC->PLL0STAT & (1 << 26))); // Wait for PLL0 lock

    LPC_SC->CCLKCFG = 0; // Set CCLKSEL (bits 0-7) to 0, making CCLK equal to the PLL0 frequency

    LPC_SC->PLL0CON = 0x03; // Enable and Connect PLL0
    LPC_SC->PLL0FEED = 0xAA;
    LPC_SC->PLL0FEED = 0x55;
    while (!(LPC_SC->PLL0STAT & ((1 << 25) | (1 << 24)))); // Wait for PLL0 enable and connect

    SystemCoreClock = 12000000; // Update SystemCoreClock variable to 12 MHz
}

void setup_system_clock(void) {
    // Power up the main external oscillator
    LPC_SC->PCONP |= (1 << 16);

    // Configure the pins for the main oscillator
    LPC_PINCON->PINSEL3 |= (1 << 8) | (1 << 10);

    // Select the main external oscillator as the PLL0 clock source
    LPC_SC->CLKSRCSEL = 0x01;

    // Configure PLL0 (MSEL = 5, NSEL = 0) to get a PLL0 frequency of 72 MHz
    // F_pll0 = (2 * M * F_osc) / N = (2 * 6 * 12) / 1 = 144 MHz
    LPC_SC->PLL0CFG = (5 << 0) | (0 << 16);
    LPC_SC->PLL0FEED = 0xAA;
    LPC_SC->PLL0FEED = 0x55;

    // Enable PLL0
    LPC_SC->PLL0CON = 0x01;
    LPC_SC->PLL0FEED = 0xAA;
    LPC_SC->PLL0FEED = 0x55;
    while (!(LPC_SC->PLL0STAT & (1 << 26))); // Wait for PLL0 lock

    // Set CCLKSEL (bits 0-7) to 3, making CCLK equal to the PLL0 frequency divided by 4
    // F_cclk = F_pll0 / (CCLKSEL + 1) = 144 MHz / 4 = 36 MHz
    LPC_SC->CCLKCFG = 3;

    // Enable and Connect PLL0
    LPC_SC->PLL0CON = 0x03;
    LPC_SC->PLL0FEED = 0xAA;
    LPC_SC->PLL0FEED = 0x55;
    while (!(LPC_SC->PLL0STAT & ((1 << 25) | (1 << 24)))); // Wait for PLL0 enable and connect

    SystemCoreClock = 36000000; // Update SystemCoreClock variable to 36 MHz
}

void setup_clkout(void) {
    // Configure P1.27 as CLKOUT
    LPC_PINCON->PINSEL3 &= ~(3 << 22); // Clear P1.27 pin configuration
    LPC_PINCON->PINSEL3 |= (1 << 22);  // Set P1.27 to CLKOUT function

    // Set the CLKOUT divider to 1 (CCLK / 1 = CCLK)
    LPC_SC->CLKOUTCFG &= ~(0xFF << 4); // Clear CLKOUT divider configuration
    LPC_SC->CLKOUTCFG |= (0 << 4);     // Set divider to 1

    // Select the CPU core clock (CCLK) as the CLKOUT source
    LPC_SC->CLKOUTCFG &= ~(0x0F); // Clear CLKOUT source selection
    LPC_SC->CLKOUTCFG |= (0);     // Set CCLK as the CLKOUT source

    // Enable CLKOUT
    LPC_SC->CLKOUTCFG |= (1 << 8);
}

void uart0_init(uint32_t baudrate) {
    LPC_SC->PCONP |= (1 << 3); // Power up UART0
    LPC_SC->PCLKSEL0 &= ~(3 << 6); // Clear PCLK_UART0

    // Set PCLK_UART0 = CCLK / 1 = 36 MHz / 1 = 36 MHz
    LPC_SC->PCLKSEL0 |= (1 << 6);

    uint32_t Fdiv = (SystemCoreClock / 16) / baudrate;
    LPC_UART0->LCR = 0x83; // 8 bits, no Parity, 1 Stop bit, DLAB=1
    LPC_UART0->DLM = Fdiv / 256;
    LPC_UART0->DLL = Fdiv % 256;
    LPC_UART0->LCR = 0x03; // Disable DLAB access

    LPC_PINCON->PINSEL0 |= (1 << 4) | (1 << 6); // Enable UART0 TXD and RXD pins

    LPC_UART0->FCR = 0x07; // Enable and reset TX and RX FIFO
}

void uart0_write_char(char c) {
    while (!(LPC_UART0->LSR & (1 << 5))); // Wait for THR empty
    LPC_UART0->THR = c;
}

void uart0_write_string(const char *str) {
    while (*str) {
        while (!(LPC_UART0->LSR & (1 << 5))); // Wait for THR empty
        LPC_UART0->THR = *str++;
    }
}

#define COUNT 1000000
#pragma GCC push_options
#pragma GCC optimize ("O0")

void delay(int count) {
    volatile int i;
    for (i = 0; i < count; i++);
}

int main(void) {
    LPC_GPIO0->SET = SIGNAL_PIN;
    LPC_GPIO0->DIR |= LED_PIN; // Set GPIO0.0 as output
    LPC_GPIO0->DIR |= LED2_PIN;

    LPC_GPIO0->DIR |= SIGNAL_PIN; // Set GPIO0.26 as output
    LPC_GPIO0->SET = LED_PIN;
    LPC_GPIO0->CLR = LED_PIN;
    LPC_GPIO0->SET = LED_PIN;
//
//    LPC_GPIO0->SET = SIGNAL_PIN;

//    LPC_GPIO0->SET = SIGNAL_PIN;

    char msg[64];

    if ((LPC_GPIO2->PIN & (1<<13)) == 0) {
        // Disable the BOD reset
        LPC_SC->PCON |= (1 << 4); // Set the BORD bit in the PCON register
        // Fully disable the BOD circuitry
        LPC_SC->PCON |= (1 << 3); // Set the BOGD bit in the PCON registerd
        LPC_GPIO0->CLR = LED2_PIN;
    } else {
        LPC_GPIO0->SET = LED2_PIN;
    }


//    SystemCoreCe_clock();
//    setup_clkout();

    uart0_init(BAUD_RATE);
    snprintf(msg, sizeof(msg), "X");
    uart0_write_string(msg);
    //uart0_write_char(0x42);

    volatile uint32_t *CPR_REG = (uint32_t *) 0x400FC184;
    volatile uint32_t *CPR_KEY = (uint32_t *) 0x2fc;

    // delay(2000000);


    while (1) {
        delay(10000);
        volatile int sum = 0;

        LPC_GPIO0->CLR = LED_PIN;
        LPC_GPIO0->CLR = SIGNAL_PIN;
        LPC_GPIO0->SET = SIGNAL_PIN;
        LPC_GPIO0->CLR = SIGNAL_PIN;
        // Turn on LED
        for (volatile int i = 0; i < 100; ++i) {
            // 1.52ms per loop cycle
            // or 4.27ms when cpu clock is being set above.

            for (volatile int j = 0; j < 100; ++j) {
                for (volatile int k = 0; k < 100; ++k) {
                    sum++;
                }
            }
            // LPC_GPIO0->SET = SIGNAL_PIN;
        }


        LPC_GPIO0->SET = LED_PIN;
        LPC_GPIO0->SET = SIGNAL_PIN;


        if (sum == COUNT) {
            snprintf(msg, sizeof(msg), ".");
            uart0_write_string(msg);
            //uart0_write_char(0x62);
        } else {
            snprintf(msg, sizeof(msg), "\r\n! 0x%08X - %d !\r\n", sum, sum);
            // *hex_value_addr
            uart0_write_string(msg);
        }

        delay(100000);

    }
}

#pragma GCC pop_options
