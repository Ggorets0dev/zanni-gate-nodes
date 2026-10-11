#pragma once

#include <stddef.h>
#include "znc_types.h"

typedef struct
{
    int port;
} znc_server_config_t;

typedef struct znc_config
{
    znc_server_config_t server;
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