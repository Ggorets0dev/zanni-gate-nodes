#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdint.h>
#include <fcntl.h>
#include <errno.h>
#include <pthread.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/epoll.h>
#include <sys/eventfd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include "znc.h"
#include "znc_log.h"

#define MAX_EVENTS      10

static void set_nonblocking(int fd) 
{
    int flags = fcntl(fd, F_GETFL, 0);
    fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}

int znc_start_server(int port, int listen_backlog, zanni_recv_callback_t callback)
{
    int rc = 0;
    zanni_message_t msg = {0};

    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) 
    {
        return 1;
    }
    
    if (listen(server_fd, listen_backlog) < 0) 
    {
        return 1;
    }

    int epoll_fd = epoll_create1(0);
    if (epoll_fd == -1)     return 1;

    set_nonblocking(server_fd);

    struct epoll_event ev;
    
    ev.events = EPOLLIN | EPOLLET;
    ev.data.fd = server_fd;
    epoll_ctl(epoll_fd, EPOLL_CTL_ADD, server_fd, &ev);

    struct epoll_event events[MAX_EVENTS];

    for (;;)
    {
        int n = epoll_wait(epoll_fd, events, MAX_EVENTS, -1);

        for (int i = 0; i < n; i++)
        {
            int active_fd = events[i].data.fd;

            if (active_fd == server_fd)
            {
                for (;;)
                {
                    char ip[INET_ADDRSTRLEN] = {0};
                    struct sockaddr_in client_addr;
                    socklen_t client_len = sizeof(client_addr);
                    int client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &client_len);

                    if (client_fd == -1) 
                    {
                        if (errno == EAGAIN || errno == EWOULDBLOCK) 
                        {
                            break;
                        } 
                        else 
                        {
                            break;
                        }
                    }

                    set_nonblocking(client_fd);
                    
                    ev.events = EPOLLIN | EPOLLET;
                    ev.data.fd = client_fd;
                    
                    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_fd, &ev) == -1) 
                    {
                        perror("epoll_ctl: client_fd");
                        close(client_fd);
                    }

                    if (inet_ntop(AF_INET, &client_addr.sin_addr, ip, sizeof(ip)) != NULL) 
                    {
                        log_format(LOG_LEVEL_INFO, "New client connected: [ip=%s, fd=%d]", ip, client_fd);
                    }
                }
            }
            else 
            {
                uint8_t buf[ZANNI_MSG_SIZE] = {0};

                for (;;)
                {
                    ssize_t bytes_read = read(active_fd, buf, sizeof(buf));
                    
                    // ======= ERROR SECTION
                    if (bytes_read == -1) 
                    {
                        if (errno == EAGAIN || errno == EWOULDBLOCK) 
                        {
                            break;
                        } 
                        else 
                        {
                            close(active_fd);
                            log_format(LOG_LEVEL_INFO, "Client disconnected: [fd=%d]", active_fd);
                            break;
                        }
                    } 
                    else if (bytes_read != sizeof(buf)) 
                    {
                        close(active_fd);
                        log_format(LOG_LEVEL_INFO, "Client disconnected: [fd=%d]", active_fd);
                        break;
                    }
                    // =======

                    rc = zanni_message_decode(buf, bytes_read, &msg);

                    if (rc == 0)
                    {
                        callback(&msg);
                        continue;
                    }
                }
            }
        }
    }

    return 0;
}