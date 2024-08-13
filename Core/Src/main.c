#include "main.h"
#include "usart.h"
#include "gpio.h"
#include "adc.h"
#include "tim.h"
#include "string.h"
#include "stdio.h"

// UART buffer and state variables
#define UART_BUFFER_SIZE 100
char uart_buffer[UART_BUFFER_SIZE];
volatile uint8_t uart_index = 0;
volatile uint8_t command_received = 0;

// Function prototypes
void handle_command(char *cmd);
void send_response(char *response);

// Command functions
void help_command(void);
void clear_command(void);
void set_pin_command(char *args);
void get_pin_command(char *args);
void toggle_pin_command(char *args);
void read_adc_command(char *args);
void get_info_command(void);
void start_timer_command(char *args);
void stop_timer_command(void);

// Helper functions
GPIO_TypeDef* get_port_from_pin(char *pin, uint16_t *pin_num);

int main(void)
{
    HAL_Init();
    SystemClock_Config();
    MX_GPIO_Init();
    MX_USART2_UART_Init();
    MX_ADC1_Init();
    MX_TIM2_Init();

    // Start UART in interrupt mode
    HAL_UART_Receive_IT(&huart2, (uint8_t*)&uart_buffer[uart_index], 1);

    while (1)
    {
        if (command_received)
        {
            uart_buffer[uart_index] = '\0';  // Null-terminate the received command
            handle_command((char *)uart_buffer);
            uart_index = 0;  // Reset index for the next command
            command_received = 0;
            HAL_UART_Receive_IT(&huart2, (uint8_t*)&uart_buffer[uart_index], 1);  // Restart UART reception
        }
    }
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if (uart_buffer[uart_index] == '\r')  // Command termination character
    {
        command_received = 1;
    }
    else
    {
        uart_index++;
        if (uart_index < UART_BUFFER_SIZE)  // Prevent buffer overflow
        {
            HAL_UART_Receive_IT(huart, (uint8_t*)&uart_buffer[uart_index], 1);
        }
        else
        {
            uart_index = 0;  // Reset if overflow
        }
    }
}

void handle_command(char *cmd)
{
    char command[20];
    sscanf(cmd, "%s", command);  // Extract command from the input string

    if (strcmp(command, "help") == 0)
    {
        help_command();
    }
    else if (strcmp(command, "clear") == 0)
    {
        clear_command();
    }
    else if (strcmp(command, "set_pin") == 0)
    {
        set_pin_command(cmd + strlen(command));
    }
    else if (strcmp(command, "get_pin") == 0)
    {
        get_pin_command(cmd + strlen(command));
    }
    else if (strcmp(command, "toggle_pin") == 0)
    {
        toggle_pin_command(cmd + strlen(command));
    }
    else if (strcmp(command, "read_adc") == 0)
    {
        read_adc_command(cmd + strlen(command));
    }
    else if (strcmp(command, "get_info") == 0)
    {
        get_info_command();
    }
    else if (strcmp(command, "start_timer") == 0)
    {
        start_timer_command(cmd + strlen(command));
    }
    else if (strcmp(command, "stop_timer") == 0)
    {
        stop_timer_command();
    }
    else
    {
        send_response("Unknown command\r\n");
    }
}

void send_response(char *response)
{
    HAL_UART_Transmit(&huart2, (uint8_t*)response, strlen(response), HAL_MAX_DELAY);
}

// GPIO Commands
GPIO_TypeDef* get_port_from_pin(char *pin, uint16_t *pin_num)
{
    if (strncmp(pin, "PA", 2) == 0)
    {
        *pin_num = 1 << (pin[2] - '0');
        return GPIOA;
    }
    // Add additional ports as needed (e.g., GPIOB, GPIOC)
    return NULL;
}

void set_pin_command(char *args)
{
    char pin[4];
    int state;
    sscanf(args, "%s %d", pin, &state);

    uint16_t pin_num;
    GPIO_TypeDef *port = get_port_from_pin(pin, &pin_num);

    if (port != NULL)
    {
        HAL_GPIO_WritePin(port, pin_num, state ? GPIO_PIN_SET : GPIO_PIN_RESET);
        send_response("Pin set successfully\r\n");
    }
    else
    {
        send_response("Invalid pin\r\n");
    }
}

void get_pin_command(char *args)
{
    char pin[4];
    sscanf(args, "%s", pin);

    uint16_t pin_num;
    GPIO_TypeDef *port = get_port_from_pin(pin, &pin_num);

    if (port != NULL)
    {
        GPIO_PinState pin_state = HAL_GPIO_ReadPin(port, pin_num);
        char response[32];
        sprintf(response, "Pin state: %d\r\n", pin_state);
        send_response(response);
    }
    else
    {
        send_response("Invalid pin\r\n");
    }
}

void toggle_pin_command(char *args)
{
    char pin[4];
    sscanf(args, "%s", pin);

    uint16_t pin_num;
    GPIO_TypeDef *port = get_port_from_pin(pin, &pin_num);

    if (port != NULL)
    {
        HAL_GPIO_TogglePin(port, pin_num);
        send_response("Pin toggled successfully\r\n");
    }
    else
    {
        send_response("Invalid pin\r\n");
    }
}

// ADC Command
void read_adc_command(char *args)
{
    int channel;
    sscanf(args, "%d", &channel);

    ADC_ChannelConfTypeDef sConfig = {0};
    sConfig.Channel = channel;
    sConfig.Rank = ADC_REGULAR_RANK_1;
    sConfig.SamplingTime = ADC_SAMPLETIME_1CYCLE_5;
    HAL_ADC_ConfigChannel(&hadc1, &sConfig);

    HAL_ADC_Start(&hadc1);
    HAL_ADC_PollForConversion(&hadc1, HAL_MAX_DELAY);
    uint32_t adc_value = HAL_ADC_GetValue(&hadc1);

    char response[32];
    sprintf(response, "ADC Value: %lu\r\n", adc_value);
    send_response(response);
}

// System Info Command
void get_info_command(void)
{
    send_response("STM32 UART Shell v1.0\r\n");
}

// Timer Commands
void start_timer_command(char *args)
{
    int period;
    sscanf(args, "%d", &period);
    __HAL_TIM_SET_AUTORELOAD(&htim2, period - 1);
    HAL_TIM_Base_Start_IT(&htim2);
    send_response("Timer started\r\n");
}

void stop_timer_command(void)
{
    HAL_TIM_Base_Stop_IT(&htim2);
    send_response("Timer stopped\r\n");
}

// Help Command
void help_command(void)
{
    const char *help_text = "Available commands:\r\n"
                            "help            - Show this help message\r\n"
                            "clear           - Clear the terminal\r\n"
                            "set_pin <pin> <state> - Set GPIO pin state (0 or 1)\r\n"
                            "get_pin <pin>   - Get GPIO pin state\r\n"
                            "toggle_pin <pin> - Toggle GPIO pin\r\n"
                            "read_adc <channel> - Read ADC value from channel\r\n"
                            "get_info        - Get system information\r\n"
                            "start_timer <period> - Start a timer with the given period (ms)\r\n"
                            "stop_timer      - Stop the timer\r\n";
    send_response((char *)help_text);
}

// Clear Command
void clear_command(void)
{
    const char *clear_text = "\033[H\033[J";  // ANSI escape code to clear terminal
    send_response((char *)clear_text);
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
    RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

    /** Initializes the RCC Oscillators according to the specified parameters
    * in the RCC_OscInitTypeDef structure.
    */
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
    RCC_OscInitStruct.HSIState = RCC_HSI_ON;
    RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI_DIV2;
    RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL16;
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
    {
        Error_Handler();
    }

    /** Initializes the CPU, AHB and APB buses clocks
    */
    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                                |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
    {
        Error_Handler();
    }
    PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_ADC;
    PeriphClkInit.AdcClockSelection = RCC_ADCPCLK2_DIV2;
    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
    {
        Error_Handler();
    }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
    __disable_irq();
    while (1)
    {
    }
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
    // Add custom implementation if needed
}
#endif /* USE_FULL_ASSERT */
