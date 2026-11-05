
#include <3ds.h>
#include <stdio.h>

#include "game.h"

int main(int argc, char *argv[])
{
    (void)argc;
    (void)argv;

    gfxInitDefault();
    consoleInit(GFX_TOP, NULL);

    Game game;

    game_init(&game);

    printf("\x1b[1;1H");
    printf("================================\n");
    printf("       POKEMON WORLD\n");
    printf("================================\n");
    printf("\n");
    printf("Loading game...\n");

    while (aptMainLoop() && game.running)
    {
        hidScanInput();

        game_update(&game);
        game_render(&game);

        gfxFlushBuffers();
        gfxSwapBuffers();
        gspWaitForVBlank();
    }

    game_shutdown(&game);

    gfxExit();

    return 0;
}
