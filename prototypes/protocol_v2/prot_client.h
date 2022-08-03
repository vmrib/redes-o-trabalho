#pragma once

#include <sys/types.h>

int protc_cd(int sockfd, char *dirname);

int protc_ls(int sockfd, char *arg);

int protc_mkdir(int sockfd, char *dirname);

int protc_get(int sockfd, char *filename);

int protc_put(int sockfd, int fd);