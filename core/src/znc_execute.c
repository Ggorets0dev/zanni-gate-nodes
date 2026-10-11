#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "znc_execute.h"

char* znc_execute_toggle(const znc_commands_toggle_t* cmds, bool state) 
{
    if (cmds == NULL) 
    {
        return NULL; 
    }

    const char *command = state ? cmds->on : cmds->off;

    if (command == NULL) 
    {
        return NULL;
    }

    FILE *fp = popen(command, "r");
    if (fp == NULL) 
    {
        return NULL;
    }

    size_t buffer_size = 1024;
    char *output = malloc(buffer_size);
    if (output == NULL) 
    {
        pclose(fp);
        return NULL;
    }
    
    output[0] = '\0';
    size_t current_len = 0;
    char temp_buffer[256];

    while (fgets(temp_buffer, sizeof(temp_buffer), fp) != NULL) 
    {
        size_t temp_len = strlen(temp_buffer);
        
        if (current_len + temp_len + 1 > buffer_size) 
        {
            buffer_size *= 2;
            char *new_output = realloc(output, buffer_size);
            if (new_output == NULL) 
            {
                free(output);
                pclose(fp);
                return NULL;
            }
            output = new_output;
        }
        
        strcpy(output + current_len, temp_buffer);
        current_len += temp_len;
    }

    pclose(fp);
    return output;
}