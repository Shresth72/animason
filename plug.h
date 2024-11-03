#ifndef PLUG_H_
#define PLUG_H_

#include <stdlib.h>

// variable containing a pointer to the function of this type & init with NULL
void (*plug_init)(void) = NULL;
void *(*plug_pre_reload)(void) = NULL;
void (*plug_post_reload)(void *) = NULL;
void (*plug_update)(void) = NULL;

#endif // !PLUG_H_
