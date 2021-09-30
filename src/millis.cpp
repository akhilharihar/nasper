#include "millis.h"
#include "pico/time.h"

unsigned int millis() {
    return to_ms_since_boot(get_absolute_time());
}