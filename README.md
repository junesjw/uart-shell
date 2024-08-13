# STM32 UART Shell/REPL

This project implements a UART Shell/REPL (Read-Eval-Print Loop) on an STM32F103RB Nucleo board. The shell allows you to interact with various hardware peripherals via UART commands. It provides an interface for controlling GPIO pins, reading ADC values, starting/stopping timers, and retrieving system information.

## Features

- **Help Command**: Displays a list of available commands.
- **Clear Command**: Clears the terminal screen.
- **GPIO Control**: 
  - `set_pin <pin> <state>`: Set a GPIO pin to a high or low state.
  - `get_pin <pin>`: Retrieve the current state of a GPIO pin.
  - `toggle_pin <pin>`: Toggle the state of a GPIO pin.
- **ADC Reading**: 
  - `read_adc <channel>`: Read the value from an ADC channel.
- **Timer Control**: 
  - `start_timer <period>`: Start a timer with a specified period.
  - `stop_timer`: Stop the running timer.
- **System Info**: 
  - `get_info`: Retrieve system information like firmware version.

## Hardware Requirements

- **STM32F103RB Nucleo Board**
- **USB Cable** to connect the Nucleo board to your computer

## Software Requirements

- **STM32CubeIDE**: Integrated development environment for STM32 microcontrollers.
- **STM32CubeMX**: Optional, for configuring peripherals.
- **PuTTY/Tera Term**: Or any serial terminal to interface with the UART shell.

## Project Structure

- `Core/`: Contains the main application code and peripheral configuration.
  - `Src/`: Source files for application logic.
  - `Inc/`: Header files.
- `Drivers/`: Peripheral driver code provided by STM32 HAL library.
- `README.md`: This file.
- `.gitignore`: Defines files and directories to be ignored by Git.

## Getting Started

### 1. Setup and Configuration

- **Clone the Repository**:
  ```bash
  git clone https://github.com/your-username/stm32-uart-shell.git
  cd stm32-uart-shell
  ```

- **Open the Project in STM32CubeIDE**:
  - Launch STM32CubeIDE.
  - Import the project by selecting "File" -> "Open Projects from File System" and navigating to the project directory.

- **Build and Flash**:
  - Click on the build icon in STM32CubeIDE to compile the project.
  - Connect your STM32 Nucleo board to your PC and flash the program.

### 2. Connecting to the Shell

- **Open a Serial Terminal**:
  - Use PuTTY, Tera Term, or any terminal application.
  - Set the Baud rate to `115200`.
  - Connect to the appropriate COM port.

- **Interacting with the Shell**:
  - Type `help` in the terminal to see the available commands.
  - Use the commands to control GPIOs, read ADC values, and more.

## Commands

- `help` : Show this help message.
- `clear` : Clear the terminal.
- `set_pin <pin> <state>` : Set a GPIO pin state (0 or 1).
- `get_pin <pin>` : Get GPIO pin state.
- `toggle_pin <pin>` : Toggle GPIO pin.
- `read_adc <channel>` : Read ADC value from a specific channel.
- `get_info` : Get system information.
- `start_timer <period>` : Start a timer with the given period (in ms).
- `stop_timer` : Stop the running timer.

## Future Enhancements

- Implement more advanced commands for peripheral control.
- Add support for additional communication interfaces like I2C and SPI.
- Implement logging and data storage features.

## Contributing

Contributions are welcome! Feel free to submit a pull request or open an issue for discussion.

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.