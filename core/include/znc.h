#pragma once

#include "zanni_protocol.h"

typedef int (*zanni_recv_callback_t)(zanni_message_t *);

int znc_start_server(int port, int listen_backlog, zanni_recv_callback_t callback);