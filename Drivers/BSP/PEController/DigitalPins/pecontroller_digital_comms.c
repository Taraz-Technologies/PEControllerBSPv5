/**
 ********************************************************************************
 * @file    	pecontroller_digital_inout.c
 * @author 		Waqas Ehsan Butt
 * @date    	September 2, 2021
 *
 * @brief   Controls the digital input / output port of the PEController
 ********************************************************************************
 * @attention
 *
 * <h2><center>&copy; Copyright (c) 2021 Taraz Technologies Pvt. Ltd.</center></h2>
 * <h3><center>All rights reserved.</center></h3>
 *
 * <center>This software component is licensed by Taraz Technologies under BSD 3-Clause license,
 * the "License"; You may not use this file except in compliance with the License. You may obtain
 * a copy of the License at:
 *                        www.opensource.org/licenses/BSD-3-Clause</center>
 *
 ********************************************************************************
 */

/********************************************************************************
 * Includes
 *******************************************************************************/
#include "pecontroller_digital_comms.h"
/********************************************************************************
 * Defines
 *******************************************************************************/
#define DCOMM_COUNT						(8)
/********************************************************************************
 * Typedefs
 *******************************************************************************/

/********************************************************************************
 * Structures
 *******************************************************************************/

/********************************************************************************
 * Static Variables
 *******************************************************************************/
/** Defines the pins associated with digital input / output port
 */
static const digital_pin_t dCommPins[DCOMM_COUNT] =
{
		{.GPIO = GPIOC, .pinMask = GPIO_PIN_10},
		{.GPIO = GPIOC, .pinMask = GPIO_PIN_11},
		{.GPIO = GPIOA, .pinMask = GPIO_PIN_15},
		{.GPIO = GPIOB, .pinMask = GPIO_PIN_2},
		{.GPIO = GPIOC, .pinMask = GPIO_PIN_12},
		{.GPIO = GPIOD, .pinMask = GPIO_PIN_2},
		{.GPIO = GPIOH, .pinMask = GPIO_PIN_7},
		{.GPIO = GPIOC, .pinMask = GPIO_PIN_9},
};
/********************************************************************************
 * Global Variables
 *******************************************************************************/

/********************************************************************************
 * Function Prototypes
 *******************************************************************************/

/********************************************************************************
 * Code
 *******************************************************************************/
/**
 * @brief Initialize the Pin with default parameters
 * @param pinNo Pin number of the specified pin
 * @param *GPIO_InitStruct Pointer to the GPIO Structure
 * @param state Output State
 * @return digital_pin_t* pointer to the pin structure
 */
static const digital_pin_t* InitPin(uint32_t pinNo, GPIO_InitTypeDef* GPIO_InitStruct, GPIO_PinState state)
{
	const digital_pin_t* pin = &dCommPins[pinNo - 1];
	GPIO_InitStruct->Pin = pin->pinMask;
	GPIO_InitStruct->Pull = GPIO_NOPULL;
	GPIO_InitStruct->Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	HAL_GPIO_WritePin(pin->GPIO, pin->pinMask, state);
	HAL_GPIO_Init(pin->GPIO, GPIO_InitStruct);
	return pin;
}
/**
 * @brief Set the DComm pin as GPIO
 * @param pinNo DComm Pin No ( Range 1 - 8)
 * @param state State of the pin
 * @return *pin Pointer to the DComm pin structure
 */
const digital_pin_t* BSP_DComm_SetAsIOPin(uint32_t pinNo, GPIO_PinState state)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	return InitPin(pinNo, &GPIO_InitStruct, state);
}

/**
 * @brief Selects the Alternate Output Functionality. To configure as IO use BSP_DComm_SetAsIOPin(pinNo, state)
 * @param pinNo DComm pin No (Range 1-16)
 * @param AlternateFunction Alternate Functionality to be used
 * @return *digital_pin_t pointer to the pin structure
 */
const digital_pin_t* BSP_DComm_SetPinAlternateFunction(uint32_t pinNo, uint32_t AlternateFunction)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Alternate = AlternateFunction;
	return InitPin(pinNo, &GPIO_InitStruct, GPIO_PIN_RESET);
}
/**
 * @brief Set the DComm Port as Input.
 *
 */
void BSP_DComm_SetAsInputPort(void)
{
	// set the io buffer as input
	static GPIO_InitTypeDef GPIO_InitStruct = {0};
	for (int i = 0; i < DCOMM_COUNT; i++)
	{
		GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
		InitPin(i + 1, &GPIO_InitStruct, GPIO_PIN_RESET);
	}
}
/**
 * @brief Set the IO Port as Output. Pin 9 is always output
 * @note Initial value for output pin is 0
 *
 */
void BSP_DComm_SetAsOutputPort(void)
{
	static GPIO_InitTypeDef GPIO_InitStruct = {0};
	for (int i = 0; i < DCOMM_COUNT; i++)
	{
		GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
		InitPin(i + 1, &GPIO_InitStruct, GPIO_PIN_RESET);
	}
}
/*!
 * @brief Set the value of output port
 * @param val- value to be set
 */
void BSP_DComm_SetPortValue(uint32_t val)
{
	for(int i = 0; i < DCOMM_COUNT; i++)
	{
		const digital_pin_t* pin = &dCommPins[i];
		HAL_GPIO_WritePin(pin->GPIO, pin->pinMask, (GPIO_PinState)(val & 0x1));
		val = val >> 1U;
	}
}
/*!
 * @brief Gets the value of the input port
 * @returns val- value on the port
 */
uint32_t BSP_DComm_GetPortValue(void)
{
	uint32_t val = 0;
	for(int i = DCOMM_COUNT-1; i > 0; i--)
	{
		const digital_pin_t* pin = &dCommPins[i];
		val |= (uint32_t)HAL_GPIO_ReadPin(pin->GPIO, pin->pinMask);
		val = val << 1U;
	}
	val |= (uint32_t)HAL_GPIO_ReadPin(dCommPins[0].GPIO, dCommPins[0].pinMask);
	return val;
}
/* EOF */
