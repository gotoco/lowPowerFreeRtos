/*
 * MCP980x_drv_by_PG.cpp
 *
 *  Created on: 3 mar 2015
 *      Author: pgielmuda
 */

#ifdef _PG_DRV
#include "error.h"
#include "semphr.h"
#include "MCP980x_T.h"
#include "i2c_T.h"

#define MCP_MSB_ADDR 		MCP_DEF_ADDR
#define USR_ADDR( addr ) 	addr & 0x03


static xSemaphoreHandle _drvMCPSemaphore[8];


enum Error initMCP980x(uint8_t addr)
{
	enum Error error=i2cInitializeT(); //initialize I2C; //initialize I2C


	vSemaphoreCreateBinary(_drvMCPSemaphore[USR_ADDR(addr)]);

	if (_drvMCPSemaphore[USR_ADDR(addr)] == NULL)			// semaphore not created?
		return ERROR_FreeRTOS_errCOULD_NOT_ALLOCATE_REQUIRED_MEMORY;// return with error

	//check if there is a connection to MCP with this address
	char buff=CONF_REG;
	//read from CONF register
	error = i2cWriteToSlaveT(addr, &buff, 1, portMAX_DELAY );
	if(error != ERROR_NONE)
		return error;
	buff=0xff;
	error = i2cReadFromSlaveT(addr, &buff, 1,portMAX_DELAY );
	if(buff&0x80)//only 7th bit is certainly not zero
		return ERROR_CHIP_NOT_FOUND;

	return error;
}


enum Error readTemperatureMCP980x(float* pvTemp, uint8_t addr)
{
	//taking semaphore
	portBASE_TYPE ret=xSemaphoreTake(_drvMCPSemaphore[USR_ADDR(addr)], portMAX_DELAY);
	enum Error error = errorConvert_portBASE_TYPE(ret);
	RET_ERROR( error );

	//checking one-shot mode
	char confBuff[2]={CONF_REG,0x00};
	error = i2cWriteToSlaveT(addr,confBuff,1,portMAX_DELAY);
	RET_ERROR( error );
	error = i2cReadFromSlaveT(addr,(confBuff+1),1,portMAX_DELAY);
	RET_ERROR( error );


	if( confBuff[1] & 0x01){//one-shot mode it is
		confBuff[1]|=0x80;
		error = i2cWriteToSlaveT(addr,confBuff,2,portMAX_DELAY);//starting conversion
		RET_ERROR( error );
		while(confBuff[1] & 0x80){//waiting for a end of conversion
			error = i2cReadFromSlaveT(addr,(confBuff+1),1,portMAX_DELAY);
			RET_ERROR( error );
		}
	}

	//now we can read a tempareature
	char tempBuff[2]={TEMP_REG,0x00};
	error = i2cWriteToSlaveT(addr,tempBuff,1,portMAX_DELAY);
	RET_ERROR( error );
	error = i2cReadFromSlaveT(addr,tempBuff,2,portMAX_DELAY);
	RET_ERROR( error );

	//releasing semaphore
	xSemaphoreGive(_drvMCPSemaphore[USR_ADDR(addr)]);
	//converting temperature to float
	*pvTemp=(float)tempBuff[0]+(float)tempBuff[1]/256;
	return ERROR_NONE;

}


enum Error readConfRegisterMCP980x(uint8_t* pvConfRegValue, uint8_t addr)
{
	//taking semaphore
	portBASE_TYPE ret=xSemaphoreTake(_drvMCPSemaphore[USR_ADDR(addr)], portMAX_DELAY);
	enum Error error = errorConvert_portBASE_TYPE(ret);
	RET_ERROR( error );
	//reading conf register
	char buff=CONF_REG;
	error = i2cWriteToSlaveT(addr,&buff,1,portMAX_DELAY);
	RET_ERROR( error );
	error = i2cReadFromSlaveT(addr,&buff,1,portMAX_DELAY);
	RET_ERROR( error );
	//releasing semaphore
	xSemaphoreGive(_drvMCPSemaphore[USR_ADDR(addr)]);
	//returning value
	*pvConfRegValue=buff;
	return ERROR_NONE;


}

enum Error setConfRegisterMCP980x(uint8_t confRegValue, uint8_t addr)
{
	//taking semaphore
	portBASE_TYPE ret=xSemaphoreTake(_drvMCPSemaphore[USR_ADDR(addr)], portMAX_DELAY);
	enum Error error = errorConvert_portBASE_TYPE(ret);
	RET_ERROR( error );

	//setting conf register
	char buff[2]={CONF_REG,confRegValue};
	error = i2cWriteToSlaveT(addr,buff,2,portMAX_DELAY);
	RET_ERROR( error );

	//reading conf register
	buff[1]=0xff;
	error = i2cReadFromSlaveT(addr,(buff+1),1,portMAX_DELAY);
	RET_ERROR( error );

	//checking if everything is ok
	if(buff[1]!=confRegValue)
		return ERROR_CHIP_NOT_FOUND;

	//releasing semaphore
	xSemaphoreGive(_drvMCPSemaphore[USR_ADDR(addr)]);

	return ERROR_NONE;


}
#endif /* _PG_DRV */
