#include "idle_manager.h"

unsigned long last_touch_time = 0;
bool idle_timeout_triggered = false;
bool block_idle_timeout = false;