/*
 * MCP980x_drv.h
 *
 *  Created on: 1 mar 2015
 *      Author: pgielmuda
 */

#ifndef PERIPHERALS_MCP980X_DRV_H_
#define PERIPHERALS_MCP980X_DRV_H_

#define TEMP_REG 0x00
#define CONF_REG 0x01
#define HYST_REG 0x02
#define LIMIT_REG 0x03
#define RET_ERROR( error )	if(error != ERROR_NONE) return error

#define MCP_MSB_ADDR 0x48
#define USR_ADDR( addr ) 	addr & 0x03


/** \brief Initialize MCP980x in continuous conversion mode
 * 	Initialize MCP980x in continuous conversion mode
 * 	\param[in] addr -I2C address of MCP980x
 * 	\return ERROR_NONE on success, otherwise an error code defined in the file error.h
 */
enum Error initMCP980x(uint8_t addr);

/** \brief Read temperature from MCP980x
 * 	Read last converted temperature or make a single measurement
 * 	\param[out] pvTemp -  pointer to a float where temperature will be written
 * 	\param[in] addr -I2C address of MCP980x
 * 	\return ERROR_NONE on success, otherwise an error code defined in the file error.h
 */
enum Error readTemperature(float* pvTemp, uint8_t addr);

/** \brief Read  conf register from MCP980x
 * 	Read conf register from MCP980x
 * 	\param[out] pvConfRegVAlue -  pointer to a uint_8 where conf register value will be written
 * 	\param[in] addr -I2C address of MCP980x
 * 	\return ERROR_NONE on success, otherwise an error code defined in the file error.h
 */
enum Error readConfRegister(uint8_t* pvConfRegValue, uint8_t addr);

/** 	Set conf register to MCP980x
* 	\param[in] confRegVAlue - uint_8 value to write into conf register
* 	(all bits of configuration register are descibed in datasheet)
* 	\param[in] addr -I2C address of MCP980x
* 	\return ERROR_NONE on success, otherwise an error code defined in the file error.h
*/
enum Error setConfRegister(uint8_t confRegValue, uint8_t addr);
enum Error readHyst(uint16_t* pvHystValue, uint8_t addr);
enum Error setHyst(uint16_t hystValue, uint8_t addr);





#endif /* PERIPHERALS_MCP980X_DRV_H_ */
