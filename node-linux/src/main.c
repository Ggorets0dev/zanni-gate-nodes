#include <stdio.h>
#include "znc.h"
#include "znc_log.h"
#include <fcntl.h>

int on_recv(zanni_message_t *message)
{
    if (!message) return 1;

    log_format(LOG_LEVEL_INFO, "Recieved message: [tag=%s, dev_type=%d, status=%d]", message->tag, message->device_type, message->status);
}

int main(int argc, char** argv) 
{
    log_format(LOG_LEVEL_INFO, "Starting ZanniGate node...");

    if (argc != 2)
    {
        log_format(LOG_LEVEL_ERROR, "Wrong count of command line arguments, usage: zanni-gate-node config.json");
    }
    else if (access(argv[1], F_OK) != 0) 
    {
        log_format(LOG_LEVEL_ERROR, "Failed to find config file: [path=%s]", argv[1]);
    }

    znc_start_server(9199, 10, on_recv);
    
    return 0;
}
