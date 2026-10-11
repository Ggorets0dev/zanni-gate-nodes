#pragma once

#include <stddef.h>
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

typedef struct znc_config
{
    znc_virtual_device_t *devices;
    size_t devices_count;
} znc_config_t;

/**
 * Loads and parses JSON configuration from file.
 *
 * @param file_path Path to JSON configuration file.
 * @param config Destination structure to populate.
 * @return 0 on success, non-zero on error.
 */
int znc_config_load(const char *file_path, znc_config_t *config);

/**
 * Releases memory allocated for configuration structure.
 *
 * @param config Pointer to configuration structure.
 */
void znc_config_free(znc_config_t *config);

/**
 * Finds a virtual device by its tag.
 *
 * @param config Pointer to configuration structure.
 * @param tag Tag name to search for.
 * @return Pointer to virtual device structure, or NULL if not found.
 */
const znc_virtual_device_t* znc_config_get_device_by_tag(const znc_config_t *config, const char *tag);