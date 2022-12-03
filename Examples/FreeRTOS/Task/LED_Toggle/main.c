#include "at32f403a_407_clock.h"
#include "at32f403a_407_printf.h"
#include <stdio.h>
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

/**
  * @brief  configure led gpio
  * @retval none
  */
void at32_led_init(void)
{
  gpio_init_type gpio_init_struct;

  /* enable the led clock */
  crm_periph_clock_enable(CRM_GPIOB_PERIPH_CLOCK, TRUE);
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
  gpio_init_struct.gpio_pins = GPIO_PINS_12|GPIO_PINS_13;
  gpio_init(GPIOB, &gpio_init_struct);
}

/**
  * @brief  turns selected led on.
  * @retval none
  */
void at32_led_on(void)
{
  GPIOB->clr = GPIO_PINS_12;
  GPIOB->clr = GPIO_PINS_13;
  GPIOC->clr = GPIO_PINS_13;
}

/**
  * @brief  turns selected led off.
  * @retval none
  */
void at32_led_off()
{
  GPIOB->scr = GPIO_PINS_12;
  GPIOB->scr = GPIO_PINS_13;
  GPIOC->scr = GPIO_PINS_13;
}

void task1(void *pvParameters)
{
    (void)(pvParameters);

    while (1)
    {
      // led toggle
      GPIOB->odt ^= GPIO_PINS_12;
      vTaskDelay(2000);
    }
}

void task2(void *pvParameters)
{
    (void)(pvParameters);

    while (1)
    {
      printf("Task 2 printf\r\n");
      vTaskDelay(1000);
    }
}

void task3(void *pvParameters)
{
    (void)(pvParameters);

    while (1)
    {
      // led toggle
      GPIOB->odt ^= GPIO_PINS_13;
      vTaskDelay(800);
    }
}

void task4(void *pvParameters)
{
    (void)(pvParameters);

    while (1)
    {
      // led toggle
      GPIOC->odt ^= GPIO_PINS_13;
      vTaskDelay(400);
    }
}

/**
  * @brief  main function.
  * @param  none
  * @retval none
  */
int main(void)
{
  BaseType_t xReturned;

  nvic_priority_group_config(NVIC_PRIORITY_GROUP_4);

  sclk_240m_hext_config();

  at32_led_init();

  uart_print_init(115200);

  taskENTER_CRITICAL();

  xReturned = xTaskCreate(
        task1,                      // Task function point
        "Task1",                    // Task name
        configMINIMAL_STACK_SIZE,   // Use the minimum stack size, each take 4 bytes(32bit)
        NULL,                       // Parameters
        2,                          // Priority
        NULL);                      // Task handler

  if (xReturned != pdPASS)
  {
      while (1);
  }

  xReturned = xTaskCreate(task2, "Task2", configMINIMAL_STACK_SIZE, NULL, 2, NULL);
  if (xReturned != pdPASS)
  {
      while (1);
  }
  xReturned = xTaskCreate(task3, "Task3", configMINIMAL_STACK_SIZE, NULL, 2, NULL);
  if (xReturned != pdPASS)
  {
      while (1);
  }
  xReturned = xTaskCreate(task4, "Task4", configMINIMAL_STACK_SIZE, NULL, 2, NULL);
  if (xReturned != pdPASS)
  {
      while (1);
  }

  taskEXIT_CRITICAL();

  printf("FreeRTOS Scheduler starting...\r\n");

  /* Start the scheduler. */
  vTaskStartScheduler();

  /* Will only get here if there was not enough heap space to create the idle task. */
  return 0;
}
