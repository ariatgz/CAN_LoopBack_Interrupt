/*
 * it.c
 *
 *  Created on: Dec 18, 2024
 *      Author: ariat
 */
#include "main.h"

extern UART_HandleTypeDef huart2;
extern CAN_HandleTypeDef hcan1;

void SysTick_Handler(void){

		HAL_IncTick();
		HAL_SYSTICK_IRQHandler();
}
 void CAN1_TX_IRQHandler(void){

	 HAL_CAN_IRQHandler(&hcan1);

}
 void CAN1_RX0_IRQHandler(void){

	 HAL_CAN_IRQHandler(&hcan1);

 }
 void CAN1_RX1_IRQHandler(void){
	 HAL_CAN_IRQHandler(&hcan1);

 }
 void CAN1_SCE_IRQHandler(void){

	 HAL_CAN_IRQHandler(&hcan1);

 }


