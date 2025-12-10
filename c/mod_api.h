#pragma once
#include "grug/grug.h"
#include <stdio.h>

/* --- Game-level functions --- */
static inline void print_string(const char *s) {
    printf("[game fn] %s\n", s);
}

#define max_trampoline_args 32

typedef struct super_trampoline_data {
    char* entity;
    char* fn_name;
    uintptr_t num_tags;
    // We converted the enums to chars because why on earth would we need 64 bits for an enum that has 3 values
    // This actually led to a bug in the assembly because my dumb ass forgot I set it to char instead of enum
    // But that's a potential point against the super tramp: this structure is difficult to change as the layout is hard-coded into the assembly
    char /* grug_tag_t */ arg_tags[max_trampoline_args];
} super_trampoline_data;


/* --- Global function pointers for entity callbacks --- */
/* These are the pointers the game calls. Generated from mod_api.json. */
extern void (*on_spawn_Dog)(void* fn_data, int32_t age_in_years);
extern void* on_spawn_Dog_data;

extern void (*on_many_args_Dog)(void* fn_data, int32_t p1, int32_t p2, int32_t p3, char* p4, char* p5, int32_t p6, int32_t p7, int32_t p8, char* p9);
extern void* on_many_args_Dog_data;

/* --- Hooks used by grug.c to set the pointers --- */
/* Implementations live in mod_api.c (generated) */
void mod_api_set_function(const char *name, void *fn, void* fn_data);
void mod_api_register_trampolines(void);
// symbol is in here, this function does have one arg but it's really a naked assembly hack so giving it any args / return is a bit odd
void super_trampoline_smallreturn(void);
