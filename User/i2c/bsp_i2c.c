#include "./i2c/bsp_i2c.h"

  
I2C_HandleTypeDef I2C_Handle;					
/*******************************  Function ************************************/

/**
  * @brief  ��ʼ��I2C���ߣ�ʹ��I2Cǰ��Ҫ����
  * @param  ��
  * @retval ��
  */
void I2CMaster_Init(void) 
{
	GPIO_InitTypeDef GPIO_InitStructure;

	/* ʹ��I2Cxʱ�� */
	SENSORS_I2C_RCC_CLK_ENABLE();

	/* ʹ��I2C GPIO ʱ�� */
	SENSORS_I2C_SCL_GPIO_CLK_ENABLE();
	SENSORS_I2C_SDA_GPIO_CLK_ENABLE();

	/* ����I2Cx����: SCL ----------------------------------------*/
	GPIO_InitStructure.Pin =  SENSORS_I2C_SCL_GPIO_PIN; 
	GPIO_InitStructure.Mode = GPIO_MODE_AF_OD;
	GPIO_InitStructure.Speed = GPIO_SPEED_HIGH;
	GPIO_InitStructure.Pull= GPIO_NOPULL;
	GPIO_InitStructure.Alternate=SENSORS_I2C_AF; 
	HAL_GPIO_Init(SENSORS_I2C_SCL_GPIO_PORT, &GPIO_InitStructure);

	/* ����I2Cx����: SDA ----------------------------------------*/
	GPIO_InitStructure.Pin = SENSORS_I2C_SDA_GPIO_PIN;  
	HAL_GPIO_Init(SENSORS_I2C_SDA_GPIO_PORT, &GPIO_InitStructure); 
	
	if(HAL_I2C_GetState(&I2C_Handle) == HAL_I2C_STATE_RESET)
	{	
		/* ǿ�Ƹ�λI2C����ʱ�� */  
		SENSORS_I2C_FORCE_RESET(); 

		/* �ͷ�I2C����ʱ�Ӹ�λ */  
		SENSORS_I2C_RELEASE_RESET(); 
		
    	/* I2C ���� */
		I2C_Handle.Instance = SENSORS_I2C;
    I2C_Handle.Init.AddressingMode  = I2C_ADDRESSINGMODE_7BIT;
    I2C_Handle.Init.ClockSpeed      = 400000;
    I2C_Handle.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
    I2C_Handle.Init.DutyCycle       = I2C_DUTYCYCLE_2;
    I2C_Handle.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
    I2C_Handle.Init.NoStretchMode   = I2C_NOSTRETCH_DISABLE;
    I2C_Handle.Init.OwnAddress1     = 0 ;
    I2C_Handle.Init.OwnAddress2     = 0; 
   
		/* ��ʼ��I2C */
		HAL_I2C_Init(&I2C_Handle);	
		/* ʹ��ģ���˲��� */
		HAL_I2CEx_AnalogFilter_Config(&I2C_Handle, I2C_ANALOGFILTER_ENABLE); 
	}
}
/**
  * @brief  Manages error callback by re-initializing I2C.
  * @param  Addr: I2C Address
  * @retval None
  */
static void I2Cx_Error(void)
{
	/* �ָ�I2C�Ĵ���ΪĬ��ֵ */
	HAL_I2C_DeInit(&I2C_Handle); 
	/* ���³�ʼ��I2C���� */
	I2CMaster_Init();
}
/**
  * @brief  дһ�ֽ����ݵ�OV5640�Ĵ���
  * @param  Addr: OV5640 �ļĴ�����ַ
  * @param  Data: Ҫд�������
  * @retval ����0��ʾд��������0xFF��ʾ����
  */
uint8_t OV5640_WriteReg(uint16_t Addr, uint8_t Data)
{
  HAL_StatusTypeDef status = HAL_OK;
  
  status = HAL_I2C_Mem_Write(&I2C_Handle, OV5640_DEVICE_ADDRESS, (uint16_t)Addr, I2C_MEMADD_SIZE_16BIT, (uint8_t*)&Data, 1, 1000);
  
  /* Check the communication status */
  if(status != HAL_OK)
  {
    /* Re-Initiaize the I2C Bus */
    I2Cx_Error();
  }
  return status;
}
/**
  * @brief  ��OV5640�Ĵ����ж�ȡһ���ֽڵ�����
  * @param  Addr: �Ĵ�����ַ
  * @retval ���ض�ȡ�õ�����
  */
uint8_t OV5640_ReadReg(uint16_t Addr)
{
  uint8_t Data = 0;
 
  HAL_StatusTypeDef status = HAL_OK;

  status = HAL_I2C_Mem_Read(&I2C_Handle, OV5640_DEVICE_ADDRESS, (uint16_t)Addr, I2C_MEMADD_SIZE_16BIT, (uint8_t*)&Data, 1, 1000);

  /* ���ͨ��״̬ */
  if(status != HAL_OK)
  {
    /* �����������³�ʼ��I2C */
    I2Cx_Error();
  }
  /* ���ض��������� */
    return Data;
}


/**
  * @brief  ���̼�д�뵽OV5640Ƭ��MCU
  * @param  Addr: OV5640 ��MCU����ַ0x8000
  * @param  Data: Ҫд�������
  * @retval ����0��ʾд��������0xFF��ʾ����
  */
uint8_t OV5640_WriteFW(uint8_t *pBuffer ,uint16_t BufferSize)
{
  uint16_t Addr=0x8000;
  HAL_StatusTypeDef status = HAL_OK;
  
  status = HAL_I2C_Mem_Write(&I2C_Handle, OV5640_DEVICE_ADDRESS, (uint16_t)Addr, I2C_MEMADD_SIZE_16BIT, pBuffer, BufferSize, 1000);
  
  /* ���ͨ��״̬ */
  if(status != HAL_OK)
  {
    /* �����������³�ʼ��I2C */
    I2Cx_Error();
  }
  return status;
}
