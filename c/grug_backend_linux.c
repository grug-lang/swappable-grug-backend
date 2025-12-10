#include "grug/grug.h"
#include "mod_api.h"
#include <stdio.h>
#include <stdint.h>

/* ------------------------------------------------------------------
   Native implementation of the Dog::on_spawn function
   ------------------------------------------------------------------ */
static void native_on_spawn_Dog(void* fn_data, int32_t age_in_years) {
    (void)fn_data;
    printf("[native backend] Dog spawned, age=%d\n", age_in_years);

    /* Since this is a native backend, we can directly call game functions */
    for (int32_t i = 0; i < age_in_years; ++i) {
        print_string("Woof from native backend!");
    }
}

static void native_on_many_args_Dog(void* fn_data, int32_t p1, int32_t p2, int32_t p3, char* p4, char* p5, int32_t p6, int32_t p7, int32_t p8, char* p9) {
    (void)fn_data;
    printf("[native backend] Dog got a bunch of args: %d, %d, %d, '%s', '%s', %d, %d, %d, '%s'\n", p1, p2, p3, p4, p5, p6, p7, p8, p9);
}

/* ------------------------------------------------------------------
   Symbol table: informs grug.c which functions we provide
   ------------------------------------------------------------------ */
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
static grug_symbol_t symbols[] = {
    { "on_spawn_Dog", native_on_spawn_Dog },
    { "on_many_args_Dog", native_on_many_args_Dog },
};
#pragma GCC diagnostic pop

/* ------------------------------------------------------------------
   No-op execute (not needed for native backends)
   ------------------------------------------------------------------ */
static void backend_execute(const grug_ast_t *ast, grug_value_t *args, size_t argc) {
    (void)args;
    (void)argc;
    /* Normally unused, but provided for interface completeness */
    printf("[native backend] execute() called for %s::%s\n",
           ast->entity_name, ast->function_name);
}

/* ------------------------------------------------------------------
   Backend instance exported for grug.c to dlopen() and dlsym()
   ------------------------------------------------------------------ */
grug_backend_t grug_backend_instance = {
    .name = "native_linux_x86_64",
    .symbols = symbols,
    .num_symbols = sizeof(symbols) / sizeof(symbols[0]),
    .execute = backend_execute
};
