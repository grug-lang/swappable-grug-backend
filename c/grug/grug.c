#include "grug.h"
#include "../mod_api.h"   /* include generated mod API header */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dlfcn.h>

/* mock AST loaded from file */
static grug_ast_t loaded_asts[] = {
    {
        .mod_name = "animals/labrador",
        .entity_name = "Dog",
        .function_name = "on_spawn"
    },
    {.mod_name = "animals/labrador",
        .entity_name = "Dog",
        .function_name = "on_many_args"}
};

/* backend state */
static grug_backend_t *current_backend = NULL;

/* default interpreter backend implementation (keeps demo tiny) */
static void interpreter_execute(const grug_ast_t *ast, grug_value_t *args, size_t argc) {
    printf("[interpreter backend] Executing %s::%s\n",
           ast->entity_name, ast->function_name);
    if (argc == 1 && args[0].tag == GRUG_TAG_I32) {
        int32_t age = args[0].as.i32;
        for (int32_t i = 0; i < age; ++i) {
            /* interpreter-level effect; in a real interpreter you'd call print_string AST node */
            print_string("Woof from interpreter backend!");
        }
    }
    if (argc == 9 && args[0].tag == GRUG_TAG_I32) {
        printf("[interpreter backend] Dog got a bunch of args: %d, %d, %d, '%s', '%s', %d, %d, %d, '%s'\n",
            args[0].as.i32, args[1].as.i32, args[2].as.i32, args[3].as.string, args[4].as.string, args[5].as.i32, args[6].as.i32, args[7].as.i32, args[8].as.string);
    }
}

/* default backend object (no exported native symbols) */
static grug_backend_t default_interpreter_backend = {
    .name = "default_interpreter",
    .symbols = NULL,
    .num_symbols = 0,
    .execute = interpreter_execute
};

void grug_set_backend(grug_backend_t *backend) {
    current_backend = backend;
}

/* Load custom backend .so and, if found, register its provided symbols via mod_api_set_function().
   Note: grug.c does not hardcode function names — it simply iterates the backend's exported table. */
void grug_load_custom_backend(const char *so_path) {
    void *handle = dlopen(so_path, RTLD_NOW);
    if (!handle) {
        printf("[bindings] No custom backend found at %s — staying with interpreter\n", so_path);
        grug_set_backend(&default_interpreter_backend);
        return;
    }

    grug_backend_t *custom_backend = dlsym(handle, "grug_backend_instance");
    if (!custom_backend) {
        printf("[bindings] custom backend missing symbol grug_backend_instance — using interpreter\n");
        grug_set_backend(&default_interpreter_backend);
        return;
    }

    /* override interpreter trampolines with any native symbols the backend provides */
    if (custom_backend->symbols && custom_backend->num_symbols > 0) {
        for (size_t i = 0; i < custom_backend->num_symbols; ++i) {
            const char *name = custom_backend->symbols[i].name;
            void *fn = custom_backend->symbols[i].fn;
            /* delegate actual per-name binding to mod_api.c (generated) */
            mod_api_set_function(name, fn, NULL);
            printf("[bindings] Backend override: %s -> %p\n", name, fn);
        }
    }

    grug_set_backend(custom_backend);
    printf("[bindings] Using backend %s\n", custom_backend->name);
}

/* initialization & minimal frontend simulation */
void grug_init(void) {
    printf("[bindings] Initializing grug...\n");
    /* set trampolines as default bindings */
    mod_api_register_trampolines();
    /* default backend */
    grug_set_backend(&default_interpreter_backend);
}

void grug_load_mods(void) {
    printf("[frontend] (mock) loaded mod %s for entity %s\n", loaded_asts[0].mod_name, loaded_asts[0].entity_name);
}

grug_ast_t *grug_get_ast_for(const char *entity, const char *fn) {
    for(size_t i=0; i<sizeof(loaded_asts)/sizeof(grug_ast_t); ++i) {
        grug_ast_t* loaded_ast = &(loaded_asts[i]);
        if (strcmp(entity, loaded_ast->entity_name) == 0 && strcmp(fn, loaded_ast->function_name) == 0) {
            return loaded_ast;
        }
    }

    return NULL;
}

void grug_call_backend(const grug_ast_t *ast, grug_value_t *args, size_t argc) {
    if (!ast || !current_backend || !current_backend->execute) {
        printf("[bindings] no backend/ast available\n");
        return;
    }
    current_backend->execute(ast, args, argc);
}
