#include <StaticDisplayMode.h>

StaticDisplatMode::StaticDisplatMode(uint32_t** content)
: content(content) {}

uint32_t** StaticDisplatMode::get() {
    return this->content;
}
