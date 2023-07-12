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

#include "TMP116.hpp"

using ::testing::_;
using ::testing::Eq;
using ::testing::Return;
using ::testing::ReturnArg;

using DeviceAddress = TMP116::I2C::DeviceAddress;
using MemoryAddress = TMP116::I2C::MemoryAddress;
using Register		= TMP116::I2C::Register;

// Tests of Static Functions
TEST(TMP116_TestStatic, getTemperatureReturnsCorrectValue) {
	EXPECT_FLOAT_EQ(TMP116::getTemperature(0x0000u), 0.0f);
	EXPECT_FLOAT_EQ(TMP116::getTemperature(0x0001u), 0.0078125f);
	EXPECT_FLOAT_EQ(TMP116::getTemperature(0x8000u), -256.0f);
	EXPECT_FLOAT_EQ(TMP116::getTemperature(0x8001u), -255.9921875f);
	EXPECT_FLOAT_EQ(TMP116::getTemperature(0xFFFFu), -0.0078125f);
	EXPECT_FLOAT_EQ(TMP116::getTemperature(0x7FFFu), 255.9921875f);
	EXPECT_FLOAT_EQ(TMP116::getTemperature(0x7FFEu), 255.984375f);
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
	MockedI2C mockedI2C;
	TMP116	  tmp116{&mockedI2C, TMP116::DeviceAddress::ADD0_GND};

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
