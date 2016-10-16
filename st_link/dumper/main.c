#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/usart.h>

#define LED_PIN         GPIO9

static void delay(int x) {
    for (int i = 0; i < x; i++)
        for (int j = 0; j < 4000; j++)
            __asm__("nop");
}

static void clock_setup(void) {
    rcc_clock_setup_in_hse_8mhz_out_72mhz();
    rcc_periph_clock_enable(RCC_USART2);
    rcc_periph_clock_enable(RCC_GPIOA);
}

static void usart_setup(void) {
    gpio_set_mode(GPIOA, GPIO_MODE_OUTPUT_50_MHZ,
            GPIO_CNF_OUTPUT_ALTFN_PUSHPULL, GPIO_USART2_TX);

    usart_set_baudrate(USART2, 9600);
    usart_set_databits(USART2, 8);
    usart_set_stopbits(USART2, USART_STOPBITS_1);
    usart_set_mode(USART2, USART_MODE_TX);
    usart_set_parity(USART2, USART_PARITY_NONE);
    usart_set_flow_control(USART2, USART_FLOWCONTROL_NONE);

    usart_enable(USART2);
}

int main(void) {
    clock_setup();
    usart_setup();
    gpio_set_mode(GPIOA, GPIO_MODE_OUTPUT_50_MHZ,
            GPIO_CNF_OUTPUT_PUSHPULL, LED_PIN);
    gpio_set(GPIOA, LED_PIN);

    /* Mark firmware start */
    usart_send_blocking(USART2, 0xAB);
    usart_send_blocking(USART2, 0xCD);

    for (uint32_t ptr = 0x08000000; ptr <= 0x08000000 + 0x4000; ptr++) {
        usart_send_blocking(USART2, *((uint16_t *) ptr));
        delay(1);
    }

    /* Indicate that we're done */
    gpio_clear(GPIOA, LED_PIN);
}
