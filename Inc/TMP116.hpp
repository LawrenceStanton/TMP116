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
	 * @brief Get the Device ID of the TMP116.
	 *
	 * @return std::optional<Register> The Device ID if successful.
	 * @note The Device ID should always be 0x1116u.
	 */
	std::optional<Register> getDeviceId();

	struct Config {
		enum class Averages : Register {
			AVG_1  = 0x0000u,
			AVG_8  = 0x0020u,
			AVG_32 = 0x0040u,
			AVG_64 = 0x0060u,
		};

		enum class TemperatureConversionMode : Register {
			CONTINUOUS = 0x0000u,
			SHUTDOWN   = 0x0400u,
			ONESHOT	   = 0x0C00u,
		};

		enum class ConversionCycleTime : Register {
			CONV_15_5MS	 = 0b000u << 7, // ! Period increased when Averages > 1
			CONV_125MS	 = 0b001u << 7, // ! Period increased when Averages > 8
			CONV_250MS	 = 0b010u << 7, // ! Period increased when Averages > 8
			CONV_500MS	 = 0b011u << 7, // ! Period increased when Averages > 32
			CONV_1000MS	 = 0b100u << 7,
			CONV_4000MS	 = 0b101u << 7,
			CONV_8000MS	 = 0b110u << 7,
			CONV_16000MS = 0b111u << 7,
		};

		enum class ThermalAlertModeSelect : Register {
			THERM = 0x0010u, // Temperature Mode
			ALERT = 0x0000u, // Alert Mode
		};

		enum class AlertPolarity : Register {
			ACTIVE_HIGH = 0x0008u,
			ACTIVE_LOW	= 0x0000u,
		};

		enum class DataReadyAlertPinSelect : Register {
			DATA_READY = 0x0004u, // ALERT Pin reflects status of Data Ready Flag
			ALERT	   = 0x0000u, // ALERT Pin reflects status of Alert Flags
		};

		bool					  highAlertFlag				= false;
		bool					  lowAlertFlag				= false;
		bool					  dataReadyFlag				= false;
		bool					  eepromBusyFlag			= false;
		TemperatureConversionMode temperatureConversionMode = TemperatureConversionMode::CONTINUOUS;
		ConversionCycleTime		  conversionCycleTime		= ConversionCycleTime::CONV_1000MS;
		Averages				  averages					= Averages::AVG_8;
		ThermalAlertModeSelect	  thermalAlertMode			= ThermalAlertModeSelect::ALERT;
		AlertPolarity			  alertPolarity				= AlertPolarity::ACTIVE_LOW;
		DataReadyAlertPinSelect	  dataReadyAlertSelection	= DataReadyAlertPinSelect::ALERT;

		/**
		 * @brief Construct a new Config object from a TMP116 Config Register value.
		 *
		 * @param configRegister The TMP116 Config Register value.
		 */
		explicit Config(Register configRegister);

		/**
		 * @brief Construct a default Config object.
		 * @note The default is the TMP116 default configuration, not the defaults for the Config object variables.
		 */
		explicit constexpr Config(void) = default;

		Config(
			TemperatureConversionMode temperatureConversionMode,
			ConversionCycleTime		  conversionCycleTime,
			Averages				  averages,
			ThermalAlertModeSelect	  thermalAlertMode,
			AlertPolarity			  alertPolarity,
			DataReadyAlertPinSelect	  dataReadyAlertSelection
		);

		/**
		 * @brief Convert a Config object to a TMP116 Config Register value.
		 *
		 * @return Register The equivalent TMP116 Config Register value.
		 * @note The last 2 LSBs are Read Only and will always be set to 0, regardless of the constructor parameter.
		 * @note The Temperature Conversion Mode (0b10) is mapped to (0b00) (both accepted by TMP116 but not by type).
		 * 		 This results in non-reversible conversions with some register values when using this Config class.
		 */
		operator Register() const;
	};

	/**
	 * @brief Get the TMP116 Config Register value.
	 *
	 * @return std::optional<Register> The config register value of the TMP116 if successful.
	 */
	std::optional<Register> getConfigRegister();

	/**
	 * @brief Get the Config object representation of TMP116 Configuration.
	 *
	 * @return std::optional<Config> The Config object if successful.
	 */
	std::optional<Config> getConfig();

	/**
	 * @brief Set the configuration of the TMP116.
	 *
	 * @param config The total configuration of the TMP116.
	 * @return std::optional<Register> The register value written to the TMP116 config register if successful.
	 */
	std::optional<Register> setConfig(Config config);

	std::optional<Register> setConfig(
		std::optional<Config::TemperatureConversionMode> temperatureConversionMode = std::nullopt,
		std::optional<Config::ConversionCycleTime>		 conversionCycleTime	   = std::nullopt,
		std::optional<Config::Averages>					 averages				   = std::nullopt,
		std::optional<Config::ThermalAlertModeSelect>	 thermalAlertMode		   = std::nullopt,
		std::optional<Config::AlertPolarity>			 alertPolarity			   = std::nullopt,
		std::optional<Config::DataReadyAlertPinSelect>	 dataReadyAlertSelection   = std::nullopt
	);

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

private:
	I2C			 *i2c;
	DeviceAddress deviceAddress;

public:
	inline DeviceAddress getDeviceAddress() const { return deviceAddress; }
	inline void			 setDeviceAddress(DeviceAddress deviceAddress) { this->deviceAddress = deviceAddress; }
};
