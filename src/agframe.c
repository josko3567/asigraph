#include <stdint.h>

uint64_t * __agframe_location(void) {
        static uint64_t frame = 0;
        return &frame;
}