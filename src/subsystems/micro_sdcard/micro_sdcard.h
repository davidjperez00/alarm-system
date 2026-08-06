#ifndef MICRO_SDCARD_H
#define MICRO_SDCARD_H

#include <stdbool.h>
#include <stdint.h>

bool micro_sdcard_init(void);
bool micro_sdcard_check_inserted(void);

#endif // MICRO_SDCARD_H