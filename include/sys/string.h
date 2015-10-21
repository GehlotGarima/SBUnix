#ifndef _STRING_H
#define _STRING_H

#include <sys/defs.h>

/*
 * String functions
 */

const char* strcpy(char *dest, const char *src);
const char* strncpy(char *dest, const char *src, int count);
int strcmp(const char *str1, const char *str2);
int strncmp(const char *str1, const char *str2, int count);
int strlen(const char *str);
void strcat(char *new, const char *orig);
void strncat(char *new, const char *orig, int count);
char* index(char *str, char chr);
void bzero(void *s1, unsigned n);
char* strtok(char *str, const char* delim);
void memset(void *ptr, int value, uint64_t num);
void memcpy(void *dest, void *src, int size);
char *int_to_str(int num);
#endif
