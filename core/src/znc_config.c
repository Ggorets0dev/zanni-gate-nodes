#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <json-c/json.h>

#include "znc_config.h"
#include "znc_log.h"

static int parse_device_type(const char *type_str, zanni_virt_dev_e *out_type)
{
    if (!type_str || !out_type)
    {
        return -1;
    }

    /* Handles both "toggle" and example typo "toogle" */
    if (strcasecmp(type_str, "toggle") == 0 || strcasecmp(type_str, "toogle") == 0)
    {
        *out_type = ZANNI_VIRT_DEV_TOGGLE;
        return 0;
    }

    log_format(LOG_LEVEL_WARN, "Unknown device type: '%s'", type_str);
    return -1;
}

static int parse_device(struct json_object *dev_obj, znc_virtual_device_t *device)
{
    struct json_object *tag_obj = NULL;
    struct json_object *type_obj = NULL;
    struct json_object *commands_obj = NULL;

    if (!dev_obj || !device)
    {
        return -1;
    }

    memset(device, 0, sizeof(*device));

    /* Parse 'tag' field */
    if (!json_object_object_get_ex(dev_obj, "tag", &tag_obj))
    {
        log_format(LOG_LEVEL_ERROR, "Device entry is missing required field: 'tag'");
        return -1;
    }

    const char *tag_str = json_object_get_string(tag_obj);
    if (!tag_str || strlen(tag_str) == 0)
    {
        log_format(LOG_LEVEL_ERROR, "Device 'tag' field cannot be empty");
        return -1;
    }

    strncpy(device->tag, tag_str, ZANNI_TAG_SIZE);
    device->tag[ZANNI_TAG_SIZE - 1] = '\0';

    /* Parse 'type' field */
    if (!json_object_object_get_ex(dev_obj, "type", &type_obj))
    {
        log_format(LOG_LEVEL_ERROR, "Device '%s' is missing required field: 'type'", device->tag);
        return -1;
    }

    const char *type_str = json_object_get_string(type_obj);
    if (!type_str)
    {
        log_format(LOG_LEVEL_ERROR, "Device '%s' has invalid 'type' field", device->tag);
        return -1;
    }

    if (parse_device_type(type_str, &device->type) != 0)
    {
        log_format(LOG_LEVEL_WARN, "Device '%s' has unsupported type '%s'", device->tag, type_str);
    }

    /* Parse 'commands' object */
    if (json_object_object_get_ex(dev_obj, "commands", &commands_obj))
    {
        if (json_object_is_type(commands_obj, json_type_object))
        {
            struct json_object *on_obj = NULL;
            struct json_object *off_obj = NULL;

            if (json_object_object_get_ex(commands_obj, "on", &on_obj))
            {
                const char *on_str = json_object_get_string(on_obj);
                if (on_str)
                {
                    device->commands.toggle.on = strdup(on_str);
                }
            }

            if (json_object_object_get_ex(commands_obj, "off", &off_obj))
            {
                const char *off_str = json_object_get_string(off_obj);
                if (off_str)
                {
                    device->commands.toggle.off = strdup(off_str);
                }
            }
        }
        else
        {
            log_format(LOG_LEVEL_WARN, "Device '%s' field 'commands' is not an object", device->tag);
        }
    }

    log_format(LOG_LEVEL_INFO, "Parsed device: [tag=%s, type=%s, on='%s', off='%s']",
               device->tag,
               type_str ? type_str : "unknown",
               device->commands.toggle.on ? device->commands.toggle.on : "none",
               device->commands.toggle.off ? device->commands.toggle.off : "none");

    return 0;
}

int znc_config_load(const char *file_path, znc_config_t *config)
{
    if (!file_path || !config)
    {
        log_format(LOG_LEVEL_ERROR, "Invalid parameters passed to znc_config_load");
        return -1;
    }

    memset(config, 0, sizeof(*config));

    log_format(LOG_LEVEL_INFO, "Loading configuration from '%s'", file_path);

    struct json_object *root = json_object_from_file(file_path);
    if (!root)
    {
        log_format(LOG_LEVEL_ERROR, "Failed to parse JSON file: '%s'", file_path);
        return -1;
    }

    struct json_object *devices_arr = NULL;
    if (!json_object_object_get_ex(root, "virtual_devices", &devices_arr))
    {
        log_format(LOG_LEVEL_ERROR, "Root object has no 'virtual_devices' key in '%s'", file_path);
        json_object_put(root);
        return -1;
    }

    if (!json_object_is_type(devices_arr, json_type_array))
    {
        log_format(LOG_LEVEL_ERROR, "'virtual_devices' must be a JSON array in '%s'", file_path);
        json_object_put(root);
        return -1;
    }

    size_t count = json_object_array_length(devices_arr);
    if (count == 0)
    {
        log_format(LOG_LEVEL_WARN, "No virtual devices configured in '%s'", file_path);
        json_object_put(root);
        return 0;
    }

    config->devices = calloc(count, sizeof(znc_virtual_device_t));
    if (!config->devices)
    {
        log_format(LOG_LEVEL_ERROR, "Memory allocation failure for virtual devices array");
        json_object_put(root);
        return -1;
    }

    config->devices_count = 0;

    for (size_t i = 0; i < count; i++)
    {
        struct json_object *dev_obj = json_object_array_get_idx(devices_arr, i);
        if (!dev_obj)
        {
            continue;
        }

        if (parse_device(dev_obj, &config->devices[config->devices_count]) == 0)
        {
            config->devices_count++;
        }
        else
        {
            log_format(LOG_LEVEL_WARN, "Skipped invalid device entry at index %zu", i);
        }
    }

    json_object_put(root);

    log_format(LOG_LEVEL_INFO, "Successfully loaded %zu virtual devices from '%s'",
               config->devices_count, file_path);

    return 0;
}

void znc_config_free(znc_config_t *config)
{
    if (!config)
    {
        return;
    }

    if (config->devices)
    {
        for (size_t i = 0; i < config->devices_count; i++)
        {
            free(config->devices[i].commands.toggle.on);
            free(config->devices[i].commands.toggle.off);
        }

        free(config->devices);
        config->devices = NULL;
    }

    config->devices_count = 0;
}

const znc_virtual_device_t* znc_config_get_device_by_tag(const znc_config_t *config, const char *tag)
{
    if (!config || !tag)
    {
        return NULL;
    }

    for (size_t i = 0; i < config->devices_count; i++)
    {
        if (strncmp(config->devices[i].tag, tag, ZANNI_TAG_SIZE) == 0)
        {
            return &config->devices[i];
        }
    }

    return NULL;
}
