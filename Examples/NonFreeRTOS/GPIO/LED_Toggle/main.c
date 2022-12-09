#include "at32f403a_407_clock.h"
#include "at32f403a_407_delay.h"
#include "at32f403a_407_printf.h"


/**
  * @brief  configure led gpio
  * @retval none
  */
void at32_led_init(void)
{
  gpio_init_type gpio_init_struct;

  /* enable the led clock */
  crm_periph_clock_enable(CRM_GPIOC_PERIPH_CLOCK, TRUE);

  /* set default parameter */
  gpio_default_para_init(&gpio_init_struct);

  /* configure the led gpio */
  gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER;
  gpio_init_struct.gpio_out_type  = GPIO_OUTPUT_PUSH_PULL;
  gpio_init_struct.gpio_mode = GPIO_MODE_OUTPUT;
  gpio_init_struct.gpio_pins = GPIO_PINS_13;
  gpio_init_struct.gpio_pull = GPIO_PULL_NONE;
  gpio_init(GPIOC, &gpio_init_struct);
}

/**
  * @brief  turns selected led on.
  * @retval none
  */
void at32_led_on(void)
{
  GPIOC->clr = GPIO_PINS_13;
}

/**
  * @brief  turns selected led off.
  * @retval none
  */
void at32_led_off()
{
  GPIOC->scr = GPIO_PINS_13;
}

/**
  * @brief  turns selected led toggle.
  * @retval none
  */
void at32_led_toggle(void)
{
  GPIOC->odt ^= GPIO_PINS_13;
}

/**
  * @brief  main function.
  * @param  none
  * @retval none
  */
int main(void)
{
  sclk_240m_hext_config();

  uart_print_init(115200);

  /* initialize delay function */
  delay_init();

  at32_led_init();
  at32_led_off();

  while(1)
  {
    printf("echo\r\n");
    at32_led_toggle();
    delay_ms(1000);
  }
}
