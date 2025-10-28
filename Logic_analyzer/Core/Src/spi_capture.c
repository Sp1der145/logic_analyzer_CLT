/*
 * spi_capture.c
 *
 *  Created on: Oct 22, 2025
 *      Author: crupton23
 */
#include "stm32l4xx_hal.h"
#include <stdint.h>
#include <string.h>
#include <stdbool.h>

/* ----------------------------------------------------- */
/* ---------------------- SPI&GPIO --------------------- */
/* ----------------------------------------------------- */
//This sections needs to read SPI input from an outside source (another micro-chip) and shoves that data into Arrays based on the current trigger tracing & sample rate

/* ---------------- Var Defs ---------------- */
/* Buffer size defining */
#define SPI_CAPTURE_BUFFER_LEN 99U //Defined length of ARRAY buffer
//(NEED TO MAKE THIS 102 FOR THE 3 xBADF00D's CHECK EVENTUALLY) /\

/* Extern vars from mode1Source.c */
extern volatile uint8_t triggerTrace;   // current trigger trace value from FSM out
extern volatile uint32_t sampleRate;    // divider N (store every Nth CLK edge) MIGHT NEED TO CHANGE DUE TO THE FSM FOR THE ENCODER I HAVE TO MAKE STILL
										//PULL VALS FROM QUEUE (Ask Carroll how this is done)

/* Buffers */
volatile uint8_t CS_buf[SPI_CAPTURE_BUFFER_LEN];
volatile uint8_t CLK_buf[SPI_CAPTURE_BUFFER_LEN];
volatile uint8_t MOSI_buf[SPI_CAPTURE_BUFFER_LEN];
volatile uint32_t SPI_capture_count = 0;    // how many samples stored so far

/* Internal state */			//CONFIG PORTS LATER TEMP NULL VALS
static GPIO_TypeDef *CS_port = NULL;
static uint16_t       CS_pin  = 0;
static GPIO_TypeDef *CLK_port = NULL;
static uint16_t       CLK_pin  = 0;
static GPIO_TypeDef *MOSI_port = NULL;
static uint16_t       MOSI_pin  = 0;

static volatile bool capture_enabled = false;
static volatile uint32_t edge_counter = 0; /* counts CLK edges seen (for decimation) */

/* Forward declarations */
void SPI_CAPTURE_Init(GPIO_TypeDef *csPort, uint16_t csPin,
                      GPIO_TypeDef *clkPort, uint16_t clkPin,
                      GPIO_TypeDef *mosiPort, uint16_t mosiPin);
void SPI_CAPTURE_Start(void);
void SPI_CAPTURE_Stop(void);
void SPI_CAPTURE_Reset(void);
uint32_t SPI_CAPTURE_GetCount(void);
const uint8_t* SPI_CAPTURE_GetCSBuffer(void);
const uint8_t* SPI_CAPTURE_GetCLKBuffer(void);
const uint8_t* SPI_CAPTURE_GetMOSIBuffer(void);

/* ---------------- FXNS ---------------- */
void SPI_CAPTURE_Init(GPIO_TypeDef *csPort, uint16_t csPin, GPIO_TypeDef *clkPort, uint16_t clkPin, GPIO_TypeDef *mosiPort, uint16_t mosiPin)
{
    /* Store pin refs */
    CS_port   = csPort;   CS_pin   = csPin;
    CLK_port  = clkPort;  CLK_pin  = clkPin;
    MOSI_port = mosiPort; MOSI_pin = mosiPin;

    /* Reset buffers/state */
    SPI_CAPTURE_Reset();

    /* NOTE FOR FUTURE CONNER
       This function does NOT configure the GPIO modes for CS/CLK/MOSI.
       NEED TO configure
         - CS pin as input (pull-up or pull-down)
         - CLK pin as input with the desired edge (rising/falling) in CubeMX
         - MOSI pin as input
    */
}

/* Start/stop capture */
void SPI_CAPTURE_Start(void)
{
    /* Enable capture only if buffers exist and not full */
    capture_enabled = true;
}

void SPI_CAPTURE_Stop(void)
{
    capture_enabled = false;
}

/* Clear buffers and counters */
void SPI_CAPTURE_Reset(void)
{
    __disable_irq();
    memset((void*)CS_buf, 0, sizeof(CS_buf));
    memset((void*)CLK_buf, 0, sizeof(CLK_buf));
    memset((void*)MOSI_buf, 0, sizeof(MOSI_buf));
    SPI_capture_count = 0;
    edge_counter = 0;
    capture_enabled = false;
    __enable_irq();
}

/* Getters */
uint32_t SPI_CAPTURE_GetCount(void) { return SPI_capture_count; }
const uint8_t* SPI_CAPTURE_GetCSBuffer(void)   { return (const uint8_t*)CS_buf; }
const uint8_t* SPI_CAPTURE_GetCLKBuffer(void)  { return (const uint8_t*)CLK_buf; }
const uint8_t* SPI_CAPTURE_GetMOSIBuffer(void) { return (const uint8_t*)MOSI_buf; }

//REMV ????
/* Helper: When EXTI for CLK occurs, call this function (from HAL_GPIO_EXTI_Callback or directly) */
void SPI_CAPTURE_EXTI_Handler(uint16_t GPIO_Pin)
{
    if (GPIO_Pin != CLK_pin) return; /* not our pin */

    /* Quick sanity check (require pointers set) */
    if (CLK_port == NULL || MOSI_port == NULL || CS_port == NULL) return;

    /* If capturing disabled then ignore */
    if (!capture_enabled) return;

    /* If buffers full then optionally stop capturing  */
    if (SPI_capture_count >= SPI_CAPTURE_BUFFER_LEN) {
        capture_enabled = false;
        return;
    }

    /* Use sampleRate as a divider: store only every Nth CLK edge.
       If sampleRate == 0 (uninitialized), treat as divider = 1 (store every edge). */
    uint32_t div = (sampleRate == 0) ? 1u : sampleRate;
    edge_counter++;
    if ((edge_counter % div) != 0u) {
        /* skip this sample */
        return;
    }

    /* Read pins */
    uint8_t cs_val   = (uint8_t)(HAL_GPIO_ReadPin(CS_port, CS_pin) == GPIO_PIN_SET ? 1u : 0u);
    uint8_t clk_val  = (uint8_t)(HAL_GPIO_ReadPin(CLK_port, CLK_pin) == GPIO_PIN_SET ? 1u : 0u);
    uint8_t mosi_val = (uint8_t)(HAL_GPIO_ReadPin(MOSI_port, MOSI_pin) == GPIO_PIN_SET ? 1u : 0u);

    /* Optionally gate capturing on triggerTrace value:
       Here we only store when triggerTrace != 0.
    */
    if (triggerTrace == 0u) {
        return;
    }

    /* Store into buffers */
    __disable_irq();
    uint32_t idx = SPI_capture_count;
    if (idx < SPI_CAPTURE_BUFFER_LEN) {
        CS_buf[idx]   = cs_val;
        CLK_buf[idx]  = clk_val;
        MOSI_buf[idx] = mosi_val;
        SPI_capture_count = idx + 1;
    }
    __enable_irq();
}

/* HAL's EXTI callback - if we use HAL and haven't overridden this, it will be called automatically.
   If we already implement HAL_GPIO_EXTI_Callback elsewhere, call SPI_CAPTURE_EXTI_Handler(GPIO_Pin) from it. */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    /* Pass-through to module handler. If our project uses HAL callbacks for other pins, ensure we call their handlers too. */
    SPI_CAPTURE_EXTI_Handler(GPIO_Pin);
}

//MIGHT REMV TO NOT WASTE TIME.........
/* Optional: simple blocking wait to gather N samples (example helper) */
HAL_StatusTypeDef SPI_CAPTURE_WaitForSamples(uint32_t desired_samples, uint32_t timeout_ms)
{
    uint32_t start = HAL_GetTick();
    while (SPI_CAPTURE_GetCount() < desired_samples) {
        if ((HAL_GetTick() - start) > timeout_ms) return HAL_TIMEOUT;
    }
    return HAL_OK;
}
