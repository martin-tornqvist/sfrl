#include "io.hpp"

namespace io
{

// -----------------------------------------------------------------------------
// Private
// -----------------------------------------------------------------------------
namespace
{

void set_clr_attr(const Clr fg, const Clr bg)
{
    const int idx = fg * COLORS + bg;

    const int ncurses_clr_pair = COLOR_PAIR(idx);

    attron(ncurses_clr_pair);
}

} // namespace

// -----------------------------------------------------------------------------
// Public
// -----------------------------------------------------------------------------
void init()
{
    // Setup ncurses
    initscr();

    // TODO: What does this do?
    raw();

    // Allow for extended keyboard (like F1)
    keypad(stdscr, true);

    // Don't echo characters to the window
    noecho();

    // Hide the cursor
    curs_set(CURSOR_VISIBILITY::CURSOR_INVISIBLE);

    // Use colors
    start_color();

    // Init color pairs
    for (int f = 0; i < COLORS; ++f)
    {
        for (int b = 0; b < COLORS; ++b)
        {
            const int idx = f * COLORS + b;

            init_pair(idx, f, b);
        }
    }
}

void cleanup()
{
    endwin();
}

void update_scr()
{
    refresh();
}

void clear_scr()
{
    clear();
}

P scr_dim()
{
    P p;

    getmaxyx(stdscr, &p.y, &p.x);

    return p;
}

void draw_char(const P& p,
               const char c,
               const Clr fg,
               const Clr bg,
               const FontWgt wgt)
{
    set_clr_attr(fg, bg);

    if (wgt == FontWgt::bold)
    {
        attron(A_BOLD());
    }

    mvaddch(p.y, p.x, c);

    if (wgt == FontWgt::bold)
    {
        attroff(A_BOLD());
    }
}

void draw_text(const P& p,
               const std::string& str,
               const Clr fg,
               const Clr bg,
               const FontWgt wgt)
{
    set_clr_attr(fg, bg);

    if (wgt == FontWgt::bold)
    {
        attron(A_BOLD());
    }

    mvprintw(p.y, p.x, text);

    if (wgt == FontWgt::bold)
    {
        attroff(A_BOLD());
    }
}

Input get_input()
{
    const int ncurses_inp = getch();
    
    Input out;

    if (ncurses_inp >= 33 && ncurses_inp <= 126)
    {
        // Character (e.g. 'a', or '?')
        out.c = ncurses_inp;
    }
    else // Keycode (e.g. "left", or "F1")
    {
        keycode.c = ncurses_inp;
    }
    
    return out;
}

} // io
