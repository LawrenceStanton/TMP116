/**
 ******************************************************************************
 * @file			: TMP116.test.cpp
 * @brief			: TMP116 Tests
 * @author			: Lawrence Stanton
 ******************************************************************************
 */

#include "TMP116.hpp"

#include "gmock/gmock.h"
#include "gtest/gtest.h"

#include "../Src/TMP116.cpp"

using ::testing::_;
using ::testing::Eq;
using ::testing::Return;
using ::testing::ReturnArg;

using DeviceAddress = TMP116::I2C::DeviceAddress;
using MemoryAddress = TMP116::I2C::MemoryAddress;
using Register		= TMP116::I2C::Register;

// Tests of Static Functions
TEST(TMP116_TestStatic, convertTemperatureRegisterReturnsCorrectValuesRegisterToFloat) {
	EXPECT_FLOAT_EQ(convertTemperatureRegister(static_cast<Register>(0x0000u)), 0.0f);
	EXPECT_FLOAT_EQ(convertTemperatureRegister(static_cast<Register>(0x0001u)), 0.0078125f);
	EXPECT_FLOAT_EQ(convertTemperatureRegister(static_cast<Register>(0x8000u)), -256.0f);
	EXPECT_FLOAT_EQ(convertTemperatureRegister(static_cast<Register>(0x8001u)), -255.9921875f);
	EXPECT_FLOAT_EQ(convertTemperatureRegister(static_cast<Register>(0xFFFFu)), -0.0078125f);
	EXPECT_FLOAT_EQ(convertTemperatureRegister(static_cast<Register>(0x7FFFu)), 255.9921875f);
	EXPECT_FLOAT_EQ(convertTemperatureRegister(static_cast<Register>(0x7FFEu)), 255.984375f);
}

TEST(TMP116_TestStatic, convertTemperatureRegisterReturnsCurrentValuesFloatToRegister) {
	EXPECT_EQ(convertTemperatureRegister(0.0f), static_cast<Register>(0x0000u));
	EXPECT_EQ(convertTemperatureRegister(0.0078125f), static_cast<Register>(0x0001u));
	EXPECT_EQ(convertTemperatureRegister(-0.0078125f), static_cast<Register>(0xFFFFu));
	EXPECT_EQ(convertTemperatureRegister(-0.015625f), static_cast<Register>(0xFFFEu));
	EXPECT_EQ(convertTemperatureRegister(-256.0f), static_cast<Register>(0x8000u));
	EXPECT_EQ(convertTemperatureRegister(-255.9921875f), static_cast<Register>(0x8001u));
	EXPECT_EQ(convertTemperatureRegister(255.9921875f), static_cast<Register>(0x7FFFu));
	EXPECT_EQ(convertTemperatureRegister(255.984375f), static_cast<Register>(0x7FFEu));
}

class MockedI2C : public TMP116::I2C {
public:
	MOCK_METHOD(
		std::optional<Register>, //
		read,
		(DeviceAddress deviceAddress, MemoryAddress memoryAddress),
		(override)
	);
	MOCK_METHOD(
		std::optional<Register>,
		write,
		(DeviceAddress deviceAddress, MemoryAddress memoryAddress, Register registerValue),
		(override)
	);
};

class TMP116_Test : public ::testing::Test {
public:
	MockedI2C			  mockedI2C;
	TMP116::DeviceAddress deviceAddress = TMP116::DeviceAddress::ADD0_GND;
	TMP116				  tmp116{&mockedI2C, deviceAddress};

	inline void disableI2C(void) {
		ON_CALL(mockedI2C, read).WillByDefault(Return(std::nullopt));
		ON_CALL(mockedI2C, write).WillByDefault(Return(std::nullopt));
	}
};

TEST_F(TMP116_Test, getTemperatureNormallyReturnsValue) {
	const MemoryAddress temperatureAddress				   = 0x00u;
	const Register		temperatureRegisterTestRandomValue = 0x15D2u;
	const float			expectedTemperatureValue		   = 43.640625f;
	EXPECT_CALL(mockedI2C, read(Eq(this->tmp116.getDeviceAddress()), Eq(temperatureAddress)))
		.WillOnce(Return(temperatureRegisterTestRandomValue));

	const auto temperature = this->tmp116.getTemperature();
	EXPECT_FLOAT_EQ(temperature, expectedTemperatureValue);
}

TEST_F(TMP116_Test, getTemperatureReturnsAbsoluteZeroWhenI2CReadFails) {
	this->disableI2C();
	EXPECT_EQ(this->tmp116.getTemperature(), -256.0f);
}

TEST_F(TMP116_Test, getDeviceIdNormallyReturnsValue) {
	const MemoryAddress deviceIdAddress		 = 0x0Fu;
	const Register		deviceIdDefaultValue = 0x0118u;
	EXPECT_CALL(mockedI2C, read(Eq(this->tmp116.getDeviceAddress()), Eq(deviceIdAddress)))
		.WillOnce(Return(deviceIdDefaultValue));

	const auto deviceId = this->tmp116.getDeviceId();
	EXPECT_EQ(deviceId.value(), deviceIdDefaultValue);
}

TEST_F(TMP116_Test, getDeviceIdReturnsNulloptWhenI2CReadFails) {
	this->disableI2C();
	EXPECT_EQ(this->tmp116.getDeviceId(), std::nullopt);
}

TEST_F(TMP116_Test, setHighLimitNormallyReturnsRegisterValue) {
	const MemoryAddress highLimitAddress			  = 0x02u;
	float				setHighLimitValue			  = -10.0f;
	const Register		highLimitExpectedWrittenValue = 0xFB00u;

	EXPECT_CALL(mockedI2C, write(Eq(this->deviceAddress), Eq(highLimitAddress), _)).WillOnce(ReturnArg<2>());

	const auto highLimitResult = this->tmp116.setHighLimit(setHighLimitValue);
	EXPECT_EQ(highLimitResult.value(), highLimitExpectedWrittenValue);
}

TEST_F(TMP116_Test, setHighLimitReturnsNulloptWhenI2CWriteFails) {
	this->disableI2C();
	EXPECT_EQ(this->tmp116.setHighLimit(0.0f), std::nullopt);
}

TEST_F(TMP116_Test, setLowLimitNormallyReturnsRegisterValue) {
	const MemoryAddress lowLimitAddress				 = 0x03u;
	float				setLowLimitValue			 = -10.0f;
	const Register		lowLimitExpectedWrittenValue = 0xFB00u;

	EXPECT_CALL(mockedI2C, write(Eq(this->deviceAddress), Eq(lowLimitAddress), _)).WillOnce(ReturnArg<2>());

	const auto lowLimitResult = this->tmp116.setLowLimit(setLowLimitValue);
	EXPECT_EQ(lowLimitResult.value(), lowLimitExpectedWrittenValue);
}

TEST_F(TMP116_Test, setLowLimitReturnsNulloptWhenI2CWriteFails) {
	this->disableI2C();
	EXPECT_EQ(this->tmp116.setLowLimit(0.0f), std::nullopt);
}
