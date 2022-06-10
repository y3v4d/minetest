#include "game.h"

int main() {
    if(game_init() != 0) return 1;

    game_loop();
    game_close();

    return 0;
}
