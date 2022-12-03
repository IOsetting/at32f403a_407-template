#include "at32f403a_407_clock.h"
#include "at32f403a_407_delay.h"


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
  * @brief  configure button gpio
  * @param  none
  * @retval none
  */
void at32_button_init(void)
{
  gpio_init_type gpio_init_struct;

  /* enable the button clock */
  crm_periph_clock_enable(CRM_GPIOA_PERIPH_CLOCK, TRUE);

  /* set default parameter */
  gpio_default_para_init(&gpio_init_struct);

  /* configure button pin as input with pull-up/pull-down */
  gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER;
  gpio_init_struct.gpio_out_type  = GPIO_OUTPUT_PUSH_PULL;
  gpio_init_struct.gpio_mode = GPIO_MODE_INPUT;
  gpio_init_struct.gpio_pins = GPIO_PINS_0;
  /* pull up because the button is connected between pa0 and gound */
  gpio_init_struct.gpio_pull = GPIO_PULL_UP;
  gpio_init(GPIOA, &gpio_init_struct);
}

/**
  * @brief  configure the gpio of swj-dp.
  * @param  none
  * @retval none
  */
void swj_dp_config(void)
{
  gpio_init_type gpio_init_struct;

  crm_periph_clock_enable(CRM_IOMUX_PERIPH_CLOCK, TRUE);
  crm_periph_clock_enable(CRM_GPIOA_PERIPH_CLOCK, TRUE);
  crm_periph_clock_enable(CRM_GPIOB_PERIPH_CLOCK, TRUE);

  /* configure pa13 (jtms/swdat), pa14 (jtck/swclk) and pa15 (jtdi) as output push-pull */
  gpio_init_struct.gpio_pins = GPIO_PINS_13 | GPIO_PINS_14 | GPIO_PINS_15;
  gpio_init_struct.gpio_mode = GPIO_MODE_OUTPUT;
  gpio_init_struct.gpio_pull = GPIO_PULL_NONE;
  gpio_init_struct.gpio_out_type = GPIO_OUTPUT_PUSH_PULL;
  gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER;
  gpio_init(GPIOA, &gpio_init_struct);

  /* configure pb3 (jtdo) and pb4 (jtrst) as output push-pull */
  gpio_init_struct.gpio_pins = GPIO_PINS_3 | GPIO_PINS_4;
  gpio_init(GPIOB, &gpio_init_struct);
}

/**
  * @brief  toggles the specified gpio pin
  * @param  gpiox: where x can be (a..g depending on device used) to select the gpio peripheral
  * @param  gpio_pin: specifies the pins to be toggled.
  * @retval none
  */
void gpio_pins_toggle(gpio_type* gpio_x, uint16_t gpio_pin)
{
  gpio_x->odt ^= gpio_pin;
}

/**
  * @brief  main function.
  * @param  none
  * @retval none
  */
int main(void)
{
  sclk_240m_hick_config();

  /* initialize delay function */
  delay_init();

  at32_button_init();
  at32_led_init();
  /* preserve the time window in case anything going wrong */
  delay_ms(2000);
  /* config pa13,pa14,pa15,pb3,pb4 as gpio output, waiting to be remapped */
  swj_dp_config();

  while(1)
  {
    if(gpio_input_data_bit_read(GPIOA, GPIO_PINS_0) == RESET)
    {
      /* Turn on the remap, then those pins start output */
      gpio_pin_remap_config(SWJTAG_MUX_100, TRUE);
      gpio_bits_set(GPIOC, GPIO_PINS_13);
      delay_ms(1000);
    }

    gpio_pins_toggle(GPIOC, GPIO_PINS_13);
    delay_ms(200);

    /* toggle jtms/swdat pin */
    gpio_pins_toggle(GPIOA, GPIO_PINS_13);
    delay_us(200);

    /* toggle jtck/swclk pin */
    gpio_pins_toggle(GPIOA, GPIO_PINS_14);
    delay_us(200);

    /* toggle jtdi pin */
    gpio_pins_toggle(GPIOA, GPIO_PINS_15);
    delay_us(200);

    /* toggle jtdo pin */
    gpio_pins_toggle(GPIOB, GPIO_PINS_3);
    delay_us(200);

    /* toggle jtrst pin */
    gpio_pins_toggle(GPIOB, GPIO_PINS_4);
    delay_us(200);
  }
}
