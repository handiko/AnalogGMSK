/*
 * AnalogGMSK.hpp
 *
 *  Created on: Dec 30, 2020
 *      Author: handiko
 */

#ifndef INC_ANALOGGMSK_HPP_
#define INC_ANALOGGMSK_HPP_

#include "main.h"

#ifndef STM32F4
#error "STM32F4xx must be used !"
#else

#define PULSE_RISING_EDGE 0
#define PULSE_FALLING_EDGE 1
#define PULSE_STOP 2

#define SAMPLE_PER_SYMBOL 64

#define ENABLE_MODULATION 1
#define DISABLE_MODULATION 0

#define NO_UART 0
#define USING_UART 1

struct InputPorts_t {
	GPIO_TypeDef* bitsPort = BITS_GPIO_Port;
	GPIO_TypeDef* pttPort = PTT_GPIO_Port;

	uint16_t bitsPin = BITS_Pin;
	uint16_t pttPin = PTT_Pin;
};

struct OutputPorts_t {
	GPIO_TypeDef* relayPort = RELAY_GPIO_Port;

	uint16_t relayPin = RELAY_Pin;
};

// TODO not implemented yet
struct LedPorts_t {
	GPIO_TypeDef* ledTxPort;
	GPIO_TypeDef* ledDataPort;
	GPIO_TypeDef* ledCHAPort;
	GPIO_TypeDef* ledCHBPort;

	uint16_t ledTxPin;
	uint16_t ledDataPin;
	uint16_t ledCHAPin;
	uint16_t ledCHBPin;
};

struct IRQs_t {
	IRQn_Type bitsIRQ = BITS_EXTI_IRQn;
	IRQn_Type pttIRQ = PTT_EXTI_IRQn;
};

class AnalogGMSK {
private:
	uint8_t pulseDir = PULSE_RISING_EDGE;
	uint16_t tick=0;
	const uint32_t gaussian[SAMPLE_PER_SYMBOL] = {
		19,41,64,91,119,151,186,223,
		264,308,356,407,462,520,582,647,
		717,790,866,946,1029,1115,1204,1295,
		1389,1485,1583,1683,1784,1885,1988,2090,
		2192,2294,2395,2494,2592,2689,2783,2874,
		2963,3049,3132,3212,3288,3361,3430,3496,
		3558,3616,3671,3722,3769,3814,3854,3892,
		3927,3958,3987,4013,4037,4058,4078,4095
	};

	//TODO : Modulation is enabled at start (for debugging)
	bool EnableModulation = ENABLE_MODULATION;

	struct PeripheralTypeDef {
		TIM_HandleTypeDef *htim;
		DAC_HandleTypeDef *hdac;
		UART_HandleTypeDef *huart;
		InputPorts_t inputPorts;
		OutputPorts_t outputPorts;
		IRQs_t interrupts;
	} peripherals;

	bool UsingUART = NO_UART;

	void initGPIOClock(void);
	void initGPIOOutputLevel(void);
	void initConfigGPIOPinsOutput(void);
	void initConfigGPIOPinsInput(void);
	void initNVICInterrupt(void);
	void init(void);

	void ModulateRisingEdge(void);
	void ModulateFallingEdge(void);

public:
	AnalogGMSK();

	void Start(InputPorts_t inputPorts, OutputPorts_t outputPorts, DAC_HandleTypeDef *hdac, TIM_HandleTypeDef *htim, IRQs_t interrupts);
	void Start(InputPorts_t inputPorts, OutputPorts_t outputPorts, DAC_HandleTypeDef *hdac, TIM_HandleTypeDef *htim, IRQs_t interrupts, UART_HandleTypeDef *huart);

	void Tick(void);

	void SetPulseRising(void);
	void SetPulseFalling(void);

	void StartModulate(void);
	void StopModulate(void);

	virtual ~AnalogGMSK();
};

#endif

#endif /* SRC_ANALOGGMSK_HPP_ */
