/**
  **************************************************************************
  * @file     main.c
  * @version  v2.1.2
  * @date     2022-08-16
  * @brief    main program
  **************************************************************************
  * 
  *   sdio1                       at32f403a           sd/mmc card
  * - sdio1_d0                    pc8          --->   dat0
  * - sdio1_d1                    pc9          --->   dat1
  * - sdio1_d2                    pc10         --->   dat2
  * - sdio1_d3                    pc11         --->   dat3
  * - sdio1_ck                    pc12         --->   clk
  * - sdio1_cmd                   pd2          --->   cmd
  *
  * THIS SOFTWARE IS PROVIDED ON "AS IS" BASIS WITHOUT WARRANTIES,
  * GUARANTEES OR REPRESENTATIONS OF ANY KIND. ARTERY EXPRESSLY DISCLAIMS,
  * TO THE FULLEST EXTENT PERMITTED BY LAW, ALL EXPRESS, IMPLIED OR
  * STATUTORY OR OTHER WARRANTIES, GUARANTEES OR REPRESENTATIONS,
  * INCLUDING BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY,
  * FITNESS FOR A PARTICULAR PURPOSE, OR NON-INFRINGEMENT.
  *
  **************************************************************************
  */

#include <stdbool.h>
#include <string.h>

#include "at32f403a_407_clock.h"
#include "at32f403a_407_delay.h"
#include "at32f403a_407_printf.h"
#include "at32f403a_407_bsp_sdio.h"
#include "ff.h"

/** @addtogroup AT32F403A_periph_examples
  * @{
  */

/** @addtogroup 403A_SDIO_sdio_fatfs SDIO_sdio_fatfs
  * @{
  */

FATFS fs;
FIL file;
BYTE work[FF_MAX_SS];

uint8_t buffer_compare(uint8_t* pbuffer1, uint8_t* pbuffer2, uint16_t buffer_length);
static void nvic_configuration(void);

/**
  * @brief  compares two buffers.
  * @param  pbuffer1, pbuffer2: buffers to be compared.
  * @param  buffer_length: buffer's length
  * @retval 1: pbuffer1 identical to pbuffer2
  *         0: pbuffer1 differs from pbuffer2
  */
uint8_t buffer_compare(uint8_t* pbuffer1, uint8_t* pbuffer2, uint16_t buffer_length)
{
  while(buffer_length--)
  {
    if(*pbuffer1 != *pbuffer2)
    {
      return 0;
    }
    pbuffer1++;
    pbuffer2++;
  }
  return 1;
}

/**
  * @brief  configures sdio1 irq channel.
  * @param  none
  * @retval none
  */
static void nvic_configuration(void)
{
  nvic_priority_group_config(NVIC_PRIORITY_GROUP_4);
  nvic_irq_enable(SDIO1_IRQn, 0, 0);
}

/**
  * @brief  fatfs file read/write test.
  * @param  none
  * @retval ERROR: fail.
  *         SUCCESS: success.
  */
static error_status fatfs_test(void)
{
  FRESULT ret;
  char filename[] = "1:/test1.txt";
  const char wbuf[] = "this is my file for test fatfs!\r\n";
  char rbuf[50];
  UINT bytes_written = 0;
  UINT bytes_read = 0;
  DWORD fre_clust, fre_sect, tot_sect;
  FATFS* pt_fs;

  printf("fs mount \r\n");
  ret = f_mount(&fs, "1:", 1);
  if(ret){
    printf("fs mount err:%d.\r\n", ret);
    if(ret == FR_NO_FILESYSTEM){
      printf("create fatfs..\r\n");
      ret = f_mkfs("1:", 0, work, sizeof(work));
      if(ret){
        printf("creates fatfs err:%d.\r\n", ret);
        return ERROR;
      }
      else{
        printf("creates fatfs ok.\r\n");
      }
      ret = f_mount(NULL, "1:", 1);
      ret = f_mount(&fs, "1:", 1);
      if(ret){
        printf("fs mount err:%d.\r\n", ret);
        return ERROR;
      }
      else{
        printf("fs mount ok.\r\n");
      }
    }
    else{
      return ERROR;
    }
  }
  else{
    printf("fs mount ok.\r\n");
  }

  ret = f_open(&file, filename, FA_READ | FA_WRITE | FA_CREATE_ALWAYS);
  if(ret){
    printf("open file err:%d.\r\n", ret);
  }
  else{
    printf("open file ok.\r\n");
  }
  ret = f_write(&file, wbuf, sizeof(wbuf), &bytes_written);
  if(ret){
    printf("write file err:%d.\r\n", ret);
  }
  else{
    printf("write file ok, byte:%u.\r\n", bytes_written);
  }
  f_lseek(&file, 0);
  ret = f_read(&file, rbuf, sizeof(rbuf), &bytes_read);
  if(ret){
    printf("read file err:%d.\r\n", ret);
  }
  else{
    printf("read file ok, byte:%u.\r\n", bytes_read);
  }
  ret = f_close(&file);
  if(ret){
    printf("close file err:%d.\r\n", ret);
  }
  else{
    printf("close file ok.\r\n");
  }

  pt_fs = &fs;
  /* get volume information and free clusters of drive 1 */
  ret = f_getfree("1:", &fre_clust, &pt_fs);
  if(ret == FR_OK)
  {
    /* get total sectors and free sectors */
    tot_sect = (pt_fs->n_fatent - 2) * pt_fs->csize;
    fre_sect = fre_clust * pt_fs->csize;

    /* print the free space (assuming 512 bytes/sector) */
    printf("%10lu KiB total drive space.\r\n%10lu KiB available.\r\n", tot_sect / 2, fre_sect / 2);
  }

  ret = f_mount(NULL, "1:", 1);

  if(1 == buffer_compare((uint8_t*)rbuf, (uint8_t*)wbuf, sizeof(wbuf))){
    printf("r/w file data test ok.\r\n");
  }
  else{
    printf("r/w file data test fail.\r\n");
    return ERROR;
  }

  return SUCCESS;
}

/* gloable functions ---------------------------------------------------------*/
/**
  * @brief  main function.
  * @param  none
  * @retval none
  */
int main(void)
{
  sclk_240m_hext_config();

  delay_init();

  nvic_configuration();

  uart_print_init(115200);
  printf("start test fatfs r0.14b..\r\n");

  if(SUCCESS != fatfs_test())
  {
    printf("test error\r\n");
  }
  else
  {
    printf("test passed\r\n");
  }

  while(1);
}

/**
  * @}
  */

/**
  * @}
  */
