/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "tim.h"
#include "gpio.h"
#include "adc.h"

/* USER CODE BEGIN Includes */
uint16_t hallPos = 0;
uint16_t lastHallPos = 0;
uint16_t motorSpeed = 0;  // Speed controlled by potentiometer
uint16_t commutationCount = 0;
uint16_t adcValue = 0;    // ADC value for the potentiometer
uint16_t pasPulses = 0;   // PAS sensor pulses
uint8_t pasLevel = 1;     // Default PAS level
uint16_t cadence = 0;     // Calculated cadence (RPM)
uint16_t powerPercent = 20; // Power percentage (20% for PAS level 1)

/* USER CODE END Includes */

/* USER CODE BEGIN 0 */

/* USER CODE: Configure Hall sensor timer */
void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim) {
    if (htim->Instance == TIM4) {
        // Read Hall sensor data
        uint16_t hallNewPos = (GPIO_ReadInputData(GPIOD) & 0x7000) >> 12;
        
        if (hallNewPos != hallPos) {
            lastHallPos = hallPos;
            hallPos = hallNewPos;
            PrepareMotorCommutation(hallPos);
        }
    }
}

/* USER CODE: PAS Sensor (12 magnets) */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
    if (GPIO_Pin == PAS_SENSOR_PIN) {
        pasPulses++;  // Increment PAS pulse count
    }
}

/* USER CODE: Motor Commutation based on Hall sensor */
void PrepareMotorCommutation(uint16_t hallPos) {
    uint16_t motorPower = (motorSpeed * powerPercent) / 100;  // Adjust power based on PAS level

    // Update PWM values based on Hall sensor position
    switch (hallPos) {
        case 1:
            // Phase 1 active
            __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, motorPower);
            __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_2, 0);
            __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_3, 0);
            break;
        case 2:
            // Phase 2 active
            __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, 0);
            __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_2, motorPower);
            __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_3, 0);
            break;
        case 3:
            // Phase 3 active
            __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, 0);
            __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_2, 0);
            __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_3, motorPower);
            break;
        // Add other phases for full 6-step commutation
    }
}

/* USER CODE: ADC Conversion Complete Callback */
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc) {
    if (hadc->Instance == ADC1) {
        // Get the potentiometer value
        adcValue = HAL_ADC_GetValue(hadc);

        // Map ADC value to a motor speed PWM duty cycle range (0 to max PWM period)
        motorSpeed = (adcValue * TIM1->ARR) / 4096;
    }
}

/* USER CODE: Calculate Cadence and Adjust Power Based on PAS Level */
void UpdatePASLevel(void) {
    // Calculate cadence (RPM) based on PAS pulses over a fixed interval (e.g., 1 second)
    cadence = (pasPulses * 60) / 12;  // 12 pulses per revolution

    // Adjust motor power based on PAS level and cadence
    if (cadence > 0) {
        switch (pasLevel) {
            case 1:
                powerPercent = 20;  // 20% power
                break;
            case 2:
                powerPercent = 40;  // 40% power
                break;
            case 3:
                powerPercent = 60;  // 60% power
                break;
            case 4:
                powerPercent = 80;  // 80% power
                break;
            case 5:
                powerPercent = 100;  // 100% power
                break;
        }
    } else {
        powerPercent = 0;  // No power if no cadence
    }

    // Reset PAS pulse count for the next interval
    pasPulses = 0;
}

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void) {
    /* USER CODE BEGIN 1 */

    /* USER CODE END 1 */

    /* MCU Configuration--------------------------------------------------------*/

    /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
    HAL_Init();

    /* Configure the system clock */
    SystemClock_Config();

    /* Initialize all configured peripherals */
    MX_GPIO_Init();
    MX_TIM1_Init();
    MX_TIM4_Init();
    MX_ADC1_Init();

    /* USER CODE BEGIN 2 */
    // Start PWM Timer
    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_2);
    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_3);

    // Start Hall Sensor Timer
    HAL_TIM_IC_Start_IT(&htim4, TIM_CHANNEL_1);

    // Start ADC for potentiometer
    HAL_ADC_Start_IT(&hadc1);

    // Start PAS sensor (configure GPIO interrupt or timer input capture)
    HAL_GPIO_EXTI_IRQHandler(PAS_SENSOR_PIN);  // Set up EXTI for PAS sensor

    /* USER CODE END 2 */

    /* Infinite loop */
    /* USER CODE BEGIN WHILE */
    while (1) {
        // Update PAS level based on pedal cadence
        UpdatePASLevel();

        // Continuously update the motor speed based on potentiometer input
        HAL_ADC_Start_IT(&hadc1);  // Restart ADC conversion
        HAL_Delay(100);            // Adjust delay for smoother speed control
    }
    /* USER CODE END WHILE */
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void) {
    // Code to set up system clock (generated by CubeMX)
}

/* USER CODE BEGIN 6 */

/* USER CODE END 6 */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
