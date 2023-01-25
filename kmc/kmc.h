/**
 * @file kmc.h
 * @author songjian (kyosold@qq.com)
 * @brief
 * @version 0.1
 * @date 2022-12-07
 *
 * @copyright Copyright (c) 2022
 *
 */
#ifndef __K_MC_H__
#define __K_MC_H__

#include <stdint.h>
#include <stdlib.h>
#include <memcached.h>

unsigned int kmc_set(char *mc_host, int mc_port, unsigned int timeout,
                     char *key, unsigned int key_len, char *value, unsigned int value_len, time_t expire,
                     char *err, unsigned int err_size);

char *kmc_get(char *mc_host, int mc_port, unsigned int timeout,
              char *key, unsigned int key_len, int *value_len,
              char *err, unsigned int err_size);

unsigned int kmc_del(char *mc_host, int mc_port, unsigned int timeout,
                     char *key, unsigned int key_len, 
                     char *err, unsigned int err_size);

#endif