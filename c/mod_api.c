#include "mod_api.h"
#include "grug/grug.h"
#include <string.h>

/* define the game-visible function pointer */
void (*on_spawn_Dog)(void* fn_data, int32_t age_in_years) = NULL;
void* on_spawn_Dog_data;

void (*on_many_args_Dog)(void* fn_data, int32_t p1, int32_t p2, int32_t p3, char* p4, char* p5, int32_t p6, int32_t p7, int32_t p8, char* p9) = NULL;
void* on_many_args_Dog_data;

/* set a named function pointer (generated: check all known names) */
void mod_api_set_function(const char *name, void *fn, void* fn_data) {
    if (strcmp(name, "on_spawn_Dog") == 0) {
        #pragma GCC diagnostic push
        #pragma GCC diagnostic ignored "-Wpedantic"
        on_spawn_Dog = fn;
        on_spawn_Dog_data = fn_data;
        #pragma GCC diagnostic pop
        return;
    }
    if (strcmp(name, "on_many_args_Dog") == 0) {
        #pragma GCC diagnostic push
        #pragma GCC diagnostic ignored "-Wpedantic"
        on_many_args_Dog = fn;
        on_many_args_Dog_data = fn_data;
        #pragma GCC diagnostic pop
        return;
    }
    /* unknown symbol: ignore or log in real system */
    printf("[mod_api] Unknown symbol registration: %s\n", name);
}

/* install interpreter trampolines as the default binding for each known function */
super_trampoline_data on_spawn_Dog_data_allocation;
super_trampoline_data on_many_args_Dog_data_allocation;
void mod_api_register_trampolines(void) {
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wpedantic"
    on_spawn_Dog_data_allocation = (super_trampoline_data) {
        .arg_tags = {GRUG_TAG_I32},
        .num_tags = 1,
        .entity = "Dog",
        .fn_name = "on_spawn",
    };
    mod_api_set_function("on_spawn_Dog", super_trampoline_smallreturn, &on_spawn_Dog_data_allocation);
    
    on_many_args_Dog_data_allocation = (super_trampoline_data) {
        .arg_tags = {GRUG_TAG_I32, GRUG_TAG_I32,GRUG_TAG_I32, GRUG_TAG_STRING, GRUG_TAG_STRING, GRUG_TAG_I32, GRUG_TAG_I32, GRUG_TAG_I32, GRUG_TAG_STRING},
        .num_tags = 9,
        .entity = "Dog",
        .fn_name = "on_many_args",
    };
    mod_api_set_function("on_many_args_Dog", super_trampoline_smallreturn, &on_many_args_Dog_data_allocation);
    #pragma GCC diagnostic pop
}
