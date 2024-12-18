#include "main.h"
#include "math.h"

// ADC handle declaration
ADC_HandleTypeDef hadc1;
ADC_HandleTypeDef hadc2;

// Function prototypes
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_ADC1_Init(void);
static void MX_ADC2_Init(void);

// Define constants
#define VREF 3.3        // Reference voltage for ADC
#define GAIN 40.0       // Gain of DRV8353 amplifier
#define R_SHUNT 0.001   // Shunt resistor value in ohms
#define NON_LINEAR_REGION 0.25  // Non-linear region from datasheet

// Function to calculate current for one phase
float calculate_current(uint32_t adc_value) {
    // Convert ADC value to voltage
    float adc_voltage = (adc_value * VREF) / 4095.0;  // 12-bit ADC

    // Adjust for non-linear region
    float effective_voltage = adc_voltage - NON_LINEAR_REGION;

    // Calculate current using the gain and shunt resistance
    float current = (effective_voltage) / (GAIN * R_SHUNT);

    return current;
}

int main(void) {
    // Initialize the system
    HAL_Init();
    SystemClock_Config();
    MX_GPIO_Init();
    MX_ADC1_Init();
    MX_ADC2_Init();

    // Start ADCs
    HAL_ADC_Start(&hadc1);
    HAL_ADC_Start(&hadc2);

    while (1) {
        // Poll for ADC values
        HAL_ADC_PollForConversion(&hadc1, HAL_MAX_DELAY);
        uint32_t adc_value_A = HAL_ADC_GetValue(&hadc1);  // Get the ADC value for Phase A

        HAL_ADC_PollForConversion(&hadc2, HAL_MAX_DELAY);
        uint32_t adc_value_B = HAL_ADC_GetValue(&hadc2);  // Get the ADC value for Phase B

        // Calculate the currents for Phase A and Phase B
        float current_A = calculate_current(adc_value_A);
        float current_B = calculate_current(adc_value_B);

        // Calculate the current for Phase C
        float current_C = -(current_A + current_B);

        HAL_Delay(100);  // Sample delay (adjust based on application)
    }
}

// ADC1 initialization function (for Phase A current sensing)
static void MX_ADC1_Init(void) {
    ADC_ChannelConfTypeDef sConfig = {0};

    // Configure the ADC1 peripheral
    hadc1.Instance = ADC1;
    hadc1.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV4;
    hadc1.Init.Resolution = ADC_RESOLUTION_12B;
    hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
    hadc1.Init.ScanConvMode = ADC_SCAN_DISABLE;
    hadc1.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
    hadc1.Init.ContinuousConvMode = DISABLE;
    hadc1.Init.NbrOfConversion = 1;
    HAL_ADC_Init(&hadc1);

    // Configure ADC1 channel for Phase A current sensing
    sConfig.Channel = ADC_CHANNEL_1;  // Select the appropriate channel for Phase A current sensing
    sConfig.Rank = 1;
    sConfig.SamplingTime = ADC_SAMPLETIME_3CYCLES;
    HAL_ADC_ConfigChannel(&hadc1, &sConfig);
}

// ADC2 initialization function (for Phase B current sensing)
static void MX_ADC2_Init(void) {
    ADC_ChannelConfTypeDef sConfig = {0};

    // Configure the ADC2 peripheral
    hadc2.Instance = ADC2;
    hadc2.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV4;
    hadc2.Init.Resolution = ADC_RESOLUTION_12B;
    hadc2.Init.DataAlign = ADC_DATAALIGN_RIGHT;
    hadc2.Init.ScanConvMode = ADC_SCAN_DISABLE;
    hadc2.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
    hadc2.Init.ContinuousConvMode = DISABLE;
    hadc2.Init.NbrOfConversion = 1;
    HAL_ADC_Init(&hadc2);

    // Configure ADC2 channel for Phase B current sensing
    sConfig.Channel = ADC_CHANNEL_2;  // Select the appropriate channel for Phase B current sensing
    sConfig.Rank = 1;
    sConfig.SamplingTime = ADC_SAMPLETIME_3CYCLES;
    HAL_ADC_ConfigChannel(&hadc2, &sConfig);
}

// System clock configuration
void SystemClock_Config(void) {
    // System clock configuration here (auto-generated by STM32CubeMX)
}

// GPIO initialization
static void MX_GPIO_Init(void) {
    // GPIO initialization here (auto-generated by STM32CubeMX)
}
