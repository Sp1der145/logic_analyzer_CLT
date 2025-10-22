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

//DONT NEED??
//void SPI_CAPTURE_EXTI_Handler(uint16_t GPIO_Pin);

/* ---------------- FXNS ---------------- */

void SPI_CAPTURE_Init(GPIO_TypeDef *csPort, uint16_t csPin,
                      GPIO_TypeDef *clkPort, uint16_t clkPin,
                      GPIO_TypeDef *mosiPort, uint16_t mosiPin)
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
         - CLK pin as input with the desired edge (rising/falling) in CubeMX or manually below
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


