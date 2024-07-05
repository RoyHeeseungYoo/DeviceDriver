#include "DeviceDriver.h"

DeviceDriver::DeviceDriver(FlashMemoryDevice* hardware) : m_hardware(hardware)
{}

int DeviceDriver::read(long address)
{
    unsigned char result = (m_hardware->read(address));

    for (int i = 1; i < READ_TRIAL; i++) {
        if (result != (m_hardware->read(address))) {
            throw ReadFailException();
        }
    }

    return (int)result;
}

void DeviceDriver::write(long address, int data)
{
    if (VALUE_EMPTY != this->read(address)) {
        throw WriteFailException();
    }

    m_hardware->write(address, (unsigned char)data);
}