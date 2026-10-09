#include <stdio.h>
#include "zanni_node_core.h" // Assuming this header exists in core/include

int on_recv(zanni_message_t *message)
{
    if (!message) return 1;

    printf("TAG: %s\n", message->tag);
    printf("DEVICE_TYPE: %d\n", message->device_type);
    printf("STATUS: %d\n", message->status);
}

int main() 
{
    printf("Starting Zanni Linux Node...\n");

    znc_start_server(9199, 10, on_recv);
    
    return 0;
}
