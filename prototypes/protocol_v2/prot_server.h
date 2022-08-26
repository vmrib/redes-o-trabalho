#pragma once

#include <sys/types.h>

int prots_cd(int sockfd, const char *dirname);

int prots_ls(int sockfd, char *flag);

int prots_mkdir(int sockfd, char *dirname);

int prots_get(int sockfd);

int prots_put(int sockfd);