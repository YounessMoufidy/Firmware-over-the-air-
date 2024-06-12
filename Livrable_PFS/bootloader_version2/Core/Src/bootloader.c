/*
 * bootloader.c
 *
 *  Created on: Apr 18, 2024
 *      Author: pc
 */


#include "bootloader.h"

static uint32_t BL_CRC_verify(uint8_t *pdata,uint32_t data_length,uint32_t HOST_CRC);
uint8_t Perform_Flash_Erase(uint32_t PageAddress,uint8_t page_Number);
static uint8_t BL_Adddress_Verification(uint32_t Address);
static uint8_t FLASH_Memory_Payload_Write(uint16_t *pdata,uint32_t StartAddress,uint8_t Payload_data_length);




static void BL_sendACK(uint8_t data_length);
static void BL_sendNACK(void);
static void BL_GET_Version(uint8_t * HOST_Buffer);
static void BL_GET_Help(uint8_t * HOST_Buffer);
static void BL_FLASH_ERASE(uint8_t * HOST_Buffer);
static void BL_Write_Data(uint8_t * HOST_Buffer);
void Bootloader_Jump_To_User_App(void);
static void BL_GET_Chip_Identification_Number(uint8_t * HOST_Buffer);

static uint8_t HOST_Buffer[HOSTM_MAX_SIZE];//The array that stores tha data

void BL_SendMessage(char * format,...)
{
	char message[100]={0};
	va_list args;
	va_start(args,format);
	vsprintf(message,format,args);
	HAL_UART_Transmit(&huart1 ,(uint8_t*)message,sizeof(message),HAL_MAX_DELAY);
	va_end(args);
}


BL_status BL_FeatchHostCommand(void)
{
	BL_status status=BL_NACK;
	HAL_StatusTypeDef HAL_status=HAL_ERROR;
	uint8_t data_length=0;
	memset(HOST_Buffer,0,HOSTM_MAX_SIZE);
	HAL_status=	HAL_UART_Receive(&huart1 ,HOST_Buffer,1,HAL_MAX_DELAY);//TO receive the length first
	if(HAL_status!=HAL_OK)
	{
		status=BL_NACK;
	}
	else{
		data_length=HOST_Buffer[0];
		HAL_status=	HAL_UART_Receive(&huart1 ,&HOST_Buffer[1],data_length,HAL_MAX_DELAY);//TO receive the command
		if(HAL_status!=HAL_OK)
			{
				status=BL_NACK;
			}
		else{
			switch(HOST_Buffer[1])
			{

				case CBL_GET_VER_CMD:BL_GET_Version(HOST_Buffer);break;
				case CBL_GET_HELP_CMD:BL_GET_Help(HOST_Buffer);break;
				case CBL_GET_CID_CMD:BL_GET_Chip_Identification_Number(HOST_Buffer);break;

				case CBL_GO_TO_ADDR_CMD:Bootloader_Jump_To_User_App();break;
				case CBL_FLASH_ERASE_CMD:BL_FLASH_ERASE(HOST_Buffer);break;
				case CBL_MEM_WRITE_CMD:BL_Write_Data(HOST_Buffer);break;

				default:status=BL_NACK;

			}


		}




	}
	return status;



}

static uint32_t BL_CRC_verify(uint8_t *pdata,uint32_t data_length,uint32_t HOST_CRC)
{
	uint8_t CRC_status=CRC_VERIFING_FAILED;
	uint32_t MCU_CRC=0;
	uint32_t data_buffer=0;
	/*uint32_t HAL_CRC_Accumulate(CRC_HandleTypeDef *hcrc, uint32_t pBuffer[], uint32_t BufferLength)*/
	for(uint8_t count=0;count<data_length;count++)
	{
		data_buffer=(uint32_t)pdata[count];
		MCU_CRC=HAL_CRC_Accumulate(&hcrc,&data_buffer,1);
	}
	__HAL_CRC_DR_RESET(&hcrc);
	if(HOST_CRC==MCU_CRC){
		CRC_status=CRC_VERIFING_PASSED;
	}
	else{
		CRC_status=CRC_VERIFING_FAILED;
	}
	return CRC_status;

}

uint8_t Perform_Flash_Erase(uint32_t PageAddress,uint8_t page_Number)
{
	FLASH_EraseInitTypeDef pEraseInit;
	uint32_t PageError=0;
	HAL_StatusTypeDef HAL_status=HAL_ERROR;
	uint8_t Pagestatus=INVALID_PAGE_NUMBER;
	if(page_Number>CBL_FLASH_MAX_PAGE_NUMBER)
	{
		Pagestatus=INVALID_PAGE_NUMBER;

	}
	else{
		Pagestatus=VALID_PAGE_NUMBER;
		if(page_Number<=(CBL_FLASH_MAX_PAGE_NUMBER-1)||PageAddress==CBL_FLASH_MASS_ERASE)
		{
			if(PageAddress==CBL_FLASH_MASS_ERASE)
			{

				pEraseInit.TypeErase=FLASH_TYPEERASE_PAGES;

				pEraseInit.Banks=FLASH_BANK_1;

				pEraseInit.PageAddress=0x8002900;//we erase starting from the app address

				pEraseInit.NbPages=12;


			}
			else{
				pEraseInit.TypeErase=FLASH_TYPEERASE_PAGES;

				pEraseInit.Banks=FLASH_BANK_1;

				pEraseInit.PageAddress=PageAddress;

				pEraseInit.NbPages=page_Number;

			}
			HAL_FLASH_Unlock();
			HAL_status=HAL_FLASHEx_Erase(&pEraseInit,&PageError);
			HAL_FLASH_Lock();
			if(PageError==HAL_SUCCESSFUL_ERASE)
			{
				Pagestatus=SUCCESSFUL_ERASE;
			}
			else{
				Pagestatus=UNSUCCESSFUL_ERASE;

			}

		}
		else{
			Pagestatus=INVALID_PAGE_NUMBER;

		}
	}
	return 	Pagestatus;


}

static uint8_t BL_Adddress_Verification(uint32_t Address)
{
	uint8_t Address_verify=ADDRESS_IS_INVALID;

	if(Address>=FLASH_BASE &&Address<=STM32F103_FLASH_END )
	{
		 Address_verify=ADDRESS_IS_VALID;
	}
	else if(Address>=SRAM_BASE &&Address<=STM32F103_SRAM_END )
	{
		 Address_verify=ADDRESS_IS_VALID;

	}
	else{
		 Address_verify=ADDRESS_IS_INVALID;
	}
	return Address_verify;
}

static uint8_t FLASH_Memory_Payload_Write(uint16_t *pdata,uint32_t StartAddress,uint8_t Payload_data_length)
{
	uint32_t Address=0;
	uint8_t UpdateAddress=0;
	HAL_StatusTypeDef Hal_status=HAL_ERROR;
	uint8_t payload_status=FLASH_PAYLOAD_WRITE_FAILED;
	HAL_FLASH_Unlock();

	for(uint8_t payload_count=0,UpdateAddress=0;payload_count<Payload_data_length/2;payload_count++,UpdateAddress+=2)
	{
		Address=StartAddress+UpdateAddress;
		Hal_status=HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD,Address,pdata[payload_count]);
		if(Hal_status!=HAL_OK)
		{
			payload_status=FLASH_PAYLOAD_WRITE_FAILED;
		}
		else{
			payload_status=FLASH_PAYLOAD_WRITE_PASSED;
		}
	}



	HAL_FLASH_Lock();

	return payload_status;

}






static void BL_sendACK(uint8_t data_length)
{
	uint8_t ACK_value[2]={0};
	ACK_value[0]=SEND_ACK;
	ACK_value[1]=data_length;
	HAL_UART_Transmit(&huart1 ,(uint8_t*)ACK_value,2,HAL_MAX_DELAY);//TO receive the command


}
static void BL_sendNACK(void)
{
	uint8_t ACK_value=SEND_NACK;
	HAL_UART_Transmit(&huart1 ,&ACK_value,sizeof(ACK_value),HAL_MAX_DELAY);//TO receive the command

}

static void BL_GET_Version(uint8_t * HOST_Buffer)
{
	uint8_t version[4]={CBL_VENDOR_ID,CBL_SW_MAJOR_VERSION,CBL_SW_MINOR_VERSION,CBL_SW_PATCH_VERSION};
	uint16_t HOST_Packet_length=0;
	uint32_t CRC_value=0;

	HOST_Packet_length=HOST_Buffer[0]+1;
	CRC_value=*((uint32_t*)(HOST_Buffer+HOST_Packet_length-4));
	if(CRC_VERIFING_PASSED==BL_CRC_verify((uint8_t *)&HOST_Buffer[0],HOST_Packet_length-4,CRC_value))
	{
		BL_sendACK(4);
		HAL_UART_Transmit(&huart1 ,(uint8_t*)version,4,HAL_MAX_DELAY);//TO receive the command

	}
	else{
		BL_sendNACK();
	}

}

static void BL_GET_Help(uint8_t * HOST_Buffer)
{
		uint8_t BL_Supported_CMS[6]={CBL_GET_VER_CMD,CBL_GET_HELP_CMD,CBL_GET_CID_CMD,CBL_GO_TO_ADDR_CMD,CBL_FLASH_ERASE_CMD,CBL_MEM_WRITE_CMD};
		uint16_t HOST_Packet_length=0;
		uint32_t CRC_value=0;

		HOST_Packet_length=HOST_Buffer[0]+1;
		CRC_value=*((uint32_t*)(HOST_Buffer+HOST_Packet_length-4));
		if(CRC_VERIFING_PASSED==BL_CRC_verify((uint8_t *)&HOST_Buffer[0],HOST_Packet_length-4,CRC_value))
		{
			BL_sendACK(6);
			HAL_UART_Transmit(&huart1 ,(uint8_t*)BL_Supported_CMS,6,HAL_MAX_DELAY);//TO receive the command

		}
		else{
			BL_sendNACK();
		}

}

static void BL_GET_Chip_Identification_Number(uint8_t * HOST_Buffer)
{
	uint16_t Chip_ID=0;
	uint16_t HOST_Packet_length=0;
	uint32_t CRC_value=0;

	HOST_Packet_length=HOST_Buffer[0]+1;
	CRC_value=*((uint32_t*)(HOST_Buffer+HOST_Packet_length-4));
	if(CRC_VERIFING_PASSED==BL_CRC_verify((uint8_t *)&HOST_Buffer[0],HOST_Packet_length-4,CRC_value))
	{
		Chip_ID=(uint16_t)(DBGMCU->IDCODE &0x00000FFF);
		BL_sendACK(2);
		HAL_UART_Transmit(&huart1 ,(uint8_t*)&Chip_ID,2,HAL_MAX_DELAY);//TO receive the command
	}
	else{
		BL_sendNACK();
	}


}


static void BL_FLASH_ERASE(uint8_t * HOST_Buffer)
{
		uint8_t Erase_status=UNSUCCESSFUL_ERASE;
		uint16_t HOST_Packet_length=0;
		uint32_t CRC_value=0;

		HOST_Packet_length=HOST_Buffer[0]+1;
		CRC_value=*((uint32_t*)(HOST_Buffer+HOST_Packet_length-4));
		if(CRC_VERIFING_PASSED==BL_CRC_verify((uint8_t *)&HOST_Buffer[0],HOST_Packet_length-4,CRC_value))
		{
			Erase_status=Perform_Flash_Erase(*((uint32_t*)&HOST_Buffer[7]),HOST_Buffer[6]);
			BL_sendACK(1);
			HAL_UART_Transmit(&huart1 ,(uint8_t*)&Erase_status,1,HAL_MAX_DELAY);//TO receive the command
		}
		else{
			BL_sendNACK();
		}


}

uint16_t i=0;
static void BL_Write_Data(uint8_t * HOST_Buffer)
{
			uint8_t Address_verify=ADDRESS_IS_INVALID;
			uint32_t Address_HOST=0;
			uint8_t Data_length=0;
			uint8_t payload_status=FLASH_PAYLOAD_WRITE_FAILED;
			uint16_t HOST_Packet_length=0;
			uint32_t CRC_value=0;

			HOST_Packet_length=HOST_Buffer[0]+1;
			CRC_value=*((uint32_t*)(HOST_Buffer+HOST_Packet_length-4));
			if(CRC_VERIFING_PASSED==BL_CRC_verify((uint8_t *)&HOST_Buffer[0],HOST_Packet_length-4,CRC_value))
			{
				//BL_sendACK(1);
				/*Extract the start address from the host address*/
				Address_HOST=*((uint32_t*)&HOST_Buffer[2])+64*i;
				i++;
				/* Extract the payload length from the Host packet because the address is in 4 bytes*/
				Data_length=HOST_Buffer[6];
				Address_verify=BL_Adddress_Verification(Address_HOST);
				if(Address_verify==ADDRESS_IS_VALID)
				{
					//flash
					payload_status=FLASH_Memory_Payload_Write((uint16_t*)&HOST_Buffer[7],Address_HOST,Data_length);
					HAL_UART_Transmit(&huart1 ,(uint8_t*)&payload_status,1,HAL_MAX_DELAY);//TO receive the command

				}
				else{
					HAL_UART_Transmit(&huart1 ,(uint8_t*)&Address_verify,1,HAL_MAX_DELAY);//TO receive the command

				}
			}

			else{
				BL_sendNACK();
			}

}
/***********************JUMP TO USER APPLICATION***********************/
void Bootloader_Jump_To_User_App(void)
{
		uint32_t MSP_Value = *((volatile uint32_t*)FLASH_SECTOR2_BASE_ADDRESS);
		uint32_t MainAppAdd = *((volatile uint32_t*)(FLASH_SECTOR2_BASE_ADDRESS+4));

		pMainApp ResetHandler_Address=(pMainApp)MainAppAdd;
		HAL_RCC_DeInit();
		__set_MSP(MSP_Value);
		ResetHandler_Address();
}

