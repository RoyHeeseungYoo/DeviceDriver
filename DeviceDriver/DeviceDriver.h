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
};

class ReadFailException : public std::exception {};