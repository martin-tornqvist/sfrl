#include "io.hpp"

#include "game.hpp"

#if defined(_WIN32) && defined(SDL_MODE)
#undef main
#endif
int main(int argc, char* argv[])
{
    (void)argc;
    (void)argv;

    // Init IO
    io::init();
    io::clear_scr();

    // Run game session until done
    game::run_session();

    // Cleanup IO
    io::cleanup();

    return 0;
}
