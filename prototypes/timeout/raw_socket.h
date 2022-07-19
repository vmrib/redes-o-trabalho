#pragma once

#include <sys/types.h> // size_t e uint

int rs_socket(char *device);

void rs_set_timeout(int sockfd, uint useconds);

void rs_send(int sockfd, void *data, size_t bytes);

void rs_recv(int sockfd, void *data, size_t bytes);

void rs_close(int sockfd);