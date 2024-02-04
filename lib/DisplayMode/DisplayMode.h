#pragma once

#include <stdint.h>

class DisplayMode {
public:
    explicit DisplayMode();

    virtual uint32_t** get() = 0;
};
