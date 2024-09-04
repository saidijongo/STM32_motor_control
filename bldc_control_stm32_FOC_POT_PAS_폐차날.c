/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "tim.h"
#include "gpio.h"
#include "adc.h"
#include "math.h"

/* USER CODE BEGIN Includes */
float I_alpha, I_beta;        // Clarke transformation
float I_d, I_q;               // Park transformation
float V_d_ref = 0.0f;         // Reference flux current
float V_q_ref = 0.0f;         // Reference torque current
float V_alpha, V_beta;        // Inverse Park transformation
float Va, Vb, Vc;             // Three-phase voltages after inverse Clarke
float theta_e;                // Electrical angle from Hall sensors
float PI_Id_out, PI_Iq_out;   // PI controller outputs for Id and Iq

uint16_t adcValuePhaseA, adcValuePhaseB;  // ADC values for current sensing
uint16_t throttleValue;                   // Throttle input (potentiometer)
uint16_t pasPulses = 0;                   // PAS sensor pulses
uint16_t cadence = 0;                     // Pedal cadence (RPM)
uint8_t pasLevel = 1;                     // Default PAS level (1 to 5)
uint16_t powerPercent = 20;               // Motor power percentage (20% to 100%)

float Kp_d = 1.0f, Ki_d = 0.1f;  // PI gains for d-axis (flux control)
float Kp_q = 1.0f, Ki_q = 0.1f;  // PI gains for q-axis (torque control)
float integral_d = 0.0f, integral_q = 0.0f;  // Integral terms for PI controllers

#define SHUNT_RESISTOR_VALUE 0.001  // Shunt resistor value (1 mOhm)
#define CURRENT_SCALING 100  // Current scaling factor (depends on ADC and shunt setup)
#define ADC_OFFSET 2048  // ADC offset for center measurement

/* USER CODE END Includes */

/* USER CODE BEGIN 0 */
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc) {
    if (hadc->Instance == ADC1) {
        // Read ADC values for current sensing (phases A and B)
        adcValuePhaseA = HAL_ADC_GetValue(&hadc1);
        adcValuePhaseB = HAL_ADC_GetValue(&hadc2);  // Assuming ADC2 is used for Phase B

        // Convert ADC values to actual current (amps) using the shunt resistor
        float I_a = ((float)adcValuePhaseA - ADC_OFFSET) * CURRENT_SCALING;
        float I_b = ((float)adcValuePhaseB - ADC_OFFSET) * CURRENT_SCALING;

        // Step 1: Clarke Transformation (get I_alpha, I_beta)
        I_alpha = I_a;
        I_beta = (I_a + 2 * I_b) / sqrtf(3.0f);

        // Step 2: Park Transformation (get I_d, I_q)
        I_d = I_alpha * cosf(theta_e) + I_beta * sinf(theta_e);
        I_q = -I_alpha * sinf(theta_e) + I_beta * cosf(theta_e);

        // Step 3: PI Control for I_d and I_q
        PI_Id_out = PI_Controller(0.0f, I_d, &integral_d, Kp_d, Ki_d);  // Control Id (flux)
        PI_Iq_out = PI_Controller(V_q_ref, I_q, &integral_q, Kp_q, Ki_q);  // Control Iq (torque)

        // Step 4: Inverse Park Transformation
        V_alpha = PI_Id_out * cosf(theta_e) - PI_Iq_out * sinf(theta_e);
        V_beta = PI_Id_out * sinf(theta_e) + PI_Iq_out * cosf(theta_e);

        // Step 5: Inverse Clarke Transformation
        Va = V_alpha;
        Vb = -0.5f * V_alpha + (sqrtf(3.0f) / 2.0f) * V_beta;
        Vc = -0.5f * V_alpha - (sqrtf(3.0f) / 2.0f) * V_beta;

        // Apply PWM to motor phases
        __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, Va);
        __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_2, Vb);
        __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_3, Vc);
    }
}

/* USER CODE: PI Controller for Iq and Id */
float PI_Controller(float reference, float measured, float *integral, float Kp, float Ki) {
    float error = reference - measured;
    *integral += Ki * error;  // Integrate the error
    return Kp * error + *integral;  // Return the PI controller output
}

/* USER CODE: PAS Sensor (12 magnets) */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
    if (GPIO_Pin == PAS_SENSOR_PIN) {
        pasPulses++;  // Increment PAS pulse count
    }
}

/* USER CODE: Update motor power based on PAS level */
void UpdatePASLevel(void) {
    // Calculate cadence (RPM) based on PAS pulses over a fixed time interval
    cadence = (pasPulses * 60) / 12;  // 12 pulses per revolution
    pasPulses = 0;  // Reset pulse count for next interval

    // Adjust power distribution based on PAS level
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
        default:
            powerPercent = 0;  // No power
    }

    // Adjust V_q_ref based on PAS level and throttle
    V_q_ref = ((float)throttleValue / 4096.0f) * powerPercent;
}

/* USER CODE: Get throttle input (potentiometer) */
void UpdateThrottle(void) {
    throttleValue = HAL_ADC_GetValue(&hadc3);  // Assuming throttle is connected to ADC3
}

/* USER CODE: Get electrical angle from Hall sensors or encoder */
float GetElectricalAngle(void) {
    uint16_t hallState = (GPIO_ReadInputData(GPIOB) & 0x7);  // Assume Hall sensors are on GPIOB

    // Lookup table for electrical angles based on Hall state
    switch (hallState) {
        case 0b001: return 0.0f;
        case 0b010: return M_PI / 3.0f;
        case 0b100: return 2 * M_PI / 3.0f;
        case 0b101: return M_PI;
        case 0b110: return 4 * M_PI / 3.0f;
        case 0b011: return 5 * M_PI / 3.0f;
        default: return 0.0f;  // Invalid Hall state
    }
}

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void) {
    /* MCU Configuration--------------------------------------------------------*/
    HAL_Init();
    SystemClock_Config();

    /* Initialize all configured peripherals */
    MX_GPIO_Init();
    MX_TIM1_Init();
    MX_ADC1_Init();
    MX_ADC2_Init();  // For phase B current sensing
    MX_ADC3_Init();  // For throttle potentiometer input

    /* USER CODE BEGIN 2 */
    // Start PWM Timer
    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_2);
    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_3);

    // Start ADC for current sensing (phases A and B)
    HAL_ADC_Start_IT(&hadc1);
    HAL_ADC_Start_IT(&hadc2);

    // Start ADC for throttle input
    HAL_ADC_Start_IT(&hadc3);

    // Start PAS sensor (configure GPIO interrupt)
    HAL_GPIO_EXTI_IRQHandler(PAS_SENSOR_PIN);

    /* USER CODE END 2 */

    /* Infinite loop */
    while (1) {
        // Get rotor electrical angle
        theta_e = GetElectricalAngle();

        // Update throttle and PAS level
        UpdateThrottle();
        UpdatePASLevel();

        HAL_Delay(10);  // Delay for smoother control (adjust if needed)
    }
}

/**
  * @brief  System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void) {
    // Code generated by STM32CubeMX for clock configuration
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
