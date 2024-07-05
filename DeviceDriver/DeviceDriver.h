#pragma once
#include "FlashMemoryDevice.h"
#include <stdexcept>

class DeviceDriver
{
public:
    DeviceDriver(FlashMemoryDevice* hardware);
    int read(long address);
    void write(long address, int data);

protected:
    FlashMemoryDevice* m_hardware;

private:
    const int READ_TRIAL = 5;
    const int VALUE_EMPTY = 0xFF;
};

class ReadFailException : public std::exception {};
class WriteFailException : public std::exception {};