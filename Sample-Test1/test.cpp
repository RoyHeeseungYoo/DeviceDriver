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
	MockDevice normal_device;
	MockDevice abnormal_device;

	const int READ_ADDRESS = 0x12345678;
	const int WRITE_ADDRESS = 0xFFFF0000;
	const unsigned char VALID_VALUE_READ = 0x1;
	const unsigned char INVALID_VALUE_READ = 0x2;
	const unsigned char EMPTY_VALUE = 0xFF;

	void setNormalReadDevice() {
		EXPECT_CALL(normal_device, read(READ_ADDRESS))
			.Times(testing::AtLeast(5))
			.WillRepeatedly(testing::Return(VALID_VALUE_READ));
	}
	
	void setAbnormalReadDevice() {
		EXPECT_CALL(abnormal_device, read(READ_ADDRESS))
			.WillOnce(testing::Return(VALID_VALUE_READ))
			.WillOnce(testing::Return(VALID_VALUE_READ))
			.WillOnce(testing::Return(VALID_VALUE_READ))
			.WillOnce(testing::Return(VALID_VALUE_READ))
			.WillRepeatedly(testing::Return(INVALID_VALUE_READ));
	}

	void setNormalWriteDevice() {
		EXPECT_CALL(normal_device, read(WRITE_ADDRESS))
			.WillOnce(testing::Return(EMPTY_VALUE))
			.WillOnce(testing::Return(EMPTY_VALUE))
			.WillOnce(testing::Return(EMPTY_VALUE))
			.WillOnce(testing::Return(EMPTY_VALUE))
			.WillOnce(testing::Return(EMPTY_VALUE))
			.WillRepeatedly(testing::Return(VALID_VALUE_READ));

		EXPECT_CALL(normal_device, write(WRITE_ADDRESS, VALID_VALUE_READ))
			.WillRepeatedly(testing::Return());
	}
	
	void setAbnormalWriteDevice() {
		EXPECT_CALL(abnormal_device, read(WRITE_ADDRESS))
			.Times(testing::AtLeast(5))
			.WillRepeatedly(testing::Return(INVALID_VALUE_READ));

		EXPECT_CALL(abnormal_device, write(WRITE_ADDRESS, VALID_VALUE_READ))
			.WillRepeatedly(testing::Return());
	}
};

TEST_F(DeviceFixture, ReadNormal) {
	setNormalReadDevice();
	for (int i = 0; i < 5; i++)
		EXPECT_EQ(VALID_VALUE_READ, normal_device.read(READ_ADDRESS));
}

TEST_F(DeviceFixture, ReadAbnormal) {
	setAbnormalReadDevice();
	for (int i = 0; i < 4; i++) {
		EXPECT_EQ(VALID_VALUE_READ, abnormal_device.read(READ_ADDRESS));
	}
	EXPECT_NE(VALID_VALUE_READ, abnormal_device.read(READ_ADDRESS));
}

TEST_F(DeviceFixture, WriteNormal) {
	setNormalWriteDevice();
	for (int i = 0; i < 5; i++)
		EXPECT_EQ(EMPTY_VALUE, normal_device.read(WRITE_ADDRESS));
	normal_device.write(WRITE_ADDRESS, VALID_VALUE_READ);

	EXPECT_EQ(VALID_VALUE_READ, normal_device.read(WRITE_ADDRESS));
}

TEST_F(DeviceFixture, WriteAbnormal) {
	setAbnormalWriteDevice();
	for (int i = 0; i < 5; i++)
		EXPECT_NE(EMPTY_VALUE, abnormal_device.read(WRITE_ADDRESS));
	abnormal_device.write(WRITE_ADDRESS, VALID_VALUE_READ);

	EXPECT_NE(EMPTY_VALUE, abnormal_device.read(WRITE_ADDRESS));
	EXPECT_NE(VALID_VALUE_READ, abnormal_device.read(WRITE_ADDRESS));
}

TEST_F(DeviceFixture, ReadDeviceDriver) {
	setNormalReadDevice();
	
	DeviceDriver dd{ &normal_device };

	EXPECT_EQ(VALID_VALUE_READ, dd.read(READ_ADDRESS));
}

TEST_F(DeviceFixture, ReadDeviceDriverException) {
	setAbnormalReadDevice();
	
	DeviceDriver dd{ &abnormal_device };

	EXPECT_THROW(dd.read(READ_ADDRESS), ReadFailException);
}
