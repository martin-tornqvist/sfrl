#ifndef IO_HPP
#define IO_HPP

// -----------------------------------------------------------------------------
// Colors and font
// -----------------------------------------------------------------------------
enum class Clr
{
    black = COLOR_BLACK,
    red = COLOR_RED,
    green = COLOR_GREEN,
    yellow = COLOR_YELLOW,
    blue = COLOR_BLUE,
    magenta = COLOR_MAGENTA,
    cyan = COLOR_CYAN,
    white = COLOR_WHITE
};

enum class FontWgt
{
    normal,
    bold
};

// -----------------------------------------------------------------------------
// Keyboard key codes
// -----------------------------------------------------------------------------
const int KEY_DOWN = KEY_DOWN;
const int KEY_UP = KEY_UP;
const int KEY_LEFT = KEY_LEFT;
const int KEY_RIGHT = KEY_RIGHT;
const int KEY_HOME = KEY_HOME;
const int KEY_BACKSPACE = KEY_BACKSPACE;
const int KEY_F0 = KEY_F0;
const int KEY_F1 = KEY_F1;
const int KEY_F2 = KEY_F2;
const int KEY_F3 = KEY_F3;
const int KEY_F4 = KEY_F4;
const int KEY_F5 = KEY_F5;
const int KEY_F6 = KEY_F6;
const int KEY_F7 = KEY_F7;
const int KEY_F8 = KEY_F8;
const int KEY_F9 = KEY_F9;
const int KEY_F10 = KEY_F10;
const int KEY_F11 = KEY_F11;
const int KEY_F12 = KEY_F12;
const int KEY_F13 = KEY_F13;
const int KEY_F14 = KEY_F14;
const int KEY_F15 = KEY_F15;
const int KEY_DC = KEY_DC;
const int KEY_IC = KEY_IC;
const int KEY_SF = KEY_SF;
const int KEY_SR = KEY_SR;
const int KEY_NPAGE = KEY_NPAGE;
const int KEY_PPAGE = KEY_PPAGE;
const int KEY_ENTER = KEY_ENTER;
const int KEY_PRINT = KEY_PRINT;
const int KEY_LL = KEY_LL;
const int KEY_A1 = KEY_A1;
const int KEY_A3 = KEY_A3;
const int KEY_B2 = KEY_B2;
const int KEY_C1 = KEY_C1;
const int KEY_C3 = KEY_C3;
const int KEY_BTAB = KEY_BTAB;
const int KEY_END = KEY_END;
const int KEY_MOUSE = KEY_MOUSE;
const int KEY_RESIZE = KEY_RESIZE;
const int KEY_EVENT = KEY_EVENT;

struct Input
{
    Input() :
        c(0),
        keycode(0) {}
    
    char c;
    int  keycode;
};

namespace io
{

void init();

void cleanup();

void update_scr();

void clear_scr();

void scr_dim();

void draw_char(const P&p,
               const char c,
               const Clr fg,
               const Clr bg,
               const FontWgt wgt);

void draw_text(const P& p,
               const std::string& str,
               const Clr fg,
               const Clr bg,
               const FontWgt wgt);

Input get_input();

} // io

#endif // IO_HPP
