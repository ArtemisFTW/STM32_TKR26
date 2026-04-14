/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

void HAL_TIM_MspPostInit(TIM_HandleTypeDef *htim);

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define BTS1L_Pin GPIO_PIN_5
#define BTS1L_GPIO_Port GPIOA
#define CYTAN1_Pin GPIO_PIN_7
#define CYTAN1_GPIO_Port GPIOA
#define CYTIN1_Pin GPIO_PIN_4
#define CYTIN1_GPIO_Port GPIOC
#define CYTIN2_Pin GPIO_PIN_5
#define CYTIN2_GPIO_Port GPIOC
#define BTS4L_Pin GPIO_PIN_0
#define BTS4L_GPIO_Port GPIOB
#define BTS4R_Pin GPIO_PIN_1
#define BTS4R_GPIO_Port GPIOB
#define BTS2R_Pin GPIO_PIN_2
#define BTS2R_GPIO_Port GPIOB
#define BTS2L_Pin GPIO_PIN_10
#define BTS2L_GPIO_Port GPIOB
#define ENC1A_Pin GPIO_PIN_12
#define ENC1A_GPIO_Port GPIOB
#define ENC1A_EXTI_IRQn EXTI15_10_IRQn
#define ENC2A_Pin GPIO_PIN_13
#define ENC2A_GPIO_Port GPIOB
#define ENC2A_EXTI_IRQn EXTI15_10_IRQn
#define ENC3A_Pin GPIO_PIN_14
#define ENC3A_GPIO_Port GPIOB
#define ENC3A_EXTI_IRQn EXTI15_10_IRQn
#define ENC4A_Pin GPIO_PIN_15
#define ENC4A_GPIO_Port GPIOB
#define ENC4A_EXTI_IRQn EXTI15_10_IRQn
#define ENC1B_Pin GPIO_PIN_8
#define ENC1B_GPIO_Port GPIOC
#define ENC2B_Pin GPIO_PIN_9
#define ENC2B_GPIO_Port GPIOC
#define ENC3B_Pin GPIO_PIN_8
#define ENC3B_GPIO_Port GPIOA
#define ENC4B_Pin GPIO_PIN_11
#define ENC4B_GPIO_Port GPIOA
#define BTS1R_Pin GPIO_PIN_3
#define BTS1R_GPIO_Port GPIOB
#define BTS3L_Pin GPIO_PIN_4
#define BTS3L_GPIO_Port GPIOB
#define BTS3R_Pin GPIO_PIN_5
#define BTS3R_GPIO_Port GPIOB
#define CYTAN2_Pin GPIO_PIN_7
#define CYTAN2_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
