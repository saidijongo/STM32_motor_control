#include "stm32g4xx_hal.h"
#include "drv8353.h"
#include "gpio.h"
#include "tim.h"
#include "hall_sensor.h"

// Define the hall sensor GPIO pins
#define HALL_SENSOR_PIN_A GPIO_PIN_0
#define HALL_SENSOR_PIN_B GPIO_PIN_1
#define HALL_SENSOR_PIN_C GPIO_PIN_2
#define HALL_SENSOR_PORT GPIOA

// Function declarations
void SystemClock_Config(void);
void Error_Handler(void);
void Motor_Init(void);
void Motor_Start(void);
void Motor_Stop(void);
void HallSensor_Init(void);
void PWM_SetDutyCycle(uint32_t dutyCycle);
void HallSensor_Callback(uint8_t hallState);
uint32_t CalculateDutyCycle(uint32_t throttleValue);
uint8_t ReadHallSensors(void);

int main(void) {
    HAL_Init();
    SystemClock_Config();

    Motor_Init();
    HallSensor_Init();

    // Start the motor
    Motor_Start();

    // Infinite loop
    while (1) {
        // Main control loop can go here
        uint32_t throttleValue = HAL_ADC_GetValue(&hadc1); // Assuming ADC is used for throttle input
        uint32_t dutyCycle = CalculateDutyCycle(throttleValue);
        PWM_SetDutyCycle(dutyCycle);
    }
}

void SystemClock_Config(void) {
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
    RCC_OscInitStruct.HSIState = RCC_HSI_ON;
    RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
    RCC_OscInitStruct.PLL.PLLM = 1;
    RCC_OscInitStruct.PLL.PLLN = 10;
    RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV7;
    RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
    RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
        Error_Handler();
    }

    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK
                                  | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK) {
        Error_Handler();
    }
}

void Error_Handler(void) {
    while (1) {
        // Stay here
    }
}

void Motor_Init(void) {
    MX_GPIO_Init();
    MX_TIM1_Init();
    DRV8353_Init();
}

void HallSensor_Init(void) {
    MX_GPIO_Init();

    // Configure Hall Sensor GPIO pins as input
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = HALL_SENSOR_PIN_A | HALL_SENSOR_PIN_B | HALL_SENSOR_PIN_C;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(HALL_SENSOR_PORT, &GPIO_InitStruct);

    // Configure EXTI interrupts for hall sensors
    HAL_NVIC_SetPriority(EXTI0_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(EXTI0_IRQn);
    HAL_NVIC_SetPriority(EXTI1_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(EXTI1_IRQn);
    HAL_NVIC_SetPriority(EXTI2_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(EXTI2_IRQn);
}

void Motor_Start(void) {
    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_2);
    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_3);
}

void Motor_Stop(void) {
    HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_1);
    HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_2);
    HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_3);
}

void PWM_SetDutyCycle(uint32_t dutyCycle) {
    __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, dutyCycle);
    __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_2, dutyCycle);
    __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_3, dutyCycle);
}

uint32_t CalculateDutyCycle(uint32_t throttleValue) {
    // Convert throttle value (0-4095) to duty cycle (0-1000)
    return (throttleValue * 1000) / 4095;
}

uint8_t ReadHallSensors(void) {
    uint8_t hallState = 0;
    hallState |= HAL_GPIO_ReadPin(HALL_SENSOR_PORT, HALL_SENSOR_PIN_A) ? 0x01 : 0x00;
    hallState |= HAL_GPIO_ReadPin(HALL_SENSOR_PORT, HALL_SENSOR_PIN_B) ? 0x02 : 0x00;
    hallState |= HAL_GPIO_ReadPin(HALL_SENSOR_PORT, HALL_SENSOR_PIN_C) ? 0x04 : 0x00;
    return hallState;
}

void HallSensor_Callback(uint8_t hallState) {
    // Update motor control based on hall sensor state
    switch (hallState) {
        case 0x01:
            // Commutation step 1
            HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_SET);
            HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, GPIO_PIN_RESET);
            HAL_GPIO_WritePin(GPIOB, GPIO_PIN_2, GPIO_PIN_SET);
            break;
        case 0x02:
            // Commutation step 2
            HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_SET);
            HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, GPIO_PIN_SET);
            HAL_GPIO_WritePin(GPIOB, GPIO_PIN_2, GPIO_PIN_RESET);
            break;
        case 0x04:
            // Commutation step 3
            HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
            HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, GPIO_PIN_SET);
            HAL_GPIO_WritePin(GPIOB, GPIO_PIN_2, GPIO_PIN_SET);
            break;
        case 0x03:
            // Commutation step 4
            HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_SET);
            HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, GPIO_PIN_RESET);
            HAL_GPIO_WritePin(GPIOB, GPIO_PIN_2, GPIO_PIN_RESET);
            break;
        case 0x06:
            // Commutation step 5
            HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
            HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, GPIO_PIN_RESET);
            HAL_GPIO_WritePin(GPIOB, GPIO_PIN_2, GPIO_PIN_SET);
            break;
        case 0x05:
            // Commutation step 6
            HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
            HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, GPIO_PIN_SET);
            HAL_GPIO_WritePin(GPIOB, GPIO_PIN_2, GPIO_PIN_RESET);
            break;
        default:
            // Invalid state
            Motor_Stop();
            break;
    }
}

// EXTI interrupt handlers for hall sensors
void EXTI0_IRQHandler(void) {
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_0);
    HallSensor_Callback(ReadHallSensors());
}

void EXTI1_IRQHandler(void) {
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_1);
    HallSensor_Callback(ReadHallSensors());
}

void EXTI2_IRQHandler(void) {
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_2);
    HallSensor_Callback(ReadHallSensors());
}
