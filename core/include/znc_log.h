#pragma once

typedef enum 
{
    LOG_LEVEL_INFO,
    LOG_LEVEL_WARN,
    LOG_LEVEL_ERROR
} log_level_e;

void log_format(log_level_e level, const char *format, ...);

void log_apply_rotation(long max_size);