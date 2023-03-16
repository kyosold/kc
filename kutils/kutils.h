#ifndef __KUTILS_H__
#define __KUTILS_H__

void ktrim_str(char *str, int str_len, char *what, int mode, char *result, size_t result_size);

char *ksystem_alloc(char *cmd);
int ksystem(char *cmd, char *response, size_t response_size);

#endif