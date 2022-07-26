#pragma once

#include <sys/types.h>

int protc_cd(int sockfd, const char *dirname);

int protc_ls(int sockfd, const char *arg);

int protc_mkdir(int sockfd, const char *dirname);

int protc_get(int sockfd, const char *filename);

int protc_put(int sockfd, int fd);