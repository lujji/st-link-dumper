#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/usart.h>

#define LED_PIN         GPIO9

#ifdef REMAP_USART1
#define DUMP_USART USART1
#define DUMP_USART_TX GPIO_USART1_RE_TX
#define DUMP_USART_PORT GPIOB
#define DUMP_USART_RCC RCC_USART1
#else
#define DUMP_USART USART2
#define DUMP_USART_TX GPIO_USART2_TX
#define DUMP_USART_PORT GPIOA
#define DUMP_USART_RCC RCC_USART2
#endif

static void delay(int x) {
    for (int i = 0; i < x; i++)
        for (int j = 0; j < 4000; j++)
            __asm__("nop");
}

static void clock_setup(void) {
    rcc_clock_setup_in_hse_8mhz_out_72mhz();
    rcc_periph_clock_enable(DUMP_USART_RCC);
    rcc_periph_clock_enable(RCC_GPIOA);
#ifdef REMAP_USART1
    rcc_periph_clock_enable(RCC_GPIOB);
    rcc_periph_clock_enable(RCC_AFIO);
#endif
}

static void usart_setup(void) {
#ifdef REMAP_USART1
    gpio_primary_remap(0, AFIO_MAPR_USART1_REMAP);
#endif
    gpio_set_mode(DUMP_USART_PORT, GPIO_MODE_OUTPUT_50_MHZ,
            GPIO_CNF_OUTPUT_ALTFN_PUSHPULL, DUMP_USART_TX);

    usart_set_baudrate(DUMP_USART, 9600);
    usart_set_databits(DUMP_USART, 8);
    usart_set_stopbits(DUMP_USART, USART_STOPBITS_1);
    usart_set_mode(DUMP_USART, USART_MODE_TX);
    usart_set_parity(DUMP_USART, USART_PARITY_NONE);
    usart_set_flow_control(DUMP_USART, USART_FLOWCONTROL_NONE);

    usart_enable(DUMP_USART);
}

int main(void) {
    clock_setup();
    usart_setup();
    gpio_set_mode(GPIOA, GPIO_MODE_OUTPUT_50_MHZ,
            GPIO_CNF_OUTPUT_PUSHPULL, LED_PIN);
    gpio_set(GPIOA, LED_PIN);

    /* Mark firmware start */
    usart_send_blocking(DUMP_USART, 0xAB);
    usart_send_blocking(DUMP_USART, 0xCD);

    for (uint32_t ptr = 0x08000000; ptr <= 0x08000000 + 0x4000; ptr++) {
        usart_send_blocking(DUMP_USART, *((uint16_t *) ptr));
        delay(1);
    }

    /* Indicate that we're done */
    gpio_clear(GPIOA, LED_PIN);
}
