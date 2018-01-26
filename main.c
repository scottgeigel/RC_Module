#include "rc_init.h"
#include <stdio.h>
#include <unistd.h>

int poot_change(void* nah, int nope, int yeah) {
    for (int i = 0; i < yeah; i++) {
        printf("yeah ");
    }
    putchar('\n');
    return 0;
}

bool poot(void* nah, int poots) {
    for (int i = 0; i < poots; i++) {
        printf("poot ");
    }
    putchar('\n');
    return false;
}
int main() {
    RC_Manager manager;
    RC_Module poot_module;
    RC_Manager_init(&manager);
    RC_Module_init(&poot_module, NULL, poot, poot_change);
    RC_Manager_attach(&manager, &poot_module);
    sleep(2);
    RC_Manager_set_rc(&manager, 2);
    sleep(3);
    RC_Manager_set_rc(&manager, 4);
    sleep(1);
    RC_Manager_set_rc(&manager, 5);
    printf("screw it\n");
}
