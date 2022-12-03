#ifndef __AT32F403A_407_DELAY_H
#define __AT32F403A_407_DELAY_H

#ifdef __cplusplus
extern "C" {
#endif

#include "at32f403a_407.h"

/**
  * @brief  initialize delay function
  * @param  none
  * @retval none
  */
void delay_init(void);

/**
  * @brief  inserts a delay time.
  * @param  nus: specifies the delay time length, in microsecond.
  * @retval none
  */
void delay_us(uint32_t nus);

/**
  * @brief  inserts a delay time.
  * @param  nms: specifies the delay time length, in milliseconds.
  * @retval none
  */
void delay_ms(uint16_t nms);

/**
  * @brief  inserts a delay time.
  * @param  sec: specifies the delay time, in seconds.
  * @retval none
  */
void delay_sec(uint16_t sec);


#ifdef __cplusplus
}
#endif

#endif

