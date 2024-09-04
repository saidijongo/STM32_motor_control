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

/* USER CODE END Includes */

/* USER CODE BEGIN 0 */
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc) {
    if (hadc->Instance == ADC1) {
        // Read ADC values for phase currents
        adcValuePhaseA = HAL_ADC_GetValue(&hadc1);
        adcValuePhaseB = HAL_ADC_GetValue(&hadc2);  // Assuming ADC2 is used for Phase B

        // Convert ADC values to actual current values (in amperes)
        float I_a = ((float)adcValuePhaseA - ADC_OFFSET) * CURRENT_SCALING;
        float I_b = ((float)adcValuePhaseB - ADC_OFFSET) * CURRENT_SCALING;

        // Step 1: Clarke Transformation (get I_alpha, I_beta)
        I_alpha = I_a;
        I_beta = (I_a + 2 * I_b) / sqrtf(3.0f);

        // Step 2: Park Transformation (get I_d, I_q)
        I_d = I_alpha * cosf(theta_e) + I_beta * sinf(theta_e);
        I_q = -I_alpha * sinf(theta_e) + I_beta * cosf(theta_e);

        // Step 3: PI Control on I_d and I_q (regulate flux and torque)
        PI_Id_out = PI_Controller(0.0f, I_d);  // Set Id reference to 0 for BLDC motor
        PI_Iq_out = PI_Controller(V_q_ref, I_q);  // Control torque with Iq reference

        // Step 4: Inverse Park Transformation
        V_alpha = PI_Id_out * cosf(theta_e) - PI_Iq_out * sinf(theta_e);
        V_beta = PI_Id_out * sinf(theta_e) + PI_Iq_out * cosf(theta_e);

        // Step 5: Inverse Clarke Transformation
        Va = V_alpha;
        Vb = -0.5f * V_alpha + (sqrtf(3.0f) / 2.0f) * V_beta;
        Vc = -0.5f * V_alpha - (sqrtf(3.0f) / 2.0f) * V_beta;

        // Apply PWM to the motor phases
        __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, Va);
        __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_2, Vb);
        __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_3, Vc);
    }
}

/* USER CODE: Simple PI Controller */
float PI_Controller(float reference, float measured) {
    static float integral = 0.0f;
    float error = reference - measured;
    float Kp = 1.0f;  // Proportional gain
    float Ki = 0.1f;  // Integral gain

    integral += Ki * error;

    return Kp * error + integral;
}

/* USER CODE: Get electrical angle from Hall sensors or encoder */
float GetElectricalAngle(void) {
    uint16_t hallState = (GPIO_ReadInputData(GPIOB) & 0x7);  // Assume 3 Hall sensor bits on GPIOB

    // Lookup table for electrical angles based on hall state
    switch (hallState) {
        case 0b001: return 0.0f;
        case 0b010: return M_PI / 3.0f;
        case 0b100: return 2 * M_PI / 3.0f;
        case 0b101: return M_PI;
        case 0b110: return 4 * M_PI / 3.0f;
        case 0b011: return 5 * M_PI / 3.0f;
        default: return 0.0f;  // Invalid hall state, return 0
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

    /* USER CODE BEGIN 2 */
    // Start PWM Timer
    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_2);
    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_3);

    // Start ADC for current sensing
    HAL_ADC_Start_IT(&hadc1);
    HAL_ADC_Start_IT(&hadc2);

    /* USER CODE END 2 */

    /* Infinite loop */
    while (1) {
        // Get rotor electrical angle
        theta_e = GetElectricalAngle();

        // Optionally, implement speed control loop
        HAL_Delay(1);
    }
}

/**
  * @brief  System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void) {
    // Code generated by STM32CubeMX for clock configuration
}

/* USER CODE END 6 */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
