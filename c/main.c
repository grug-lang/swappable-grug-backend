#include "grug/grug.h"
#include "mod_api.h"
#include <stdio.h>

int main(void) {
    printf("[game] starting\n");

    /* initialize grug */
    grug_init();

    /* try to load a custom backend (overrides trampolines if it provides symbols) */
    grug_load_custom_backend("./backend.so");

    /* load mods (mock) */
    grug_load_mods();

    /* call the game-visible pointer — this is zero-cost if a native backend replaced it */
    if (on_spawn_Dog) {
        printf("[game] invoking on_spawn_Dog(3)\n");
        on_spawn_Dog(on_spawn_Dog_data, 3); /* either trampoline (interpreter) or native function pointer */
    } else {
        printf("[game] on_spawn_Dog is not bound!\n");
    }

    if(on_many_args_Dog) {
        printf("[game] invoking on_many_args_Dog(2, 3, 5, '7', '13', 17, 19, 23, '29')\n");
        on_many_args_Dog(on_many_args_Dog_data, 2, 3, 5, "7", "13", 17, 19, 23, "29");
    } else {
        printf("[game] on_many_args_Dog is not bound!\n");
    }

    printf("[game] done\n");
    return 0;
}
