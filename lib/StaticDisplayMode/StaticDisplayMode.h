#pragma once

#include <DisplayMode.h>

class StaticDisplatMode : public DisplayMode {
public:
    explicit StaticDisplatMode(uint32_t** content);

    uint32_t** get();

private:
    uint32_t** content;
};
