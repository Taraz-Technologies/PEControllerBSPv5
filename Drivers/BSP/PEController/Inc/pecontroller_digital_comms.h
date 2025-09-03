/**
 ********************************************************************************
 * @file 		pecontroller_digital_inout.h
 * @author 		Waqas Ehsan Butt
 * @date 		September 25, 2021
 *
 * @brief	Controls the digital input / output port of the PEController
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
#ifndef PECONTROLLER_DIGITAL_DCOMMS_H
#define PECONTROLLER_DIGITAL_DCOMMS_H

#ifdef __cplusplus
extern "C" {
#endif
/** @addtogroup BSP
 * @{
 */

/** @addtogroup DigitalPins
 * @{
 */

/** @addtogroup DCommPorts Digital Input Output Port
 * @brief Contains the declaration and procedures for the digital input/output pins
 * @details This port can either be input only or output only at a given time.
 * The port direction can be changed by the functions @ref BSP_DComm_SetAsInputPort() and @ref BSP_DComm_SetAsOutputPort().<br>
 * List of functions
 * 	-# <b>@ref BSP_DComm_SetAsIOPin() :</b> Set the DComm pin as GPIO
 * 	-# <b>@ref BSP_DComm_SetPinAlternateFunction() :</b> Selects the Alternate Output Functionality.
 * 													To configure as IO use @ref BSP_DComm_SetAsIOPin()
 * 	-# <b>@ref BSP_DComm_SetAsInputPort() :</b> Set the IO Port as Input. Pin 9 is always output
 * 	-# <b>@ref BSP_DComm_SetAsOutputPort() :</b> Set the IO Port as Output. Pin 9 is always output
 * 	-# <b>@ref BSP_DComm_SetPortValue() :</b> Set the value of output port
 * 	-# <b>@ref BSP_DComm_GetPortValue() :</b> Gets the value of the input port
 * @{
 */
/*******************************************************************************
 * Includes
 ******************************************************************************/
#include "pecontroller_digital_pins.h"
/*******************************************************************************
 * Defines
 ******************************************************************************/

/*******************************************************************************
 * Typedefs
 ******************************************************************************/

/*******************************************************************************
 * Structures
 ******************************************************************************/

 /*******************************************************************************
 * Exported Variables
 ******************************************************************************/

/*******************************************************************************
 * Global Function Prototypes
 ******************************************************************************/
/** @defgroup DIO_Exported_Functions Functions
  * @{
  */
/**
 * @brief Set the DComm pin as GPIO
 * @param pinNo DComm Pin No ( Range 1 - 8)
 * @param state State of the pin
 * @return *pin Pointer to the DComm pin structure
 */
extern const digital_pin_t* BSP_DComm_SetAsIOPin(uint32_t pinNo, GPIO_PinState state);
/**
 * @brief Selects the Alternate Output Functionality. To configure as IO use DComm_SetAsIOPin(pinNo, state)
 * @param pinNo DComm pin No (Range 1-16)
 * @param AlternateFunction Alternate Functionality to be used
 * @return *digital_pin_t pointer to the pin structure
 */
extern const digital_pin_t* BSP_DComm_SetPinAlternateFunction(uint32_t pinNo, uint32_t AlternateFunction);
/**
 * @brief Set the IO Port as Input. Pin 9 is always output
 */
extern void BSP_DComm_SetAsInputPort(void);
/**
 * @brief Set the IO Port as Output. Pin 9 is always output
 * @note Initial value for output pin is 0
 */
extern void BSP_DComm_SetAsOutputPort(void);
/*!
 * @brief Set the value of output port
 * @param val- value to be set
 */
extern void BSP_DComm_SetPortValue(uint32_t val);
/*!
 * @brief Gets the value of the input port
 * @returns val- value on the port
 */
extern uint32_t BSP_DComm_GetPortValue(void);
/**
 * @}
 */
/*******************************************************************************
 * Code
 ******************************************************************************/


#ifdef __cplusplus
}
#endif

/**
 * @}
 */

/**
 * @}
 */

/**
 * @}
 */
#endif

/* EOF */
