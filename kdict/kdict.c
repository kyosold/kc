#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "kdict.h"

/* Maximum value size for integers and doubles. */
#define MAXVALSZ    1024

/* Minimal allocated number of entries in a dictionary */
#define DICTMINSZ   128

/* Invalid key token */
#define DICT_INVALID_KEY ((char *)-1)

/*------------------------------------------------------------------*/
/* Doubles the allocated size associated to a pointer */
/* 'size' is the current allocated size. */
static void *_mem_double(void *ptr, int size)
{
    void *newptr;
    newptr = calloc(2 * size, 1);
    if (newptr == NULL)
        return NULL;

    memcpy(newptr, ptr, size);
    free(ptr);
    return newptr;
}

/**
 * @brief Duplicate a string
 * 
 * @param s     String to duplicate
 * @return char*    Pointer to a newly allocated string, to be freed with free()
 * 
 * This is a replacement for strdup(). This implementation is provided
 * for systems that do not have it.
 */
static char *_xstrdup(char *s)
{
    char *t;
    if (!s)
        return NULL;
    t = malloc(strlen(s) + 1);
    if (t) {
        strcpy(t, s);
    }
    return t;
}

/*------------------------------------------------------------------*/

unsigned kdict_hash(char *key)
{
    int len;
    unsigned hash;
    int i;

    len = strlen(key);
    for (hash = 0, i = 0; i < len; i++) {
        hash += (unsigned)key[i];
        hash += (hash << 10);
        hash ^= (hash >> 6);
    }
    hash += (hash << 3);
    hash ^= (hash >> 11);
    hash += (hash << 15);

    return hash;
}

/**
 * @brief Create a new dictionary object.
 * 
 * @param size      Optional initial size of the dictionary.
 * @return kdict*   1 newly allocated dictionary objet.
 * 
 * This function allocates a new dictionary object of given size and returns
 * it, use kdict_free to free. If you do not know in advance (roughly) the number of entries in the
 * dictionary, give size=0.
 */
kdict *kdict_new(int size)
{
    kdict *d;

    /* If no size was specified, allocate space for DICTMINSZ */
    if (size < DICTMINSZ)
        size = DICTMINSZ;
    
    if (!(d = (kdict *)calloc(1, sizeof(kdict))))
        return NULL;

    d->size = size;
    d->val = (char **)calloc(size, sizeof(char *));
    d->key = (char **)calloc(size, sizeof(char *));
    d->hash = (unsigned int *)calloc(size, sizeof(unsigned));

    if (d->val == NULL || d->key == NULL || d->hash == NULL) {
        if (d->val != NULL)
            free(d->val);
        if (d->key != NULL)
            free(d->key);
        if (d->hash != NULL)
            free(d->hash);

        return NULL;
    }
    return d;
}

/**
 * @brief Delete a dictionary object
 * 
 * @param d     dictionary object to deallocate.
 * 
 * Deallocate a dictionary object and all memory associated to it.
 */
void kdict_free(kdict *d)
{
    int i;

    if (d == NULL)
        return;

    for (i = 0; i < d->size; i++) {
        if (d->key[i] != NULL)
            free(d->key[i]);
        if (d->val[i] != NULL)
            free(d->val[i]);
    }
    free(d->val);
    free(d->key);
    free(d->hash);
    free(d);
    return;
}

/**
 * @brief Get a value from a dictionary.
 * 
 * @param d         dictionary object to search.
 * @param key       Key to look for in the dictionary.
 * @param def       Default value to return if key not found.
 * @return char*    1 pointer to internally allocated character string.
 * 
 * This function locates a key in a dictionary and returns a pointer to its
 * value, or the passed 'def' pointer if no such key can be found in
 * dictionary. The returned character pointer points to data internal to the
 * dictionary object, you should not try to free it or modify it.
 */
char *kdict_get(kdict *d, char *key, char *def)
{
    unsigned hash;
    int i;

    hash = kdict_hash(key);
    for (i = 0; i < d->size; i++) {
        if (d->key[i] == NULL)
            continue;
        
        /* Compare hash */
        if (hash == d->hash[i]) {
            /* Compare string, to avoid hash collisions */
            if (!strcmp(key, d->key[i])) {
                return d->val[i];
            }
        }
    }
    return def;
}

/**
 * @brief Set a value in a dictionary.
 * 
 * @param d     dictionary object to modify.
 * @param key   Key to modify or add.
 * @param val   Value to add.
 * @return int  0 if Ok, anything else otherwise
 * 
 * If the given key is found in the dictionary, the associated value is
 * replaced by the provided one. If the key cannot be found in the
 * dictionary, it is added to it.
 * 
 * It is Ok to provide a NULL value for val, but NULL values for the dictionary
 * or the key are considered as errors: the function will return immediately
 * in such a case.
 * 
 * Notice that if you kdict_set a variable to NULL, a call to kdict_get will
 * return a NULL value: the variable will be found, and its value (NULL) is 
 * returned. In other words, setting the variable content to NULL is equivalent 
 * to deleting the variable from the dictionary. It is not possible (in this 
 * implementation) to have a key in the dictionary without value.
 * 
 * This function returns non-zero in case of failure.
 * 
 */
int kdict_set(kdict *d, char *key, char *val)
{
    int i;
    unsigned hash;

    if (d == NULL || key == NULL)
        return -1;
    
    /* Compute hash for this key */
    hash = kdict_hash(key);
    /* Find if value is already in dictionary */
    if (d->n > 0) {
        for (i = 0; i < d->size; i++) {
            if (d->key[i] == NULL)
                continue;
            if (hash == d->hash[i]) {   /* Same hash value */
                if (!strcmp(key, d->key[i])) {  /* Same key */
                    /* Found a value: modify and return */
                    if (d->val[i] != NULL)
                        free(d->val[i]);
                    d->val[i] = val ? _xstrdup(val) : NULL;
                    /* Value has been modified: return */
                    return 0;
                }
            }
        }
    }
    /* Add a new value */
    /* See if dictionary needs to grow */
    if (d->n == d->size) {
        /* Reached maximum size: reallocate dictionary */
        d->val = (char **)_mem_double(d->val, d->size * sizeof(char *));
        d->key = (char **)_mem_double(d->key, d->size * sizeof(char *));
        d->hash = (unsigned int *)_mem_double(d->hash, d->size * sizeof(unsigned int));
        if ((d->val == NULL) || (d->key == NULL) || (d->hash == NULL)) {
            /* Cannot grow dictionary */
            return -1;
        }
        /* Double size */
        d->size *= 2;
    }

    /* Insert key in the first empty slot */
    for (i = 0; i < d->size; i++) {
        if (d->key[i] == NULL) {
            /* Add key here */
            break;
        }
    }
    /* Copy key */
    d->key[i] = _xstrdup(key);
    d->val[i] = val ? _xstrdup(val) : NULL;
    d->hash[i] = hash;
    d->n++;

    return 0;
}

/**
 * @brief Delete a key in a dictionary
 * 
 * @param d     dictionary object to modify.
 * @param key   Key to remove.
 * 
 * This function deletes a key in a dictionary. Nothing is done if the
 * key cannot be found.
 */
void kdict_unset(kdict *d, char *key)
{
    unsigned hash;
    int i;

    if (key == NULL)
        return;

    hash = kdict_hash(key);
    for (i = 0; i < d->size; i++) {
        if (d->key[i] == NULL)
            continue;
        
        /* Compare hash */
        if (hash == d->hash[i]) {
            /* Compare string, to avoid hash collisions */
            if (!strcmp(key, d->key[i])) {
                /* Found key */
                break;
            }
        }
    }
    if (i >= d->size) {
        /* Key not found */
        return;
    }

    free(d->key[i]);
    d->key[i] = NULL;
    if (d->val[i] != NULL) {
        free(d->val[i]);
        d->val[i] = NULL;
    }
    d->hash[i] = 0;
    d->n--;

    return;
}

/**
 * @brief Dump a dictionary to an opened file pointer.
 * 
 * @param d     Dictionary to dump
 * @param out   Opened file pointer.
 * 
 * Dumps a dictionary onto an opened file pointer. Key pairs are printed out
 * as @c [Key]=[Value], one per line. It is Ok to provide stdout or stderr as
 * output file pointers.
 */
void kdict_output_file(kdict *d, FILE *out)
{
    int i;

    if (d == NULL || out == NULL)
        return;
    if (d->n < 1) {
        /* empty dictionary */
        return;
    }
    for (i = 0; i < d->size; i++) {
        if (d->key[i]) {
            fprintf(out, "%20s\t[%s]\n",
                    d->key[i],
                    d->val[i] ? d->val[i] : "UNDEF");
        }
    }
    return;
}