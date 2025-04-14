#include <stdint.h>

// ==============================
// Base Address Definitions
// ==============================

#define PERIPH_BASE             (0x40000000UL)                      // Base address for all peripherals
#define AHB1PERIPH_OFFSET       (0x00020000UL)
#define AHB1PERIPH_BASE         (PERIPH_BASE + AHB1PERIPH_OFFSET)

#define APB1PERIPH_OFFSET       (0x00000000UL)
#define APB1PERIPH_BASE         (PERIPH_BASE + APB1PERIPH_OFFSET)

#define GPIOA_OFFSET            (0x0000UL)
#define RCC_OFFSET              (0x3800UL)
#define USART_OFFSET            (0x4400UL)

#define GPIOA_BASE              (AHB1PERIPH_BASE + GPIOA_OFFSET)
#define RCC_BASE                (AHB1PERIPH_BASE + RCC_OFFSET)
#define USART2_BASE             (APB1PERIPH_BASE + USART_OFFSET)

// Clock enable bits
#define GPIOAEN                 (1U << 0)                           // Bit 0 enables clock to GPIOA
#define USART2EN                (1U << 17)                          // Bit 17 enables clock to USART2

#define SYS_FREQ                16000000                            // System frequency = 16 MHz
#define APB1_CLK                SYS_FREQ                            // APB1 clock assumed same as system
#define UART_BAUDRATE           115200                              // Desired baud rate for UART

// ==============================
// Typedef Struct Definitions
// ==============================

#define __IO volatile

typedef struct {
	__IO uint32_t MODER;         // Mode register
	__IO uint32_t OTYPER;        // Output type register
	__IO uint32_t OSPEEDR;       // Output speed register
	__IO uint32_t PUPDR;         // Pull-up/pull-down register
	__IO uint32_t IDR;           // Input data register
	__IO uint32_t ODR;           // Output data register
	__IO uint32_t BSRR;          // Bit set/reset register
	__IO uint32_t LCKR;          // Configuration lock register
	__IO uint32_t AFRL;          // Alternate function low register (pins 0–7)
	__IO uint32_t AFRH;          // Alternate function high register (pins 8–15)
} GPIO_TypeDef;

typedef struct {
	__IO uint32_t CR;
	__IO uint32_t PLLCFGR;
	__IO uint32_t CFGR;
	__IO uint32_t CIR;
	__IO uint32_t AHB1RSTR;
	__IO uint32_t AHB2RSTR;
	__IO uint32_t AHB3RSTR;
	uint32_t RESERVED0;
	__IO uint32_t APB1RSTR;
	__IO uint32_t APB2RSTR;
	uint32_t RESERVED1[2];
	__IO uint32_t AHB1ENR;
	__IO uint32_t AHB2ENR;
	__IO uint32_t AHB3ENR;
	uint32_t RESERVED2;
	__IO uint32_t APB1ENR;
	__IO uint32_t APB2ENR;
	uint32_t RESERVED3[2];
	__IO uint32_t AHB1LPENR;
	__IO uint32_t AHB2LPENR;
	__IO uint32_t AHB3LPENR;
	uint32_t RESERVED4;
	__IO uint32_t APB1LPENR;
	__IO uint32_t APB2LPENR;
	uint32_t RESERVED5[2];
	__IO uint32_t BDCR;
	__IO uint32_t CSR;
	uint32_t RESERVED6[2];
	__IO uint32_t SSCGR;
	__IO uint32_t PLLI2SCFGR;
	__IO uint32_t PLLSAICFGR;
	__IO uint32_t DCKCFGR;
} RCC_TypeDef;

typedef struct {
	__IO uint32_t USART_SR;      // Status register
	__IO uint32_t USART_DR;      // Data register
	__IO uint32_t USART_BRR;     // Baud rate register
	__IO uint32_t USART_CR1;     // Control register 1
	__IO uint32_t USART_CR2;
	__IO uint32_t USART_CR3;
	__IO uint32_t USART_GTPR;
} USART_TypeDef;

// Peripheral base pointers
#define RCC     ((RCC_TypeDef *) RCC_BASE)
#define GPIOA   ((GPIO_TypeDef *) GPIOA_BASE)
#define USART2  ((USART_TypeDef *) USART2_BASE)

// Function prototypes
void uart_tx_init(void);
void uart2_write(int ch);
static void uart_set_baudrate(USART_TypeDef *USARTx, uint32_t PeriphClk, uint32_t BaudRate);
static uint16_t compute_uart_bd(uint32_t PeriphClk, uint32_t BaudRate);

int main(void) {

	uart_tx_init();               // Initialize UART transmission (PA2)

	while (1) {
		uart2_write('Y');         // Transmit character 'Y' continuously
	}
}

void uart_tx_init(void) {

	/********** Configure uart gpio pin **********/

	/* Enable clock access to gpio */
	RCC->AHB1ENR |= GPIOAEN;

	/* Set PA2 mode to alternate function mode */
	GPIOA->MODER |= (1U << 5);        // Set MODER2[1]
	GPIOA->MODER &= ~(1U << 4);       // Clear MODER2[0] → MODER2 = 10 (AF)

	/* Set PA2 alternate function type to UART_TX (AF7) */
	GPIOA->AFRL |= (1U << 8);         // AFRL2[0]
	GPIOA->AFRL |= (1U << 9);         // AFRL2[1]
	GPIOA->AFRL |= (1U << 10);        // AFRL2[2]
	GPIOA->AFRL &= ~(1U << 11);       // AFRL2[3] should be cleared, but this line has incorrect use of ~!

	/********** Configure uart module **********/
	/* Enable clock access to uart2 */
	RCC->APB1ENR |= USART2EN;

	/* Configure baudrate */
	uart_set_baudrate(USART2, APB1_CLK, UART_BAUDRATE);

	/* Configure the transfer direction */
	USART2->USART_CR1 = (1U << 3);     // TE = 1 (enable transmitter)

	/* Enable uart module */
	USART2->USART_CR1 |= (1U << 13);   // UE = 1 (USART enable)
}

void uart2_write(int ch) {
	/* Make sure the transmit data register is empty */
	while (!(USART2->USART_SR & (1U << 7))) { }

	/* Write to transmit data register */
	USART2->USART_DR = (ch & 0xFF);
}

static void uart_set_baudrate(USART_TypeDef *USARTx, uint32_t PeriphClk, uint32_t BaudRate) {
	USARTx->USART_BRR = compute_uart_bd(PeriphClk, BaudRate);
}

static uint16_t compute_uart_bd(uint32_t PeriphClk, uint32_t BaudRate) {
	return ((PeriphClk + (BaudRate / 2U)) / BaudRate);
}



