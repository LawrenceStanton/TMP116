/**
 ******************************************************************************
 * @file			: TMP116.cpp
 * @brief			: Source for TMP116.hpp
 * @author			: Lawrence Stanton
 ******************************************************************************
 */

#include "TMP116.hpp"

using DeviceAddress = TMP116::I2C::DeviceAddress;
using MemoryAddress = TMP116::I2C::MemoryAddress;
using Register		= TMP116::I2C::Register;

#define TMP116_TEMP_REG_ADDR	  static_cast<MemoryAddress>(0x00u) // Temperature Register Address
#define TMP116_CFGR_REG_ADDR	  static_cast<MemoryAddress>(0x01u) // Configuration Register Address
#define TMP116_HIGH_LIM_REG_ADDR  static_cast<MemoryAddress>(0x02u) // High Limit Register Address
#define TMP116_LOW_LIM_REG_ADDR	  static_cast<MemoryAddress>(0x03u) // Low Limit Register Address
#define TMP116_EEPROM_UL_REG_ADDR static_cast<MemoryAddress>(0x04u) // EEPROM Unlock Register Address
#define TMP116_EEPROM1_REG_ADDR	  static_cast<MemoryAddress>(0x05u) // EEPROM 1 Register Address
#define TMP116_EEPROM2_REG_ADDR	  static_cast<MemoryAddress>(0x06u) // EEPROM 2 Register Address
#define TMP116_EEPROM3_REG_ADDR	  static_cast<MemoryAddress>(0x07u) // EEPROM 3 Register Address
#define TMP116_EEPROM4_REG_ADDR	  static_cast<MemoryAddress>(0x08u) // EEPROM 4 Register Address
#define TMP116_DEVICE_ID_REG_ADDR static_cast<MemoryAddress>(0x0Fu) // Device ID Register Address

#define TMP116_LSB_TEMPERATURE_RESOLUTION 0.0078125f // 0.0078125 milli-degrees Celsius per LSB

TMP116::TMP116(I2C *i2c, I2C::DeviceAddress deviceAddress) : i2c(i2c), deviceAddress(deviceAddress) {}

float TMP116::convertTemperatureRegister(Register registerValue) {
	return static_cast<float>(static_cast<int16_t>(registerValue)) * TMP116_LSB_TEMPERATURE_RESOLUTION;
}

Register TMP116::convertTemperatureRegister(float temperature) {
	return static_cast<Register>(temperature / TMP116_LSB_TEMPERATURE_RESOLUTION);
}

float TMP116::getTemperature() const {
	auto registerValue = this->i2c->read(this->deviceAddress, TMP116_TEMP_REG_ADDR);
	if (registerValue) {
		return TMP116::convertTemperatureRegister(registerValue.value());
	} else return -256.0f;
}

std::optional<Register> TMP116::getDeviceId() {
	return this->i2c->read(this->deviceAddress, TMP116_DEVICE_ID_REG_ADDR);
}

std::optional<Register> TMP116::setHighLimit(float temperature) const {
	Register registerValue = TMP116::convertTemperatureRegister(temperature);
	return this->i2c->write(this->deviceAddress, TMP116_HIGH_LIM_REG_ADDR, registerValue);
}

std::optional<Register> TMP116::setLowLimit(float temperature) const {
	Register registerValue = TMP116::convertTemperatureRegister(temperature);
	return this->i2c->write(this->deviceAddress, TMP116_LOW_LIM_REG_ADDR, registerValue);
}
