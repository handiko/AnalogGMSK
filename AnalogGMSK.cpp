/*
 * AnalogGMSK.cpp
 *
 *  Created on: Dec 30, 2020
 *      Author: handiko
 */

#include "AnalogGMSK.hpp"

AnalogGMSK::AnalogGMSK() {
	// TODO Auto-generated constructor stub
	UsingUART = NO_UART;
	tick = 0;
}
/*
 * Private methods
 */
void AnalogGMSK::initGPIOClock(void)
{
#if defined(GPIOA)
	__HAL_RCC_GPIOA_CLK_ENABLE();
#endif

#if defined(GPIOB)
	__HAL_RCC_GPIOB_CLK_ENABLE();
#endif

#if defined(GPIOC)
	__HAL_RCC_GPIOC_CLK_ENABLE();
#endif

#if defined(GPIOD)
	__HAL_RCC_GPIOD_CLK_ENABLE();
#endif

#if defined(GPIOE)
	__HAL_RCC_GPIOE_CLK_ENABLE();
#endif

#if defined(GPIOF)
	__HAL_RCC_GPIOF_CLK_ENABLE();
#endif

#if defined(GPIOG)
	__HAL_RCC_GPIOG_CLK_ENABLE();
#endif

#if defined(GPIOH)
	__HAL_RCC_GPIOH_CLK_ENABLE();
#endif
}

void AnalogGMSK::initGPIOOutputLevel(void)
{
	peripherals.outputPorts.relayPort->BSRR = peripherals.outputPorts.relayPin << 16;
}

void AnalogGMSK::initConfigGPIOPinsOutput(void)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};

	GPIO_InitStruct.Pin = peripherals.outputPorts.relayPin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
#if defined(GPIO_SPEED_FREQ_VERY_HIGH)
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
#endif

	HAL_GPIO_Init(peripherals.outputPorts.relayPort, &GPIO_InitStruct);
}

void AnalogGMSK::initConfigGPIOPinsInput(void)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};

	GPIO_InitStruct.Pin = peripherals.inputPorts.bitsPin;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	HAL_GPIO_Init(peripherals.inputPorts.bitsPort, &GPIO_InitStruct);

	GPIO_InitStruct.Pin = peripherals.inputPorts.pttPin;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	HAL_GPIO_Init(peripherals.inputPorts.pttPort, &GPIO_InitStruct);
}

void AnalogGMSK::initNVICInterrupt(void)
{
	HAL_NVIC_SetPriority(peripherals.interrupts.pttIRQ, 0, 0);
	HAL_NVIC_EnableIRQ(peripherals.interrupts.pttIRQ);

	HAL_NVIC_SetPriority(peripherals.interrupts.bitsIRQ, 0, 0);
	HAL_NVIC_EnableIRQ(peripherals.interrupts.bitsIRQ);
}

void AnalogGMSK::init(void)
{
	initGPIOClock();
	initGPIOOutputLevel();
	initConfigGPIOPinsOutput();
	initConfigGPIOPinsInput();
	initNVICInterrupt();

	HAL_TIM_Base_Start_IT(peripherals.htim);

	HAL_DAC_Start(peripherals.hdac, DAC_CHANNEL_1);

	if(UsingUART == USING_UART)
	{

	}
}

void AnalogGMSK::ModulateRisingEdge(void)
{
	HAL_DAC_SetValue(peripherals.hdac, DAC_CHANNEL_1, DAC_ALIGN_12B_R, gaussian[tick++]);
	if(tick == SAMPLE_PER_SYMBOL)
	{
		pulseDir = PULSE_STOP;
	}
}

void AnalogGMSK::ModulateFallingEdge(void)
{
	HAL_DAC_SetValue(peripherals.hdac, DAC_CHANNEL_1, DAC_ALIGN_12B_R, gaussian[--tick]);
	if(tick == 0)
	{
		pulseDir = PULSE_STOP;
	}
}

/*
 * Public methods
 */
void AnalogGMSK::Start(InputPorts_t inputPorts, OutputPorts_t outputPorts, DAC_HandleTypeDef *hdac, TIM_HandleTypeDef *htim, IRQs_t interrupts)
{
	this->peripherals.inputPorts = inputPorts;
	this->peripherals.outputPorts = outputPorts;
	this->peripherals.htim = htim;
	this->peripherals.hdac = hdac;
	this->peripherals.interrupts = interrupts;

	UsingUART = NO_UART;

	init();
}

void AnalogGMSK::Start(InputPorts_t inputPorts, OutputPorts_t outputPorts, DAC_HandleTypeDef *hdac, TIM_HandleTypeDef *htim, IRQs_t interrupts, UART_HandleTypeDef *huart)
{
	this->peripherals.inputPorts = inputPorts;
	this->peripherals.outputPorts = outputPorts;
	this->peripherals.htim = htim;
	this->peripherals.hdac = hdac;
	this->peripherals.interrupts = interrupts;
	this->peripherals.huart = huart;

	UsingUART = USING_UART;

	init();
}

void AnalogGMSK::Tick(void)
{
	if(pulseDir == PULSE_RISING_EDGE)
	{
		ModulateRisingEdge();
	}
	else if(pulseDir == PULSE_FALLING_EDGE)
	{
		ModulateFallingEdge();
	}
}

void AnalogGMSK::SetPulseRising(void)
{
	peripherals.outputPorts.relayPort->BSRR = peripherals.outputPorts.relayPin;

	pulseDir = PULSE_RISING_EDGE;
	tick = 0;
}

void AnalogGMSK::SetPulseFalling(void)
{
	peripherals.outputPorts.relayPort->BSRR = peripherals.outputPorts.relayPin << 16;

	pulseDir = PULSE_FALLING_EDGE;
	tick = SAMPLE_PER_SYMBOL;
}

AnalogGMSK::~AnalogGMSK() {
	// TODO Auto-generated destructor stub
}

