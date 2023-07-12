/**
 ******************************************************************************
 * @file			: TMP116.hpp
 * @brief			: TMP116 Temperature Sensor Driver
 * @author			: Lawrence Stanton
 ******************************************************************************
 */

#pragma once

#include <cstdint>
#include <optional>

class TMP116 {
public:
	/**
	 * @brief I2C Interface
	 *
	 * @details This interface is used to abstract the I2C communication for the TMP116.
	 * The TMP116 class will use a pointer to this interface to communicate with the I2C peripheral.
	 * The user of the TMP116 class will need to provide a concrete implementation of this interface and provide it in
	 * the TMP116 constructor.
	 * @note This interface is also used to mock the I2C peripheral for unit testing.
	 */
	class I2C {
	public:
		typedef uint8_t	 MemoryAddress; // TMP116 Internal Memory Address
		typedef uint16_t Register;		// TMP116 Internal Register

		/**
		 * @brief Device Address of the TMP116 on the I2C Bus.
		 * @note These address values are right aligned and should be left shifted by 1 the LSB R/W bit should be added
		 * in the I2C call.
		 */
		enum class DeviceAddress {
			ADD0_GND = 0x48,
			ADD0_VCC = 0x49,
			ADD0_SDA = 0x4A,
			ADD0_SCL = 0x4B,
		};

		/**
		 * @brief Read a register from the TMP116.
		 *
		 * @param deviceAddress	Device Address of the TMP116 on the I2C Bus.
		 * @param memoryAddress
		 * @return std::optional<Register>
		 */
		virtual std::optional<Register> read(DeviceAddress deviceAddress, MemoryAddress memoryAddress) = 0;

		virtual std::optional<Register>
		write(DeviceAddress deviceAddress, MemoryAddress memoryAddress, Register data) = 0;

		virtual ~I2C() = default;
	};

	using DeviceAddress = I2C::DeviceAddress;
	using MemoryAddress = I2C::MemoryAddress;
	using Register		= I2C::Register;

	enum class Averages {
		AVG_1  = 0x00,
		AVG_8  = 0x01,
		AVG_32 = 0x02,
		AVG_64 = 0x03,
	};

	/**
	 * @brief Construct a new TMP116 object
	 *
	 * @param i2c I2C Interface. @see TMP116::I2C for details.
	 * @param deviceAddress Device Address of the TMP116 on the I2C Bus to address.
	 */
	TMP116(I2C *i2c, I2C::DeviceAddress deviceAddress);

	/**
	 * @brief Get the Temperature from the TMP116.
	 *
	 * @return float The temperature in degrees Celsius.
	 */
	float getTemperature() const;

	/**
	 * @brief Convert a TMP116 Register temperature value to a float.
	 *
	 * @param registerValue The TMP116 register value.
	 * @return float The equivalent temperature in degrees Celsius.
	 */
	static float convertTemperatureRegister(Register registerValue);

	/**
	 * @brief Convert a float temperature in degrees Celsius to a TMP116 Register value.
	 *
	 * @param temperature The temperature in degrees Celsius.
	 * @return Register The TMP116 register equivalent value.
	 */
	static Register convertTemperatureRegister(float temperature);

	/**
	 * @brief Get the Device ID of the TMP116.
	 *
	 * @return std::optional<Register> The Device ID if successful.
	 * @note The Device ID should always be 0x1116u.
	 */
	std::optional<Register> getDeviceId();

	/**
	 * @brief Set the High Limit threshold for the TMP116.
	 *
	 * @param temperature The temperature limit in degrees Celsius.
	 * @return std::optional<Register> The register value written to the TMP116 high limit register if successful.
	 */
	std::optional<Register> setHighLimit(float temperature) const;

	/**
	 * @brief Set the Low Limit threshold for the TMP116.
	 *
	 * @param temperature The temperature limit in degrees Celsius.
	 * @return std::optional<Register> The register value written to the TMP116 low limit register if successful.
	 */
	std::optional<Register> setLowLimit(float temperature) const;

	inline DeviceAddress getDeviceAddress() const { return deviceAddress; }
	inline void			 setDeviceAddress(DeviceAddress deviceAddress) { this->deviceAddress = deviceAddress; }

private:
	I2C			 *i2c;
	DeviceAddress deviceAddress;
};
