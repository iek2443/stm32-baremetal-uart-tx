# STM32 Bare-Metal: UART Transmit using USART2 (PA2)

---

## 📌 Summary

This project demonstrates how to transmit data over UART using **USART2** and **PA2 (TX)** pin on the STM32F4 Discovery board.  
It uses direct register-level configuration for GPIO and USART peripherals — no HAL or CMSIS is used.  
This is the first project in the series to introduce **serial communication (UART TX)** with manual baud rate calculation and pin remapping using alternate function configuration.

---

## 🔁 Previous Lesson

If you haven’t completed the previous step where we used GPIO input (user button) to control LEDs, check it out first:

👉 [Previous Lesson: Button-Controlled LED Blink](https://github.com/iek2443/stm32-baremetal-button-input)

---

## 🧠 What You Will Learn

- How to configure PA2 as an alternate function (AF7) for UART transmission
- How to enable and configure **USART2** peripheral manually
- How to calculate and set the correct baud rate without HAL
- How to send a character using low-level USART registers
- The difference between using `=` and `|=` when configuring control registers

---

## ⚙️ Key Registers Used

- `RCC->AHB1ENR` → Enables clock to GPIOA
- `RCC->APB1ENR` → Enables clock to USART2
- `GPIOA->MODER` → Configures PA2 in Alternate Function mode
- `GPIOA->AFRL`  → Selects AF7 (USART2_TX) for PA2
- `USART2->BRR`  → Baud rate register (calculated manually)
- `USART2->CR1`  → Control register (for enabling TX and USART)
- `USART2->SR`   → Status register (TXE flag check)
- `USART2->DR`   → Data register (to send the byte)

---

## 🔧 Requirements

- STM32F4 Discovery Board
- ARM GCC Toolchain
- ST-Link programmer
- STM32CubeProgrammer or OpenOCD
- USB-to-UART converter (if connecting to PC)
- Terminal emulator (PuTTY, TeraTerm, screen, etc.)

---

📁 Project Structure
--------------------

stm32-baremetal-uart-tx/\
├── src/\
│   └── main.c         --> Bare-metal USART2 TX implementation (PA2)\
├── inc/               --> (Optional: header files)\
└── README.md

---

🧭 Pin Mapping

| Function         | Port | Pin | Description         |
|------------------|------|-----|---------------------|
| UART2_TX         | A    |  2  | Serial transmit pin |
| USART2 Peripheral|      |     | APB1 connected      |

---
