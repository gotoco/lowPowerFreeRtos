/*
 * MCP980x_drv_by_PG.cpp
 *
 *  Created on: 3 mar 2015
 *      Author: pgielmuda
 */

#include "error.h"
#include "semphr.h"
#include "MCP980x_drv_by_PG.h"
#include "i2c_by_PG.h"


static xSemaphoreHandle _drvMCPSemaphore[8];


enum Error initMCP980x(uint8_t addr)
{
	enum Error error=i2cInitialize(); //initialize I2C


	vSemaphoreCreateBinary(_drvMCPSemaphore[USR_ADDR(addr)]);

	if (_drvMCPSemaphore[USR_ADDR(addr)] == NULL)			// semaphore not created?
		return ERROR_FreeRTOS_errCOULD_NOT_ALLOCATE_REQUIRED_MEMORY;// return with error

	//check if there is a connection to MCP with this address
	char buff=CONF_REG;
	//read from CONF register
	error = i2cWriteToSlave(addr, &buff, 1, portMAX_DELAY );
	RET_ERROR( error );
	buff=0xff;
	error = i2cReadFromSlave(addr, &buff, 1,portMAX_DELAY );
	if(buff&0x80)//only 7th bit is certainly not zero
		return ERROR_MCP;

	return error;
}


enum Error readTemperature(float* pvTemp, uint8_t addr)
{
	//taking semaphore
	portBASE_TYPE ret=xSemaphoreTake(_drvMCPSemaphore[USR_ADDR(addr)], portMAX_DELAY);
	enum Error error = errorConvert_portBASE_TYPE(ret);
	RET_ERROR( error );

	//checking one-shot mode
	char confBuff[2]={CONF_REG,0x00};
	error = i2cWriteToSlave(addr,confBuff,1,portMAX_DELAY);
	RET_ERROR( error );
	error = i2cReadFromSlave(addr,(confBuff+1),1,portMAX_DELAY);
	RET_ERROR( error );


	if( confBuff[1] & 0x01){//one-shot mode it is
		confBuff[1]|=0x80;
		error = i2cWriteToSlave(addr,confBuff,2,portMAX_DELAY);//starting conversion
		RET_ERROR( error );
		while(confBuff[1] & 0x80){//waiting for a end of conversion
			error = i2cReadFromSlave(addr,(confBuff+1),1,portMAX_DELAY);
			RET_ERROR( error );
		}
	}

	//now we can read a tempareature
	char tempBuff[2]={TEMP_REG,0x00};
	error = i2cWriteToSlave(addr,tempBuff,1,portMAX_DELAY);
	RET_ERROR( error );
	error = i2cReadFromSlave(addr,tempBuff,2,portMAX_DELAY);
	RET_ERROR( error );

	//releasing semaphore
	xSemaphoreGive(_drvMCPSemaphore[USR_ADDR(addr)]);
	//converting temperature to float
	*pvTemp=(float)tempBuff[0]+(float)tempBuff[1]/256;
	return ERROR_NONE;

}


enum Error readConfRegister(uint8_t* pvConfRegValue, uint8_t addr)
{
	//taking semaphore
	portBASE_TYPE ret=xSemaphoreTake(_drvMCPSemaphore[USR_ADDR(addr)], portMAX_DELAY);
	enum Error error = errorConvert_portBASE_TYPE(ret);
	RET_ERROR( error );
	//reading conf register
	char buff=CONF_REG;
	error = i2cWriteToSlave(addr,&buff,1,portMAX_DELAY);
	RET_ERROR( error );
	error = i2cReadFromSlave(addr,&buff,1,portMAX_DELAY);
	RET_ERROR( error );
	//releasing semaphore
	xSemaphoreGive(_drvMCPSemaphore[USR_ADDR(addr)]);
	//returning value
	*pvConfRegValue=buff;
	return ERROR_NONE;


}

enum Error setConfRegister(uint8_t confRegValue, uint8_t addr)
{
	//taking semaphore
	portBASE_TYPE ret=xSemaphoreTake(_drvMCPSemaphore[USR_ADDR(addr)], portMAX_DELAY);
	enum Error error = errorConvert_portBASE_TYPE(ret);
	RET_ERROR( error );

	//setting conf register
	char buff[2]={CONF_REG,confRegValue};
	error = i2cWriteToSlave(addr,buff,2,portMAX_DELAY);
	RET_ERROR( error );

	//reading conf register
	buff[1]=0xff;
	error = i2cReadFromSlave(addr,(buff+1),1,portMAX_DELAY);
	RET_ERROR( error );

	//checking if everything is ok
	if(buff[1]!=confRegValue)
		return ERROR_MCP;

	//releasing semaphore
	xSemaphoreGive(_drvMCPSemaphore[USR_ADDR(addr)]);

	return ERROR_NONE;


}

