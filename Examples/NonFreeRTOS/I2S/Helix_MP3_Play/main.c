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
  * -                MAX98357A / PT8211
  * - pa15 ws    ->  LRC,WS
  * - pb3  ck    ->  BCLK,BCK
  * - pb5  sd    ->  DIN
  *
  **************************************************************************
  */

#include "at32f403a_407_clock.h"
#include "at32f403a_407_delay.h"
#include "at32f403a_407_printf.h"
#include "mp3dec.h"
#include "mp3_data.h"


static int16_t audio_buffer[2][4096];
volatile uint32_t time_var1, time_var2, audio_buffer_idx = 0, load_flag = 0;
volatile uint16_t samples_size = 4096;
MP3FrameInfo mp3FrameInfo;
HMP3Decoder hMP3Decoder;

static void dma_config(void)
{
  dma_init_type dma_init_struct;

  // DMA2_CHANNEL2 -> SPI3/I2S3_TX
  crm_periph_clock_enable(CRM_DMA2_PERIPH_CLOCK, TRUE);
  dma_reset(DMA2_CHANNEL2);
  dma_default_para_init(&dma_init_struct);
  dma_init_struct.buffer_size = samples_size;
  dma_init_struct.direction = DMA_DIR_MEMORY_TO_PERIPHERAL;
  dma_init_struct.memory_base_addr = (uint32_t)audio_buffer[audio_buffer_idx];
  dma_init_struct.memory_data_width = DMA_MEMORY_DATA_WIDTH_HALFWORD;
  dma_init_struct.memory_inc_enable = TRUE;
  dma_init_struct.peripheral_base_addr = (uint32_t)0x40003C0C;
  dma_init_struct.peripheral_data_width = DMA_PERIPHERAL_DATA_WIDTH_HALFWORD;
  dma_init_struct.peripheral_inc_enable = FALSE;
  dma_init_struct.priority = DMA_PRIORITY_HIGH;
  dma_init_struct.loop_mode_enable = TRUE;
  dma_init(DMA2_CHANNEL2, &dma_init_struct);

  /* enable transfer half data intterrupt */
  dma_interrupt_enable(DMA2_CHANNEL2, DMA_FDT_INT, TRUE);

  /* dma channel interrupt nvic init */
  nvic_irq_enable(DMA2_Channel2_IRQn, 1, 0);

  dma_channel_enable(DMA2_CHANNEL2, TRUE);
}

/**
  * @brief  i2s configuration.
  * @param  none
  * @retval none
  */
static void i2s_config(void)
{
  i2s_init_type i2s_init_struct;

  crm_periph_clock_enable(CRM_SPI3_PERIPH_CLOCK, TRUE);

  i2s_default_para_init(&i2s_init_struct);
  // LSB for PT8211, PHILLIPS for MAX98357A
  i2s_init_struct.audio_protocol = I2S_AUDIO_PROTOCOL_LSB;
  i2s_init_struct.data_channel_format = I2S_DATA_16BIT_CHANNEL_16BIT;
  i2s_init_struct.mclk_output_enable = TRUE;
  i2s_init_struct.audio_sampling_freq = I2S_AUDIO_FREQUENCY_44_1K;
  i2s_init_struct.clock_polarity = I2S_CLOCK_POLARITY_LOW;
  i2s_init_struct.operation_mode = I2S_MODE_MASTER_TX;
  i2s_init(SPI3, &i2s_init_struct);

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

void load_next_frame(void)
{
  int offset, err;
  int outOfData = 0;
  static const char *read_ptr = mp3_data;
  static int bytes_left = MP3_DATA_SIZE;

  int16_t *samples = audio_buffer[1 - audio_buffer_idx];
  /* prepare new buffer */
  offset = MP3FindSyncWord((unsigned char *)read_ptr, bytes_left);
  bytes_left -= offset;

  if (bytes_left <= 5000)
  {
    /* start over */
    read_ptr = mp3_data;
    bytes_left = MP3_DATA_SIZE;
    offset = MP3FindSyncWord((unsigned char *)read_ptr, bytes_left);
  }

  /* decocd for one frame */
  read_ptr += offset;
  err = MP3Decode(hMP3Decoder, (unsigned char **)&read_ptr, &bytes_left, samples, 0);

  if (err)
  {
    /* error occurred */
    switch (err)
    {
    case ERR_MP3_INDATA_UNDERFLOW:
      outOfData = 1;
      break;
    case ERR_MP3_MAINDATA_UNDERFLOW:
      /* do nothing - next call to decode will provide more mainData */
      break;
    case ERR_MP3_FREE_BITRATE_SYNC:
    default:
      outOfData = 1;
      break;
    }
  }
  else
  {
    /* no error, read samples size */
    MP3GetLastFrameInfo(hMP3Decoder, &mp3FrameInfo);
  }

  if (!outOfData)
  {
    samples_size = mp3FrameInfo.outputSamps;
    load_flag = 0;
  }
}

/**
  * @brief  main function.
  * @param  none
  * @retval none
  */
int main(void)
{
  sclk_240m_hext_config();
  nvic_priority_group_config(NVIC_PRIORITY_GROUP_4);

  delay_init();
  uart_print_init(115200);
  printf("start test..\r\n");

  gpio_config();
  i2s_config();
  dma_config();

  hMP3Decoder = MP3InitDecoder();

  spi_i2s_dma_transmitter_enable(SPI3, TRUE);
  while(1)
  {
    if (load_flag == 1)
    {
      load_next_frame();
    }
  }
}

void DMA2_Channel2_IRQHandler(void)
{
  int16_t *samples;

  if (dma_flag_get(DMA2_FDT2_FLAG) != RESET)
  {
    audio_buffer_idx = 1 - audio_buffer_idx;
    samples = audio_buffer[audio_buffer_idx];
    // switch to another buffer
    DMA2_CHANNEL2->ctrl_bit.chen = FALSE;
    DMA2_CHANNEL2->maddr = (uint32_t)samples;
    DMA2_CHANNEL2->dtcnt = samples_size;
    DMA2_CHANNEL2->ctrl_bit.chen = TRUE;
    dma_flag_clear(DMA2_FDT2_FLAG);
    // set the flag for next frame loading
    load_flag = 1;
  }
}
