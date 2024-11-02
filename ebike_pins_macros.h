
#ifndef _PINS_H_
#define _PINS_H_

#define HALLA__PORT       GPIOC
#define HALLA__PIN        GPIO_PIN_6
#define HALLB__PORT       GPIOC
#define HALLB__PIN        GPIO_PIN_7
#define HALLC__PORT       GPIOC
#define HALLC__PIN        GPIO_PIN_8

#define PMWA_LOW__PORT     GPIOA
#define PMWA_LOW__PIN      GPIO_PIN_7
#define PMWB_LOW__PORT     GPIOB
#define PMWB_LOW__PIN      GPIO_PIN_0
#define PMWC_LOW__PORT     GPIOB
#define PMWC_LOW__PIN      GPIO_PIN_1

#define PMWA_HIGH__PORT    GPIOA
#define PMWA_HIGH__PIN     GPIO_PIN_8
#define PMWB_HIGH__PORT    GPIOA
#define PMWB_HIGH__PIN     GPIO_PIN_9
#define PMWC_HIGH__PORT    GPIOA
#define PMWC_HIGH__PIN     GPIO_PIN_10

#define LCD_UART_RX__PORT     GPIOB
#define LCD_UART_RX__PIN      GPIO_PIN_7
#define LCD_UART_TX__PORT     GPIOB
#define LCD_UART_TX__PIN      GPIO_PIN_6

#define BRAKE_SENSOR__PORT    GPIOB
#define BRAKE_SENSOR__PIN     GPIO_PIN_12

#define PAS1_PEDAL__PORT      GPIOB
#define PAS1_PEDAL__PIN       GPIO_PIN_4
#define PAS2_DIR__PORT        GPIOB
#define PAS2_DIR__PIN         GPIO_PIN_5

#define WHEELSPEED_SENSOR__PORT  GPIOB
#define WHEELSPEED_SENSOR__PIN   GPIO_PIN_10

#define TORQUE_SENSOR_RX__PORT     GPIOB
#define TORQUE_SENSOR_RX__PIN      GPIO_PIN_8

#define TORQUE_SENSOR_TX__PORT     GPIOB
#define TORQUE_SENSOR_TX__PIN      GPIO_PIN_9

#define BACK_LIGHT__PORT          GPIOB
#define BACK_LIGHT__PIN           GPIO_PIN_13

#define FRONT_LIGHT__PORT         GPIOB
#define FRONT_LIGHT__PIN          GPIO_PIN_14

#define BUZZER__PORT              GPIOA
#define BUZZER__PIN               GPIO_PIN_1

#define THROTTLE__PORT            GPIOA
#define THROTTLE__PIN             GPIO_PIN_0

#define VBAT_SENSE__PORT    GPIOC
#define VBAT_SENSE__PIN     GPIO_PIN_4

#define IBAT_SENSE__PORT     GPIOC
#define IBAT_SENSE__PIN      GPIO_PIN_5

#define MOTOR_TEMP_SENSOR__PORT    GPIOC
#define MOTOR_TEMP_SENSOR__PIN     GPIO_PIN_0

#define VA_SENSE__PORT    GPIOC
#define VA_SENSE__PIN     GPIO_PIN_1
#define VB_SENSE__PORT    GPIOC
#define VB_SENSE__PIN     GPIO_PIN_2
#define VC_SENSE__PORT    GPIOC
#define VC_SENSE__PIN     GPIO_PIN_3

#define IA_SENSE__PORT    GPIOA
#define IA_SENSE__PIN     GPIO_PIN_4
#define IB_SENSE__PORT    GPIOA
#define IB_SENSE__PIN     GPIO_PIN_5
#define IC_SENSE__PORT    GPIOA
#define IC_SENSE__PIN     GPIO_PIN_6

#define JONGO_LED__PORT    GPIOA
#define JONGO_LED__PIN     GPIO_PIN_6

#endif /* _PINS_H_ */
