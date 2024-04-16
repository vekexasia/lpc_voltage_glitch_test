#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/dma.h"
#include "hardware/sync.h"
#include "pio_serializer.pio.h"
#include <time.h>


#define LPC_PIN_RX 0
#define LPC_PIN_TX 1
// Needs to be jumpered.
#define LPC_PIN_SWDIO 2
#define LPC_PIN_SWCLK 3

#define LPC_PIN_0_1 10
#define LPC_PIN_0_10 11
#define LPC_PIN_0_11 12
#define LPC_PIN_2_13 13

#define SIGNAL_PIN_FROM_LPC LPC_PIN_0_10

#define LPC_PIN_RESET 21
#define LPC_PIN_RESET_OUT 20

#define MAX_PIN_ENABLE 18
#define MAX_PIN_SELECTOR 19


#define DISABLEMAXOUT 1
#define ENABLEMAXOUT 0

#define CMD_DELAY 'D'
#define CMD_PULSE 'P'
#define CMD_GLITCH 'G'

void initialize_board() {
    gpio_init(MAX_PIN_ENABLE);
    gpio_put(MAX_PIN_ENABLE, DISABLEMAXOUT);
    gpio_set_dir(MAX_PIN_ENABLE, GPIO_OUT);

    gpio_init(MAX_PIN_SELECTOR);
//    gpio_put(MAX_PIN_SELECTOR, 1);
    gpio_set_dir(MAX_PIN_SELECTOR, GPIO_OUT);

    gpio_init(SIGNAL_PIN_FROM_LPC);
    gpio_set_dir(SIGNAL_PIN_FROM_LPC, GPIO_IN);
    gpio_pull_up(SIGNAL_PIN_FROM_LPC);
}

void        power_cycle_target() {
    gpio_put(MAX_PIN_ENABLE, DISABLEMAXOUT);
    sleep_ms(50);
    gpio_put(MAX_PIN_ENABLE, ENABLEMAXOUT);
}

int dma_chan;
// send a bit every PIO_SERIAL_CLKDIV clock cycles
#define PIO_SERIAL_CLKDIV 1.f

// size of glitch buffer, every entry accounts for 32 samples
#define GLITCH_BUFFER_SIZE (32)
static uint32_t wavetable[GLITCH_BUFFER_SIZE+1] = {1};

// 6x1 insta reset
#define PATTERN (0b111100000000000)
#define PATTERN_REPEAT 5
#define PATTERN_LENGTH 15


void setup_dma(uint sm) {
    uint offset = pio_add_program(pio0, &pio_serialiser_program);
    pio_serialiser_program_init(pio0, 0, offset, MAX_PIN_SELECTOR, PIO_SERIAL_CLKDIV);
    dma_chan = dma_claim_unused_channel(true);
    dma_channel_config c = dma_channel_get_default_config(dma_chan);
    channel_config_set_transfer_data_size(&c, DMA_SIZE_32);
    channel_config_set_read_increment(&c, true);
    channel_config_set_dreq(&c, pio_get_dreq(pio0, sm, true));

    dma_channel_configure(
            dma_chan,
            &c,
            &pio0_hw->txf[sm],   // Write address (only need to set this once)
            NULL,               // Don't provide a read address yet
            GLITCH_BUFFER_SIZE+1, // Write the same value many times, then halt and interrupt
            false               // Don't start yet
    );

}
void prepare_dma() {
    dma_channel_set_read_addr(dma_chan, wavetable, false);
}


void prepare_wave(uint32_t pulse, uint32_t delay) {
    // reset everything to 1
    for (int i=0; i<GLITCH_BUFFER_SIZE+1; i++) {
        wavetable[i] = 0xffffffff;
    }


    for (int i=0; i<PATTERN_REPEAT; i++) {
        for (int j=0; j< PATTERN_LENGTH; j++) {
            char a = (PATTERN >> (PATTERN_LENGTH-1-j)) & 1;
            wavetable[(delay+i*PATTERN_LENGTH+j)/32] ^= a << (delay+i*PATTERN_LENGTH+j)%32;
        }
    }
//    for (int i=0; i<pulse; i++) {
//        //wavetable[(delay+i)/32] ^= ((uint32_t)1) << (delay+i)%32;
//    }
//    uint32_t pattern = 0b11000000000000000011111111111111;
//    for (int i=0; i<GLITCH_BUFFER_SIZE; i++) {
//        wavetable[i]= pattern;
////        for (int j=0; j< 32; j++) {
////            wavetable[i] |= ((j/4) %2 ==0) <<j;
////        }
//    }


    // Debug
//
    char b;
    for (int i=0; i< GLITCH_BUFFER_SIZE+1; i++) {
        for (int j=0; j<32; j++) {
            b = (wavetable[i] >> j) & 1;
            printf("%u", b);
        }
    }
    puts("");
    printf("\n");


}
clock_t clock()
{
    return (clock_t) time_us_64() / 10000;
}

int main() {
    set_sys_clock_khz(280000, true);
    stdio_init_all();
    stdio_set_translate_crlf(&stdio_usb, false);
    initialize_board();


    const uint led_pin = 25;
    gpio_init(led_pin);
    gpio_set_dir(led_pin, GPIO_OUT);

    gpio_init(SIGNAL_PIN_FROM_LPC);

    uint32_t delay = 10;
    uint32_t pulse = 2;
    gpio_put(led_pin, 1);
    sleep_ms(1000);
    gpio_put(led_pin, 0);
    setup_dma(0);

    prepare_dma();
    prepare_wave(0, 0);
    dma_channel_start(dma_chan);
    printf("YO");
    // :) first bootstrap needs to be done this way.


    while (1) {
        //uint8_t cmd = getchar();
        uint8_t cmd = CMD_GLITCH;
        delay++;
        if (delay > 10 * 32) {
            delay = 0;
            pulse++;
            printf("\n\r\nGiro di boa %d", delay, pulse);
            sleep_ms(1000);
        }
        switch(cmd) {
//            case CMD_DELAY:
//                fread(&delay, 1, 4, stdin);
//                printf("D: %lu\n",delay);
//                break;
//            case CMD_PULSE:
//                fread(&pulse, 1, 4, stdin);
//                printf("P: %lu\n", pulse);
//                break;
            case CMD_GLITCH:
                prepare_dma();

                // uint32_t saved_interrupt_config = save_and_disable_interrupts();
                prepare_wave(pulse, delay);
                gpio_put(MAX_PIN_ENABLE, ENABLEMAXOUT);
                // power_cycle_target();
                // sleep_ms(2);
                // Timing matters here
               // printf("Wait\n");
                clock_t start = clock();

                while(gpio_get(SIGNAL_PIN_FROM_LPC) == 1);
                dma_channel_start(dma_chan);
                clock_t b = clock();


                while(gpio_get(SIGNAL_PIN_FROM_LPC) == 0);
                clock_t afterPin = clock();
                printf("\rD: %d, W: %d, Total: %.8f\n", delay, pulse, (double)(b- start)/CLOCKS_PER_SEC);
                // sleep_ms(50);
                //dma_channel_wait_for_finish_blocking(dma_chan);



        }
    }

}
