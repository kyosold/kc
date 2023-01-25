#include <stdio.h>
#include <time.h>
#include "kmc.h"

/**
 * @brief Store an item
 * 
 * @param mc_host           The memcached server host
 * @param mc_port           The memcached server port
 * @param timeout           The timeout for connect
 * @param key               The key under which to store the value
 * @param key_len           The length of key
 * @param value             The value to store
 * @param value_len         The length of value
 * @param expire            The expiration time, 0 is never expire.
 * @param err               The store error string, set NULL if don't need
 * @param err_size          The size of err
 * @return unsigned int     Return 0:succ, 1:fail
 */
unsigned int kmc_set(char *mc_host, int mc_port, unsigned int timeout, 
        char *key, unsigned int key_len, char *value, unsigned int value_len, time_t expire, 
        char *err, unsigned int err_size)
{
    unsigned int flag = 0;
    char *result = NULL;
    int ret = 1;

    memcached_st *memc = memcached_create(NULL);
    memcached_return mrc;
    memcached_server_st *mc_servers;

    memcached_behavior_set(memc, MEMCACHED_BEHAVIOR_NO_BLOCK, 1);
    memcached_behavior_set(memc, MEMCACHED_BEHAVIOR_CONNECT_TIMEOUT, timeout);

    mc_servers = memcached_server_list_append(NULL, mc_host, mc_port, &mrc);
    if (mrc == MEMCACHED_SUCCESS) {
        mrc= memcached_server_push(memc, mc_servers);
        memcached_server_list_free(mc_servers);

        if (mrc == MEMCACHED_SUCCESS) {
            mrc = memcached_set(memc, key, key_len, value, value_len, expire, flag);
            if (mrc == MEMCACHED_SUCCESS) {
                ret = 0;
                memcached_free(memc);
                return ret;
            } 

            ret = 1;
        }
    }

    if (err) {
        snprintf(err, err_size, "%s", memcached_strerror(memc, mrc));
    }

    memcached_free(memc);
    return ret;
}

/**
 * @brief Retrieve an item
 * 
 * @param mc_host       The memcached server host
 * @param mc_port       The memcached server port
 * @param timeout       The timeout for connect
 * @param key           The key of the item to retrieve
 * @param key_len       The length of key
 * @param value_len     The length of value. -1:fail, 0:not found, >0: the length of value
 * @param err           The store error string, set NULL if don't need
 * @param err_size      The size of err
 * @return char*        Return the value stored or NULL is fail, check value_len
 */
char *kmc_get(char *mc_host, int mc_port, unsigned int timeout,
              char *key, unsigned int key_len, int *value_len, 
              char *err, unsigned int err_size)
{
    size_t nval = 0;
    unsigned int flag = 0;
    char *result = NULL;

    memcached_st *memc = memcached_create(NULL);
    memcached_return mrc;
    memcached_server_st *mc_servers;

    memcached_behavior_set(memc, MEMCACHED_BEHAVIOR_NO_BLOCK, 1);
    memcached_behavior_set(memc, MEMCACHED_BEHAVIOR_CONNECT_TIMEOUT, timeout);

    mc_servers = memcached_server_list_append(NULL, mc_host, mc_port, &mrc);
    if (mrc == MEMCACHED_SUCCESS) {
        mrc = memcached_server_push(memc, mc_servers);
        memcached_server_list_free(mc_servers);

        if (mrc == MEMCACHED_SUCCESS) {
            result = memcached_get(memc, key, key_len, &nval, &flag, &mrc);
            if (mrc == MEMCACHED_SUCCESS) {
                *value_len = nval;
                memcached_free(memc);
                return result;
            } else if (mrc == MEMCACHED_NOTFOUND) {
                *value_len = 0;
            } else {
                *value_len = -1;
            }
        }
    }

    if (err) {
        snprintf(err, err_size, "%s", memcached_strerror(memc, mrc));
    }

    memcached_free(memc);

    *value_len = -1;
    return NULL;
}

/**
 * @brief Delete an item
 * 
 * @param mc_host           The memcached server host
 * @param mc_port           The memcached server port
 * @param timeout           The timeout for connect
 * @param key               The key to be deleted
 * @param key_len           The length of key
 * @param err               The store error string, set NULL if don't need
 * @param err_size          The size of err
 * @return unsigned int     Reutrn succ:0, not found:1, fail:2
 */
unsigned int kmc_del(char *mc_host, int mc_port, unsigned int timeout,
                     char *key, unsigned int key_len, 
                     char *err, unsigned int err_size)
{
    unsigned int ret = 2;
    memcached_st *memc = memcached_create(NULL);
    memcached_return mrc;
    memcached_server_st *mc_servers;

    memcached_behavior_set(memc, MEMCACHED_BEHAVIOR_NO_BLOCK, 1);
    memcached_behavior_set(memc, MEMCACHED_BEHAVIOR_CONNECT_TIMEOUT, timeout);

    mc_servers = memcached_server_list_append(NULL, mc_host, mc_port, &mrc);
    if (mrc == MEMCACHED_SUCCESS) {
        mrc= memcached_server_push(memc, mc_servers);
        memcached_server_list_free(mc_servers);

        if (mrc == MEMCACHED_SUCCESS) {
            mrc = memcached_delete(memc, key, key_len, 0);
            if (mrc == MEMCACHED_SUCCESS) {
                ret = 0;
                memcached_free(memc);
                return ret;
            } else if (mrc == MEMCACHED_NOTFOUND) {
                ret = 1;
            } else {
                ret = 2;
            }
        } 
    } 
    
    if (err) {
        snprintf(err, err_size, "%s", memcached_strerror(memc, mrc));
    }

    memcached_free(memc);
    return ret;
}
