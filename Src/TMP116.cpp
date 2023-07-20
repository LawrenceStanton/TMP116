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

/**
 * @brief Convert a TMP116 Register temperature value to a float.
 *
 * @param registerValue The TMP116 register value.
 * @return float The equivalent temperature in degrees Celsius.
 */
static constexpr float convertTemperatureRegister(Register registerValue) {
	return static_cast<float>(static_cast<int16_t>(registerValue)) * TMP116_LSB_TEMPERATURE_RESOLUTION;
}

/**
 * @brief Convert a float temperature in degrees Celsius to a TMP116 Register value.
 *
 * @param temperature The temperature in degrees Celsius.
 * @return Register The TMP116 register equivalent value.
 */
static constexpr Register convertTemperatureRegister(float temperature) {
	Register result = static_cast<Register>(static_cast<int16_t>(temperature / TMP116_LSB_TEMPERATURE_RESOLUTION));
	return result;
}

float TMP116::getTemperature() const {
	auto registerValue = this->i2c->read(this->deviceAddress, TMP116_TEMP_REG_ADDR);
	if (registerValue) {
		return convertTemperatureRegister(registerValue.value());
	} else return -256.0f;
}

std::optional<Register> TMP116::getDeviceId() {
	return this->i2c->read(this->deviceAddress, TMP116_DEVICE_ID_REG_ADDR);
}

std::optional<Register> TMP116::setHighLimit(float temperature) const {
	Register registerValue = convertTemperatureRegister(temperature);
	return this->i2c->write(this->deviceAddress, TMP116_HIGH_LIM_REG_ADDR, registerValue);
}

std::optional<Register> TMP116::setLowLimit(float temperature) const {
	Register registerValue = convertTemperatureRegister(temperature);
	return this->i2c->write(this->deviceAddress, TMP116_LOW_LIM_REG_ADDR, registerValue);
}

TMP116::Config::Config(Register configRegister) {
	// Special case for Temperature Conversion Mode: Map 0b10 to 0b00 (both accepted by TMP116 but not by enum).
	if ((configRegister & 0x0C00u) == 0x0800u) { configRegister &= 0xF3FFu; }

	this->highAlertFlag	 = static_cast<bool>(configRegister & 0x8000u);
	this->lowAlertFlag	 = static_cast<bool>(configRegister & 0x4000u);
	this->dataReadyFlag	 = static_cast<bool>(configRegister & 0x2000u);
	this->eepromBusyFlag = static_cast<bool>(configRegister & 0x1000u);

	this->temperatureConversionMode = static_cast<TemperatureConversionMode>(configRegister & 0x0C00u);
	this->conversionCycleTime		= static_cast<ConversionCycleTime>(configRegister & 0x0380u);
	this->averages					= static_cast<Averages>(configRegister & 0x0060u);

	this->thermalAlertMode		  = static_cast<ThermalAlertModeSelect>(configRegister & 0x0010u);
	this->alertPolarity			  = static_cast<AlertPolarity>(configRegister & 0x0008u);
	this->dataReadyAlertSelection = static_cast<DataReadyAlertPinSelect>(configRegister & 0x0004u);
}

TMP116::Config::operator Register() const {
	Register registerValue = static_cast<Register>(this->highAlertFlag) << 15 |	 // bool must be shifted
							 static_cast<Register>(this->lowAlertFlag) << 14 |	 // bool must be shifted
							 static_cast<Register>(this->dataReadyFlag) << 13 |	 // bool must be shifted
							 static_cast<Register>(this->eepromBusyFlag) << 12 | // bool must be shifted

							 static_cast<Register>(this->temperatureConversionMode) | // typeof<enum class> == Register
							 static_cast<Register>(this->conversionCycleTime) |		  // typeof<enum class> == Register
							 static_cast<Register>(this->averages) |				  // typeof<enum class> == Register

							 static_cast<Register>(this->thermalAlertMode) |	   // typeof<enum class> == Register
							 static_cast<Register>(this->alertPolarity) |		   // typeof<enum class> == Register
							 static_cast<Register>(this->dataReadyAlertSelection); // typeof<enum class> == Register

	return registerValue;
}
