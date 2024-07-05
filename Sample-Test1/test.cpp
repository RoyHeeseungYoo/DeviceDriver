#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "../DeviceDriver/DeviceDriver.cpp"

using namespace std;

class MockDevice : public FlashMemoryDevice {
public:
	MOCK_METHOD(unsigned char, read, (long address), (override));
	MOCK_METHOD(void, write, (long address, unsigned char data), (override));
};

class DeviceFixture : public testing::Test {
public:
	MockDevice normal_rd_device;
	MockDevice abnormal_rd_device;

	const int READ_ADDRESS = 0x12345678;
	const unsigned char VALID_VALUE_READ = 0x1;
	const unsigned char INVALID_VALUE_READ = 0x2;

	void setNormalReadDevice() {
		EXPECT_CALL(normal_rd_device, read(READ_ADDRESS))
			.WillRepeatedly(testing::Return(VALID_VALUE_READ));
	}
	
	void setAbnormalReadDevice() {
		EXPECT_CALL(abnormal_rd_device, read(READ_ADDRESS))
			.WillOnce(testing::Return(VALID_VALUE_READ))
			.WillOnce(testing::Return(VALID_VALUE_READ))
			.WillOnce(testing::Return(VALID_VALUE_READ))
			.WillOnce(testing::Return(VALID_VALUE_READ))
			.WillRepeatedly(testing::Return(INVALID_VALUE_READ));
	}
};

TEST_F(DeviceFixture, ReadNormal) {
	setNormalReadDevice();
	for (int i = 0; i < 5; i++)
		EXPECT_EQ(VALID_VALUE_READ, normal_rd_device.read(READ_ADDRESS));
}

TEST_F(DeviceFixture, ReadAbnormal) {
	setAbnormalReadDevice();
	for (int i = 0; i < 4; i++) {
		EXPECT_EQ(VALID_VALUE_READ, abnormal_rd_device.read(READ_ADDRESS));
	}
	EXPECT_NE(VALID_VALUE_READ, abnormal_rd_device.read(READ_ADDRESS));
}