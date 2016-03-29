#ifndef IO_HPP
#define IO_HPP

#include <SDL.h>

#include "rl_utils.hpp"

// -----------------------------------------------------------------------------
// Colors
// -----------------------------------------------------------------------------
typedef SDL_Color Clr;

const Clr clr_black             = {  0,   0,   0, 0};
const Clr clr_gray              = {128, 128, 128, 0};
const Clr clr_gray_drk          = { 48,  48,  48, 0};
const Clr clr_gray_xdrk         = { 30,  30,  30, 0};
const Clr clr_white             = {192, 192, 192, 0};
const Clr clr_white_high        = {255, 255, 255, 0};

const Clr clr_red               = {128,   0,   0, 0};
const Clr clr_red_lgt           = {255,   0,   0, 0};

const Clr clr_green             = {  0, 128,   0, 0};
const Clr clr_green_lgt         = {  0, 255,   0, 0};
const Clr clr_green_drk         = {  0, 64,    0, 0};

const Clr clr_yellow            = {255, 255,   0, 0};

const Clr clr_blue              = {  0,   0, 168, 0};
const Clr clr_blue_lgt          = { 30, 144, 255, 0};

const Clr clr_magenta           = {139,   0, 139, 0};
const Clr clr_magenta_lgt       = {255,   0, 255, 0};

const Clr clr_cyan              = {  0, 128, 128, 0};
const Clr clr_cyan_lgt          = {  0, 255, 255, 0};

const Clr clr_brown             = {153, 102,  61, 0};
const Clr clr_brown_drk         = { 96,  64,  32, 0};
const Clr clr_brown_gray        = { 83,  76,  66, 0};

const Clr clr_violet            = {128,   0, 255, 0};
const Clr clr_violet_drk        = { 64,   0, 128, 0};

const Clr clr_orange            = {255, 128,   0, 0};

// -----------------------------------------------------------------------------
// Keyboard key codes
// -----------------------------------------------------------------------------
const int KEY_DOWN              = SDLK_DOWN;
const int KEY_UP                = SDLK_UP;
const int KEY_LEFT              = SDLK_LEFT;
const int KEY_RIGHT             = SDLK_RIGHT;
const int KEY_HOME              = SDLK_HOME;
const int KEY_BACKSPACE         = SDLK_BACKSPACE;
const int KEY_F1                = SDLK_F1;
const int KEY_F2                = SDLK_F2;
const int KEY_F3                = SDLK_F3;
const int KEY_F4                = SDLK_F4;
const int KEY_F5                = SDLK_F5;
const int KEY_F6                = SDLK_F6;
const int KEY_F7                = SDLK_F7;
const int KEY_F8                = SDLK_F8;
const int KEY_F9                = SDLK_F9;
const int KEY_F10               = SDLK_F10;
const int KEY_F11               = SDLK_F11;
const int KEY_F12               = SDLK_F12;
const int KEY_F13               = SDLK_F13;
const int KEY_F14               = SDLK_F14;
const int KEY_F15               = SDLK_F15;
const int KEY_DELETE            = SDLK_DELETE;
const int KEY_NPAGE             = SDLK_PAGEDOWN;
const int KEY_PPAGE             = SDLK_PAGEUP;
const int KEY_ENTER             = SDLK_RETURN;
const int KEY_END               = SDLK_END;

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

P scr_dim();

void draw_char(const P& p,
               const char C,
               const Clr fg,
               const Clr bg = clr_black);

void draw_text(const P& p,
               const std::string& str,
               const Clr fg,
               const Clr bg = clr_black);

Input get_input();

} // io

#endif // IO_HPP
