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

/* USER CODE END Includes */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_TIM1_Init(void);
static void MX_TIM4_Init(void);
static void MX_ADC1_Init(void);
void PrepareMotorCommutation(uint16_t hallPos);
void UpdatePWMDutyCycle(uint16_t dutyCycle);

/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
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

/* USER CODE: Configure Motor PWM Timer */
void PrepareMotorCommutation(uint16_t hallPos) {
    // Update PWM values based on the Hall sensor position
    // Example: Update CCR1, CCR2, CCR3 values for PWM
    switch (hallPos) {
        case 1:
            // Set PWM for step 1
            __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, motorSpeed);
            __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_2, 0);
            __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_3, 0);
            break;
        case 2:
            __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, 0);
            __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_2, motorSpeed);
            __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_3, 0);
            break;
        case 3:
            __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, 0);
            __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_2, 0);
            __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_3, motorSpeed);
            break;
        case 4:
            __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, motorSpeed);
            __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_2, 0);
            __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_3, 0);
            break;
        case 5:
            __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, 0);
            __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_2, motorSpeed);
            __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_3, 0);
            break;
        case 6:
            __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, 0);
            __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_2, 0);
            __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_3, motorSpeed);
            break;
    }
}

/* USER CODE: Update PWM Duty Cycle */
void UpdatePWMDutyCycle(uint16_t dutyCycle) {
    // Adjust the PWM duty cycle for all channels based on ADC value
    __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, dutyCycle);
    __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_2, dutyCycle);
    __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_3, dutyCycle);
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

    /* USER CODE END 2 */

    /* Infinite loop */
    /* USER CODE BEGIN WHILE */
    while (1) {
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

