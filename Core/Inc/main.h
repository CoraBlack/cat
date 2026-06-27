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
#define Motor_Left_AIN1_Pin GPIO_PIN_0
#define Motor_Left_AIN1_GPIO_Port GPIOC
#define Motor_Left_AIN2_Pin GPIO_PIN_1
#define Motor_Left_AIN2_GPIO_Port GPIOC
#define Motor_Right_BIN1_Pin GPIO_PIN_2
#define Motor_Right_BIN1_GPIO_Port GPIOC
#define Motor_Right_BIN2_Pin GPIO_PIN_3
#define Motor_Right_BIN2_GPIO_Port GPIOC
#define Motor_PWM_Pin GPIO_PIN_6
#define Motor_PWM_GPIO_Port GPIOA
#define Oled_MOSI_Pin GPIO_PIN_7
#define Oled_MOSI_GPIO_Port GPIOA
#define LeftFront_Pin GPIO_PIN_0
#define LeftFront_GPIO_Port GPIOB
#define LeftFront_EXTI_IRQn EXTI0_IRQn
#define LeftReap_Pin GPIO_PIN_1
#define LeftReap_GPIO_Port GPIOB
#define LeftReap_EXTI_IRQn EXTI1_IRQn
#define RightFront_Pin GPIO_PIN_2
#define RightFront_GPIO_Port GPIOB
#define RightFront_EXTI_IRQn EXTI2_IRQn
#define RightReap_Pin GPIO_PIN_10
#define RightReap_GPIO_Port GPIOB
#define RightReap_EXTI_IRQn EXTI15_10_IRQn
#define OLED_CS_Pin GPIO_PIN_12
#define OLED_CS_GPIO_Port GPIOB
#define OLED_SPI2_SCK_Pin GPIO_PIN_13
#define OLED_SPI2_SCK_GPIO_Port GPIOB
#define OLED_RST_Pin GPIO_PIN_14
#define OLED_RST_GPIO_Port GPIOB
#define OLED_SPI2_MOSI_Pin GPIO_PIN_15
#define OLED_SPI2_MOSI_GPIO_Port GPIOB
#define OLED_DC_Pin GPIO_PIN_7
#define OLED_DC_GPIO_Port GPIOC
#define Vision_USART_RX_Pin GPIO_PIN_10
#define Vision_USART_RX_GPIO_Port GPIOA
#define VIsion_USART_TX_Pin GPIO_PIN_15
#define VIsion_USART_TX_GPIO_Port GPIOA
#define Oled_SCK_Pin GPIO_PIN_3
#define Oled_SCK_GPIO_Port GPIOB
#define Oled_MISO_Pin GPIO_PIN_4
#define Oled_MISO_GPIO_Port GPIOB
#define ToF_SCL_Pin GPIO_PIN_6
#define ToF_SCL_GPIO_Port GPIOB
#define ToF_SDA_Pin GPIO_PIN_7
#define ToF_SDA_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
