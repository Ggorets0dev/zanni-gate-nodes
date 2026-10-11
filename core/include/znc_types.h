#pragma once

#include "zanni_protocol.h"

typedef struct 
{
    char *on;
    char *off;
} znc_commands_toggle_t;

typedef union
{
    znc_commands_toggle_t toggle;
} znc_commands_u;

typedef struct znc_virtual_device
{
    char tag[ZANNI_TAG_SIZE];
    zanni_virt_dev_e type;
    znc_commands_u commands;
} znc_virtual_device_t;