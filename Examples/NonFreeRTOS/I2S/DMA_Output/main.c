/**
  **************************************************************************
  * @file     main.c
  * @version  v2.1.2
  * @date     2022-08-16
  * @brief    main program
  **************************************************************************
  *                       Copyright notice & Disclaimer
  *
  * The software Board Support Package (BSP) that is made available to
  * download from Artery official website is the copyrighted work of Artery.
  * Artery authorizes customers to use, copy, and distribute the BSP
  * software and its related documentation for the purpose of design and
  * development in conjunction with Artery microcontrollers. Use of the
  * software is governed by this copyright notice and the following disclaimer.
  *
  * THIS SOFTWARE IS PROVIDED ON "AS IS" BASIS WITHOUT WARRANTIES,
  * GUARANTEES OR REPRESENTATIONS OF ANY KIND. ARTERY EXPRESSLY DISCLAIMS,
  * TO THE FULLEST EXTENT PERMITTED BY LAW, ALL EXPRESS, IMPLIED OR
  * STATUTORY OR OTHER WARRANTIES, GUARANTEES OR REPRESENTATIONS,
  * INCLUDING BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY,
  * FITNESS FOR A PARTICULAR PURPOSE, OR NON-INFRINGEMENT.
  * 
  * - MAX98357A / PT8211    
  * - LRC,WS      <--->     pa15 ws
  * - BCLK,BCK    <--->     pb3  ck
  * - DIN         <--->     pb5  sd
  *
  **************************************************************************
  */

#include "at32f403a_407_clock.h"
#include "at32f403a_407_delay.h"
#include "at32f403a_407_printf.h"
#include "voice.h"


/** @addtogroup AT32F403A_periph_examples
  * @{
  */

/** @addtogroup 403A_I2S_halfduplex_dma I2S_halfduplex_dma
  * @{
  */

static void gpio_config(void);
static void i2s_config(void);

/**
  * @brief  i2s configuration.
  * @param  none
  * @retval none
  */
static void i2s_config(void)
{
  dma_init_type dma_init_struct;
  i2s_init_type i2s_init_struct;

  // DMA2_CHANNEL2 -> SPI3/I2S3_TX
  crm_periph_clock_enable(CRM_DMA2_PERIPH_CLOCK, TRUE);
  dma_reset(DMA2_CHANNEL2);
  dma_default_para_init(&dma_init_struct);
  dma_init_struct.buffer_size = VOICE_BULK_SIZE;
  dma_init_struct.direction = DMA_DIR_MEMORY_TO_PERIPHERAL;
  dma_init_struct.memory_base_addr = (uint32_t)voice_bulk;
  dma_init_struct.memory_data_width = DMA_MEMORY_DATA_WIDTH_HALFWORD;
  dma_init_struct.memory_inc_enable = TRUE;
  dma_init_struct.peripheral_base_addr = (uint32_t)0x40003C0C;
  dma_init_struct.peripheral_data_width = DMA_PERIPHERAL_DATA_WIDTH_HALFWORD;
  dma_init_struct.peripheral_inc_enable = FALSE;
  dma_init_struct.priority = DMA_PRIORITY_HIGH;
  dma_init_struct.loop_mode_enable = TRUE;
  dma_init(DMA2_CHANNEL2, &dma_init_struct);

  crm_periph_clock_enable(CRM_SPI3_PERIPH_CLOCK, TRUE);

  i2s_default_para_init(&i2s_init_struct);
  i2s_init_struct.audio_protocol = I2S_AUDIO_PROTOCOL_LSB;
  i2s_init_struct.data_channel_format = I2S_DATA_16BIT_CHANNEL_16BIT;
  i2s_init_struct.mclk_output_enable = TRUE;
  i2s_init_struct.audio_sampling_freq = I2S_AUDIO_FREQUENCY_8K;
  i2s_init_struct.clock_polarity = I2S_CLOCK_POLARITY_LOW;
  i2s_init_struct.operation_mode = I2S_MODE_MASTER_TX;
  i2s_init(SPI3, &i2s_init_struct);

  dma_channel_enable(DMA2_CHANNEL2, TRUE);
  i2s_enable(SPI3, TRUE);
}

/**
  * @brief  gpio configuration.
  * @param  none
  * @retval none
  */
static void gpio_config(void)
{
  gpio_init_type gpio_initstructure;
  crm_periph_clock_enable(CRM_GPIOA_PERIPH_CLOCK, TRUE);
  crm_periph_clock_enable(CRM_GPIOB_PERIPH_CLOCK, TRUE);
  crm_periph_clock_enable(CRM_GPIOC_PERIPH_CLOCK, TRUE);
  crm_periph_clock_enable(CRM_IOMUX_PERIPH_CLOCK, TRUE);
  gpio_pin_remap_config(SWJTAG_GMUX_010, TRUE);

  /* master ws pin */
  gpio_initstructure.gpio_out_type       = GPIO_OUTPUT_PUSH_PULL;
  gpio_initstructure.gpio_pull           = GPIO_PULL_UP;
  gpio_initstructure.gpio_mode           = GPIO_MODE_MUX;
  gpio_initstructure.gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER;
  gpio_initstructure.gpio_pins           = GPIO_PINS_15;
  gpio_init(GPIOA, &gpio_initstructure);

  /* master ck pin */
  gpio_initstructure.gpio_pull           = GPIO_PULL_DOWN;
  gpio_initstructure.gpio_mode           = GPIO_MODE_MUX;
  gpio_initstructure.gpio_pins           = GPIO_PINS_3;
  gpio_init(GPIOB, &gpio_initstructure);

  /* master sd pin */
  gpio_initstructure.gpio_pull           = GPIO_PULL_UP;
  gpio_initstructure.gpio_mode           = GPIO_MODE_MUX;
  gpio_initstructure.gpio_pins           = GPIO_PINS_5;
  gpio_init(GPIOB, &gpio_initstructure);

  /* master mck pin */
  gpio_initstructure.gpio_pull           = GPIO_PULL_UP;
  gpio_initstructure.gpio_mode           = GPIO_MODE_MUX;
  gpio_initstructure.gpio_pins           = GPIO_PINS_7;
  gpio_init(GPIOC, &gpio_initstructure);
}

/**
  * @brief  main function.
  * @param  none
  * @retval none
  */
int main(void)
{
  sclk_240m_hext_config();
  delay_init();
  uart_print_init(115200);
  printf("start test..\r\n");

  gpio_config();
  i2s_config();
  spi_i2s_dma_transmitter_enable(SPI3, TRUE);
  while(1);
}

/**
  * @}
  */

/**
  * @}
  */
