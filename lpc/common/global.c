#include <global.h>

//int stack_Keypad, stack_Counter, stack_LCD, stack_ADC = 0;
//
//TickType_t xDelay1 = TicksPerMS * 1;
//TickType_t xDelay10 = TicksPerMS * 10;
//TickType_t xDelay25 = TicksPerMS * 25;
//TickType_t xDelay50 = TicksPerMS * 50;
//TickType_t xDelay100 = TicksPerMS * 100;
//TickType_t xDelay200 = TicksPerMS * 200;
//TickType_t xDelay150 = TicksPerMS * 150;
//TickType_t xDelay250 = TicksPerMS * 250;
//TickType_t xDelay500 = TicksPerMS * 500;
//TickType_t xDelay1000 = TicksPerMS * 1000;


volatile uint32_t debug1 = 0;

volatile uint32_t cpuID = 0;

//volatile TaskHandle_t xHandle[10] = {NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL };
volatile uint8_t taskcounter = 0;
volatile uint64_t globaltickms = 0;
volatile uint32_t watchdogSPI = 0;

//
//xQueueHandle xScreenMsgQueue;
//xQueueHandle xMotorMsgQueue;
//xQueueHandle xSensorQueue;
//
//xQueueHandle xBoundaryMsgQueue;

/*
typedef void (*IAP)(uint32_t[5], uint32_t[5]);
const IAP IAP_entry = (IAP)0x1FFF1FF1;

int iap_read_id(void)
{
    unsigned int cmd[5], res[5];
    cmd[0] = 54;
    IAP_entry(cmd, res);
    return ((int)res[1]);
}
*/
