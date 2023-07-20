/**
 ******************************************************************************
 * @file			: TMP116_Config.test.cpp
 * @brief			: TMP116::Config Tests
 * @author			: Lawrence Stanton
 ******************************************************************************
 */

#include "TMP116.hpp"

#include "gtest/gtest.h"

using Config   = TMP116::Config;
using Register = TMP116::I2C::Register;

TEST(TMP116_TestConfig, constructorDecodesRegisterSampleTestCases) {
	Config config{Register{0x0000u}};
	EXPECT_EQ(config.highAlertFlag, false);
	EXPECT_EQ(config.lowAlertFlag, false);
	EXPECT_EQ(config.dataReadyFlag, false);
	EXPECT_EQ(config.eepromBusyFlag, false);

	EXPECT_EQ(config.temperatureConversionMode, Config::TemperatureConversionMode::CONTINUOUS);
	EXPECT_EQ(config.conversionCycleTime, Config::ConversionCycleTime::CONV_15_5MS);
	EXPECT_EQ(config.averages, Config::Averages::AVG_1);

	EXPECT_EQ(config.thermalAlertMode, Config::ThermalAlertModeSelect::ALERT);
	EXPECT_EQ(config.alertPolarity, Config::AlertPolarity::ACTIVE_LOW);
	EXPECT_EQ(config.dataReadyAlertSelection, Config::DataReadyAlertPinSelect::ALERT);

	config = Config{Register{0xFFFFu}};
	EXPECT_EQ(config.highAlertFlag, true);
	EXPECT_EQ(config.lowAlertFlag, true);
	EXPECT_EQ(config.dataReadyFlag, true);
	EXPECT_EQ(config.eepromBusyFlag, true);

	EXPECT_EQ(config.temperatureConversionMode, Config::TemperatureConversionMode::ONESHOT);
	EXPECT_EQ(config.conversionCycleTime, Config::ConversionCycleTime::CONV_16000MS);
	EXPECT_EQ(config.averages, Config::Averages::AVG_64);

	EXPECT_EQ(config.thermalAlertMode, Config::ThermalAlertModeSelect::THERM);
	EXPECT_EQ(config.alertPolarity, Config::AlertPolarity::ACTIVE_HIGH);
	EXPECT_EQ(config.dataReadyAlertSelection, Config::DataReadyAlertPinSelect::DATA_READY);

	config = Config{Register{0xAAAAu}};
	EXPECT_EQ(config.highAlertFlag, true);
	EXPECT_EQ(config.lowAlertFlag, false);
	EXPECT_EQ(config.dataReadyFlag, true);
	EXPECT_EQ(config.eepromBusyFlag, false);

	EXPECT_EQ(config.temperatureConversionMode, Config::TemperatureConversionMode::CONTINUOUS);
	EXPECT_EQ(config.conversionCycleTime, Config::ConversionCycleTime::CONV_4000MS);
	EXPECT_EQ(config.averages, Config::Averages::AVG_8);

	EXPECT_EQ(config.thermalAlertMode, Config::ThermalAlertModeSelect::ALERT);
	EXPECT_EQ(config.alertPolarity, Config::AlertPolarity::ACTIVE_HIGH);
	EXPECT_EQ(config.dataReadyAlertSelection, Config::DataReadyAlertPinSelect::ALERT);

	config = Config{Register{0x5555u}};
	EXPECT_EQ(config.highAlertFlag, false);
	EXPECT_EQ(config.lowAlertFlag, true);
	EXPECT_EQ(config.dataReadyFlag, false);
	EXPECT_EQ(config.eepromBusyFlag, true);

	EXPECT_EQ(config.temperatureConversionMode, Config::TemperatureConversionMode::SHUTDOWN);
	EXPECT_EQ(config.conversionCycleTime, Config::ConversionCycleTime::CONV_250MS);
	EXPECT_EQ(config.averages, Config::Averages::AVG_32);

	EXPECT_EQ(config.thermalAlertMode, Config::ThermalAlertModeSelect::THERM);
	EXPECT_EQ(config.alertPolarity, Config::AlertPolarity::ACTIVE_LOW);
	EXPECT_EQ(config.dataReadyAlertSelection, Config::DataReadyAlertPinSelect::DATA_READY);
}

TEST(TMP116_TestConfig, constructorMappsAlternativeCotinousConversionMode) {
	Config config{Register{0x0800u}};
	EXPECT_EQ(config.temperatureConversionMode, Config::TemperatureConversionMode::CONTINUOUS);
	config = Config{Register{0x0000u}};
	EXPECT_EQ(config.temperatureConversionMode, Config::TemperatureConversionMode::CONTINUOUS);
}

TEST(TMP116_TestConfig, configImplicitTypeCastsToRegister) {
	Config	 config{Register{0x0000u}};
	Register registerValue = config;
	EXPECT_EQ(registerValue, Register{0x0000u});

	config		  = Config{Register{0xFFFFu}};
	registerValue = config;
	EXPECT_EQ(registerValue, Register{0xFFFCu}); // 2 LSBs are read-only, so they are masked out.

	config		  = Config{Register{0xAAAAu}};
	registerValue = config;
	EXPECT_EQ(registerValue, Register{0xA2A8u}); // CC Alternative (0x0800) Mapped to 0x0000

	config		  = Config{Register{0x5555u}};
	registerValue = config;
	EXPECT_EQ(registerValue, Register{0x5554u});
}
