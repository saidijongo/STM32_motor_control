/*
 * GaiasPins.h
 *
 *  Created on: Nov 7, 2024
 *      Author: saidi
 */

#ifndef INC_GAIASPINS_H_
#define INC_GAIASPINS_H_

#define HALLA__Port       GPIOC
#define HALLA__Pin        GPIO_PIN_6
#define HALLB__Port       GPIOC
#define HALLB__Pin        GPIO_PIN_7
#define HALLC__Port       GPIOA
#define HALLC__Pin        GPIO_PIN_13

#define PMWA_LOW__Port     GPIOB
#define PMWA_LOW__Pin      GPIO_PIN_13
#define PMWB_LOW__Port     GPIOB
#define PMWB_LOW__Pin      GPIO_PIN_14
#define PMWC_LOW__Port     GPIOB
#define PMWC_LOW__Pin      GPIO_PIN_15

#define PMWA_HIGH__Port    GPIOA
#define PMWA_HIGH__Pin     GPIO_PIN_8
#define PMWB_HIGH__Port    GPIOA
#define PMWB_HIGH__Pin     GPIO_PIN_9
#define PMWC_HIGH__Port    GPIOA
#define PMWC_HIGH__Pin     GPIO_PIN_10

#define LCD_UART_RX__Port     GPIOB
#define LCD_UART_RX__Pin      GPIO_PIN_8
#define LCD_UART_TX__Port     GPIOB
#define LCD_UART_TX__Pin      GPIO_PIN_9

#define LEFT_BRAKE_SENSOR__Port    GPIOC
#define LEFT_BRAKE_SENSOR__Pin     GPIO_PIN_14

#define RIGHT_BRAKE_SENSOR__Port    GPIOC
#define RIGHT_BRAKE_SENSOR__Pin     GPIO_PIN_15

#define PAS1_PEDAL__Port      GPIOA
#define PAS1_PEDAL__Pin       GPIO_PIN_7
#define PAS2_DIR__Port        GPIOB
#define PAS2_DIR__Pin         GPIO_PIN_0

#define WHEELSPEED_SENSOR__Port  GPIOA
#define WHEELSPEED_SENSOR__Pin   GPIO_PIN_14

//#define TORQUE_SENSOR_RX__Port     GPIOB
//#define TORQUE_SENSOR_RX__Pin      GPIO_PIN_8

#define TORQUE_SENSOR__Port     GPIOB
#define TORQUE_SENSOR__Pin      GPIO_PIN_2

#define BACK_LIGHT__Port          GPIOG
#define BACK_LIGHT__Pin           GPIO_PIN_10

#define FRONT_LIGHT__Port         GPIOB
#define FRONT_LIGHT__Pin          GPIO_PIN_10

#define BUZZER__Port       GPIOA
#define BUZZER__Pin        GPIO_PIN_3

#define THROTTLE__Port     GPIOA
#define THROTTLE__Pin      GPIO_PIN_4

#define VBATT_SENSE__Port    GPIOF
#define VBATT_SENSE__Pin     GPIO_PIN_1

//#define SEAT_FSR400_SENSOR__Port     GPIOA
//#define ISEAT_FSR400_SENSOR__Pin      GPIO_PIN_4

#define MOTOR_TEMP_SENSOR__Port    GPIOA
#define MOTOR_TEMP_SENSOR__Pin     GPIO_PIN_5

#define VA_SENSE__Port    GPIOA
#define VA_SENSE__Pin     GPIO_PIN_0
#define VB_SENSE__Port    GPIOA
#define VB_SENSE__Pin     GPIO_PIN_1
#define VC_SENSE__Port    GPIOA
#define VC_SENSE__Pin     GPIO_PIN_2

#define IA_SENSE__Port    GPIOB
#define IA_SENSE__Pin     GPIO_PIN_12
#define IB_SENSE__Port    GPIOB
#define IB_SENSE__Pin     GPIO_PIN_11
#define IC_SENSE__Port     GPIOA
#define IC_SENSE__Pin      GPIO_PIN_6


#define MOTOR_LED__Port    GPIOC
#define MOTOR_LED__Pin     GPIO_PIN_9

#define JONGO_LED__Port    GPIOB
#define JONGO_LED__Pin     GPIO_PIN_1

#define DRV_SP3MOSI__Port    GPIOB
#define DRV_SP3MOSI__Pin     GPIO_PIN_5
#define DRV_SP3MISO__Port    GPIOB
#define DRV_SP3MISO__Pin     GPIO_PIN_4
#define DRV_SP3SCK__Port    GPIOB
#define DRV_SP3SCK__Pin     GPIO_PIN_3

#define DRV_SP3CS__Port    GPIOA
#define DRV_SP3CS__Pin     GPIO_PIN_15

#define DRV_SCLK3V3__Port    GPIOD
#define DRV_SCLK3V3__Pin     GPIO_PIN_2
#define DRV_SDO3V3__Port    GPIOH
#define DRV_SDO3V3__Pin     GPIO_PIN_0
#define DRV_SDI3V3__Port    GPIOH
#define DRV_SDI3V3__Pin     GPIO_PIN_1
#define DRV_MODE__Port    GPIOC
#define DRV_MODE__Pin     GPIO_PIN_13

#endif /* INC_GAIASPINS_H_ */
