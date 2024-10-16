#include "main.h"
#include "math.h"

// ADC handle declarations
ADC_HandleTypeDef hadc1, hadc2, hadc3, hadc4;

// Function prototypes
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_ADC1_Init(void);
static void MX_ADC2_Init(void);
static void MX_ADC3_Init(void);
static void MX_ADC4_Init(void);

// Constants for voltage sensing (shared resistors for all phases)
#define VREF 3.3  // Reference voltage for ADC (STM32 max input voltage is 3.3V)
#define R_HIGH 96700.0  // High-side resistor (same for all phases)
#define R_LOW 7.0       // Low-side resistor (same for all phases)

// Constants for current sensing
#define GAIN 40.0  // Gain of DRV8353 amplifier
#define R_SHUNT 0.001  // Shunt resistor value in ohms
#define NON_LINEAR_REGION 0.25  // Non-linear region voltage from datasheet

// Function to calculate current for a given ADC value
float calculate_current(uint32_t adc_value) {
    float adc_voltage = (adc_value * VREF) / 4095.0;  // Convert ADC value to voltage
    float effective_voltage = adc_voltage - NON_LINEAR_REGION;
    return effective_voltage / (GAIN * R_SHUNT);  // Calculate current using gain and shunt resistor
}

// Function to calculate voltage using a voltage divider
float calculate_voltage(uint32_t adc_value) {
    float adc_voltage = (adc_value * VREF) / 4095.0;  // Convert ADC value to voltage
    return adc_voltage * (R_HIGH + R_LOW) / R_LOW;    // Reverse voltage divider formula
}

// Function to measure Phase A current
float measure_phase_current_A(void) {
    HAL_ADC_PollForConversion(&hadc1, HAL_MAX_DELAY);
    uint32_t adc_value = HAL_ADC_GetValue(&hadc1);  // Get ADC value for Phase A
    return calculate_current(adc_value);
}

// Function to measure Phase B current
float measure_phase_current_B(void) {
    HAL_ADC_PollForConversion(&hadc2, HAL_MAX_DELAY);
    uint32_t adc_value = HAL_ADC_GetValue(&hadc2);  // Get ADC value for Phase B
    return calculate_current(adc_value);
}

// Function to measure voltage for each phase (A, B, C) or VM (input voltage)
float measure_voltage(ADC_HandleTypeDef *hadc) {
    HAL_ADC_PollForConversion(hadc, HAL_MAX_DELAY);
    uint32_t adc_value = HAL_ADC_GetValue(hadc);  // Get ADC value for voltage sensing
    return calculate_voltage(adc_value);          // Use shared voltage calculation function
}

int main(void) {
    // Initialize the system
    HAL_Init();
    SystemClock_Config();
    MX_GPIO_Init();
    MX_ADC1_Init();  // Phase A current sense
    MX_ADC2_Init();  // Phase B current sense
    MX_ADC3_Init();  // Voltage sensing (VSENSA, VSENSB, VSENSC)
    MX_ADC4_Init();  // Input voltage sensing (VSENVM)

    // Start the ADCs
    HAL_ADC_Start(&hadc1);
    HAL_ADC_Start(&hadc2);
    HAL_ADC_Start(&hadc3);
    HAL_ADC_Start(&hadc4);

    while (1) {
        // Measure currents
        float current_A = measure_phase_current_A();
        float current_B = measure_phase_current_B();
        float current_C = -(current_A + current_B);  // Phase C current calculation

        // Measure voltages for Phase A, B, C and input voltage (VSENVM)
        float voltage_A = measure_voltage(&hadc3);  // VSENSA
        float voltage_B = measure_voltage(&hadc3);  // VSENSB
        float voltage_C = measure_voltage(&hadc3);  // VSENSC
        float input_voltage = measure_voltage(&hadc4);  // VSENVM

        // Use current and voltage values for motor control or monitoring
        HAL_Delay(100);  // Adjust sampling rate if needed
    }
}

// ADC1 initialization function (for Phase A current sensing)
static void MX_ADC1_Init(void) {
    ADC_ChannelConfTypeDef sConfig = {0};
    hadc1.Instance = ADC1;
    hadc1.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV4;
    hadc1.Init.Resolution = ADC_RESOLUTION_12B;
    hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
    hadc1.Init.ScanConvMode = ADC_SCAN_DISABLE;
    HAL_ADC_Init(&hadc1);

    sConfig.Channel = ADC_CHANNEL_1;  // Phase A current sense
    sConfig.Rank = 1;
    sConfig.SamplingTime = ADC_SAMPLETIME_3CYCLES;
    HAL_ADC_ConfigChannel(&hadc1, &sConfig);
}

// ADC2 initialization function (for Phase B current sensing)
static void MX_ADC2_Init(void) {
    ADC_ChannelConfTypeDef sConfig = {0};
    hadc2.Instance = ADC2;
    hadc2.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV4;
    hadc2.Init.Resolution = ADC_RESOLUTION_12B;
    hadc2.Init.DataAlign = ADC_DATAALIGN_RIGHT;
    hadc2.Init.ScanConvMode = ADC_SCAN_DISABLE;
    HAL_ADC_Init(&hadc2);

    sConfig.Channel = ADC_CHANNEL_2;  // Phase B current sense
    sConfig.Rank = 1;
    sConfig.SamplingTime = ADC_SAMPLETIME_3CYCLES;
    HAL_ADC_ConfigChannel(&hadc2, &sConfig);
}

// ADC3 initialization function (for Voltage sensing: VSENSA, VSENSB, VSENSC)
static void MX_ADC3_Init(void) {
    ADC_ChannelConfTypeDef sConfig = {0};
    hadc3.Instance = ADC3;
    hadc3.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV4;
    hadc3.Init.Resolution = ADC_RESOLUTION_12B;
    hadc3.Init.DataAlign = ADC_DATAALIGN_RIGHT;
    hadc3.Init.ScanConvMode = ADC_SCAN_DISABLE;
    HAL_ADC_Init(&hadc3);

    sConfig.Channel = ADC_CHANNEL_3;  // Voltage sensing (VSENSA, VSENSB, VSENSC)
    sConfig.Rank = 1;
    sConfig.SamplingTime = ADC_SAMPLETIME_3CYCLES;
    HAL_ADC_ConfigChannel(&hadc3, &sConfig);
}

// ADC4 initialization function (for Input voltage: VSENVM)
static void MX_ADC4_Init(void) {
    ADC_ChannelConfTypeDef sConfig = {0};
    hadc4.Instance = ADC4;
    hadc4.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV4;
    hadc4.Init.Resolution = ADC_RESOLUTION_12B;
    hadc4.Init.DataAlign = ADC_DATAALIGN_RIGHT;
    hadc4.Init.ScanConvMode = ADC_SCAN_DISABLE;
    HAL_ADC_Init(&hadc4);

    sConfig.Channel = ADC_CHANNEL_4;  // Input voltage (VSENVM)
    sConfig.Rank = 1;
    sConfig.SamplingTime = ADC_SAMPLETIME_3CYCLES;
    HAL_ADC_ConfigChannel(&hadc4, &sConfig);
}

// System clock configuration (auto-generated)
void SystemClock_Config(void) {
    // System clock configuration here (auto-generated by STM32CubeMX)
}

// GPIO initialization
static void MX_GPIO_Init(void) {
    // GPIO initialization here (auto-generated by STM32CubeMX)
}
