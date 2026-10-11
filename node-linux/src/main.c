#include <stdio.h>
#include "znc.h"
#include "znc_log.h"

int on_recv(zanni_message_t *message)
{
    if (!message) return 1;

    log_format(LOG_LEVEL_INFO, "Recieved message: [tag=%s, dev_type=%d, status=%d]", message->tag, message->device_type, message->status);
}

int main() 
{
    log_format(LOG_LEVEL_INFO, "Starting ZanniGate node...");

    znc_start_server(9199, 10, on_recv);
    
    return 0;
}
