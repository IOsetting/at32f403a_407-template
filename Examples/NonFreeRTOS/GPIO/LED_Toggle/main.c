#include "at32f403a_407_clock.h"


/* delay macros */
#define STEP_DELAY_MS                    50

/* delay variable */
static __IO uint32_t fac_us;
static __IO uint32_t fac_ms;

/**
  * @brief  initialize delay function
  * @param  none
  * @retval none
  */
void delay_init()
{
  /* configure systick */
  systick_clock_source_config(SYSTICK_CLOCK_SOURCE_AHBCLK_NODIV);
  fac_us = system_core_clock / (1000000U);
  fac_ms = fac_us * (1000U);
}

/**
  * @brief  inserts a delay time.
  * @param  nus: specifies the delay time length, in microsecond.
  * @retval none
  */
void delay_us(uint32_t nus)
{
  uint32_t temp = 0;
  SysTick->LOAD = (uint32_t)(nus * fac_us);
  SysTick->VAL = 0x00;
  SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk ;
  do
  {
    temp = SysTick->CTRL;
  }while((temp & 0x01) && !(temp & (1 << 16)));

  SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;
  SysTick->VAL = 0x00;
}

/**
  * @brief  inserts a delay time.
  * @param  nms: specifies the delay time length, in milliseconds.
  * @retval none
  */
void delay_ms(uint16_t nms)
{
  uint32_t temp = 0;
  while(nms)
  {
    if(nms > STEP_DELAY_MS)
    {
      SysTick->LOAD = (uint32_t)(STEP_DELAY_MS * fac_ms);
      nms -= STEP_DELAY_MS;
    }
    else
    {
      SysTick->LOAD = (uint32_t)(nms * fac_ms);
      nms = 0;
    }
    SysTick->VAL = 0x00;
    SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;
    do
    {
      temp = SysTick->CTRL;
    }while((temp & 0x01) && !(temp & (1 << 16)));

    SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;
    SysTick->VAL = 0x00;
  }
}

/**
  * @brief  inserts a delay time.
  * @param  sec: specifies the delay time, in seconds.
  * @retval none
  */
void delay_sec(uint16_t sec)
{
  uint16_t index;
  for(index = 0; index < sec; index++)
  {
    delay_ms(500);
    delay_ms(500);
  }
}

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
  system_clock_config();

  /* initialize delay function */
  delay_init();

  at32_led_init();
  at32_led_off();

  while(1)
  {
    at32_led_toggle();
    delay_ms(1000);
  }
}
