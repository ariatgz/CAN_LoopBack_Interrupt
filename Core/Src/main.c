/*
 * main.c
 *
 *  Created on: Dec 18, 2024
 *      Author: ariat
 */

#include "main.h"
#include"string.h"

void UART2_Init(void);
void Error_handler(void);
void GPIO_Init(void);
void CAN_Tx(void);
void CAN1_Init(void);
void SystemClock_Config_HSE(uint8_t clkFreq);
void CAN_Rx(void);
void CAN_Filter_Config(void);


UART_HandleTypeDef huart2;
CAN_HandleTypeDef hcan1;
uint32_t FLatency=0;
uint32_t mailbox;



int main(void){



	HAL_Init();
	//clock config should be second always
	SystemClock_Config_HSE(SYS_CLK_FREQ_50_MHZ);

	GPIO_Init();
	UART2_Init();
	CAN1_Init();

	CAN_Filter_Config();
	//this function enables all interrupt by turning their complementary bit on.
	HAL_CAN_ActivateNotification(&hcan1,CAN_IT_TX_MAILBOX_EMPTY |CAN_IT_RX_FIFO0_MSG_PENDING | CAN_IT_BUSOFF);
	if(HAL_CAN_Start(&hcan1) != HAL_OK){
		Error_handler();
	}



	CAN_Tx();
	CAN_Rx();


	while(1);



	return 0;
}

void CAN_Filter_Config(void){

	CAN_FilterTypeDef can_filter;

	can_filter.FilterActivation = ENABLE;
	can_filter.FilterBank = 0;
	can_filter.FilterFIFOAssignment = CAN_FILTER_FIFO0;
	can_filter.FilterIdHigh = 0x0;
	can_filter.FilterIdLow = 0x0;
	can_filter.FilterMaskIdHigh = 0x0;
	can_filter.FilterMaskIdLow = 0x0;
	can_filter.FilterMode = CAN_FILTERMODE_IDMASK;
	can_filter.FilterScale = CAN_FILTERSCALE_32BIT;

	if ( HAL_CAN_ConfigFilter(&hcan1, &can_filter) != HAL_OK){
		Error_handler();
	}

}

void CAN_Rx(void){
	CAN_RxHeaderTypeDef canRx;

	uint8_t rcvd_msg[5];
	char msg[50];

	while( ! HAL_CAN_GetRxFifoFillLevel(&hcan1, CAN_RX_FIFO0));

	if(HAL_CAN_GetRxMessage(&hcan1, CAN_RX_FIFO0, &canRx, rcvd_msg) != HAL_OK){
		Error_handler();
	}

	sprintf(msg,"Message Received\r\n", rcvd_msg);
	HAL_UART_Transmit(&huart2,(uint8_t *)msg, strlen(msg), HAL_MAX_DELAY);




}

void CAN_Tx(void){

	CAN_TxHeaderTypeDef canh;
	char msg[50];

	uint8_t our_msg[5] = {'H','E','L','L','O'};

	canh.DLC = 5;
	canh.StdId = 0x65d;
	canh.IDE = CAN_ID_STD;
	canh.RTR = CAN_RTR_DATA;
	if(HAL_CAN_AddTxMessage(&hcan1, &canh, our_msg, &mailbox)!= HAL_OK){
		Error_handler();
	}

	while(HAL_CAN_IsTxMessagePending(&hcan1, mailbox));


	sprintf(msg,"Message Transmitted\r\n");
	HAL_UART_Transmit(&huart2,(uint8_t *)msg, strlen(msg), HAL_MAX_DELAY);


}

void HAL_CAN_TxMailbox0CompleteCallback(CAN_HandleTypeDef *hcan){



}
void HAL_CAN_TxMailbox1CompleteCallback(CAN_HandleTypeDef *hcan){


}
void HAL_CAN_TxMailbox2CompleteCallback(CAN_HandleTypeDef *hcan){


}

void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan){


}
void HAL_CAN_ErrorCallback(CAN_HandleTypeDef *hcan){


}
void CAN1_Init(void){

	hcan1.Instance = CAN1;
	hcan1.Init.AutoBusOff =  DISABLE;
	hcan1.Init.Mode = CAN_MODE_LOOPBACK;
	hcan1.Init.AutoRetransmission = ENABLE;
	hcan1.Init.AutoWakeUp = DISABLE;
	hcan1.Init.ReceiveFifoLocked = DISABLE;
	hcan1.Init.TimeTriggeredMode = DISABLE;
	hcan1.Init.TransmitFifoPriority = DISABLE;

	hcan1.Init.Prescaler = 5;
	hcan1.Init.SyncJumpWidth = CAN_SJW_1TQ;
	hcan1.Init.TimeSeg1 = CAN_BS1_8TQ;
	hcan1.Init.TimeSeg2 = CAN_BS2_1TQ;

	if(HAL_CAN_Init(&hcan1)!= HAL_OK){
		Error_handler();
	}




}

void SystemClock_Config_HSE(uint8_t clkFreq){

	RCC_OscInitTypeDef osc_init;
	RCC_ClkInitTypeDef clk_init;

	osc_init.OscillatorType = RCC_OSCILLATORTYPE_HSE;
	osc_init.HSEState = RCC_HSE_BYPASS;
	osc_init.PLL.PLLState = RCC_PLL_ON;
	osc_init.PLL.PLLSource = RCC_PLLSOURCE_HSE;

	switch(clkFreq)
	{
	case SYS_CLK_FREQ_180_MHZ: {



				//enable clock for pwr controller

				__HAL_RCC_PWR_CLK_ENABLE();

				//set voltage scalar so we run on max clock
				__HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

				//turn on overdrive
				__HAL_PWR_OVERDRIVE_ENABLE();

		        osc_init.PLL.PLLM = 8;
				osc_init.PLL.PLLP = 1;
				osc_init.PLL.PLLN = 360;
				osc_init.PLL.PLLQ = 2;
				osc_init.PLL.PLLR = 2;

				clk_init.ClockType = RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
				clk_init.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;// look at the clock tree for more detail
				clk_init.AHBCLKDivider = RCC_SYSCLK_DIV1;
				clk_init.APB1CLKDivider = RCC_HCLK_DIV2;
				clk_init.APB2CLKDivider = RCC_HCLK_DIV2;
				FLatency = FLASH_ACR_LATENCY_5WS;



		break;
	}
	case SYS_CLK_FREQ_50_MHZ:{
		osc_init.PLL.PLLM = 8;
		osc_init.PLL.PLLP = 2;
		osc_init.PLL.PLLN = 100;
		osc_init.PLL.PLLQ = 2;
		osc_init.PLL.PLLR = 2;

		clk_init.ClockType = RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
		clk_init.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;// look at the clock tree for more detail
		clk_init.AHBCLKDivider = RCC_SYSCLK_DIV1;
		clk_init.APB1CLKDivider = RCC_HCLK_DIV2;
		clk_init.APB2CLKDivider = RCC_HCLK_DIV2;
		FLatency = FLASH_ACR_LATENCY_1WS;

		break;
	}

	case SYS_CLK_FREQ_84_MHZ:{

				osc_init.PLL.PLLM = 8;
				osc_init.PLL.PLLP = 2;
				osc_init.PLL.PLLN = 168;
				osc_init.PLL.PLLQ = 2;
				osc_init.PLL.PLLR = 2;


				clk_init.ClockType = RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
				clk_init.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;// look at the clock tree for more detail
				clk_init.AHBCLKDivider = RCC_SYSCLK_DIV1;
				clk_init.APB1CLKDivider = RCC_HCLK_DIV2;
				clk_init.APB2CLKDivider = RCC_HCLK_DIV2;
				FLatency = FLASH_ACR_LATENCY_2WS;

				break;

	}

	case SYS_CLK_FREQ_120_MHZ:{

		osc_init.PLL.PLLM = 8;
		osc_init.PLL.PLLP = 2;
		osc_init.PLL.PLLN = 240;
		osc_init.PLL.PLLQ = 2;
		osc_init.PLL.PLLR = 2;

		clk_init.ClockType = RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
		clk_init.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;// look at the clock tree for more detail
		clk_init.AHBCLKDivider = RCC_SYSCLK_DIV1;
		clk_init.APB1CLKDivider = RCC_HCLK_DIV4;
		clk_init.APB2CLKDivider = RCC_HCLK_DIV2;
		FLatency = FLASH_ACR_LATENCY_3WS;
				break;

	}
	default:
		return;

	}

	if(HAL_RCC_OscConfig(&osc_init) != HAL_OK){

		Error_handler();
	}
	if(HAL_RCC_ClockConfig(&clk_init, FLatency)!= HAL_OK){
		Error_handler();
	}

	//SYSTICK config
	HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);

	HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);


}

void GPIO_Init(void){

	__HAL_RCC_GPIOA_CLK_ENABLE();
	GPIO_InitTypeDef ledgpio;
	ledgpio.Pin = GPIO_PIN_5;
	ledgpio.Pull = GPIO_NOPULL;
	ledgpio.Mode = GPIO_MODE_OUTPUT_PP;
	HAL_GPIO_Init(GPIOA, &ledgpio);

}
void UART2_Init(){

	huart2.Instance = USART2;
	huart2.Init.BaudRate = 115200;
	huart2.Init.WordLength =  UART_WORDLENGTH_8B;
	huart2.Init.StopBits = UART_STOPBITS_1;
	huart2.Init.Parity = UART_PARITY_NONE;
	huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	huart2.Init.Mode = UART_MODE_TX_RX;
	if(HAL_UART_Init(&huart2)!= HAL_OK ){

		Error_handler();
	}
}

void Error_handler(void){

}



