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
const int key_down              = SDLK_DOWN;
const int key_up                = SDLK_UP;
const int key_left              = SDLK_LEFT;
const int key_right             = SDLK_RIGHT;
const int key_home              = SDLK_HOME;
const int key_backspace         = SDLK_BACKSPACE;
const int key_f1                = SDLK_F1;
const int key_f2                = SDLK_F2;
const int key_f3                = SDLK_F3;
const int key_f4                = SDLK_F4;
const int key_f5                = SDLK_F5;
const int key_f6                = SDLK_F6;
const int key_f7                = SDLK_F7;
const int key_f8                = SDLK_F8;
const int key_f9                = SDLK_F9;
const int key_f10               = SDLK_F10;
const int key_f11               = SDLK_F11;
const int key_f12               = SDLK_F12;
const int key_f13               = SDLK_F13;
const int key_f14               = SDLK_F14;
const int key_f15               = SDLK_F15;
const int key_delete            = SDLK_DELETE;
const int key_npage             = SDLK_PAGEDOWN;
const int key_ppage             = SDLK_PAGEUP;
const int key_enter             = SDLK_RETURN;
const int key_end               = SDLK_END;

struct Input
{
    Input() :
        c(0),
        keycode(0) {}

    char c;
    int  keycode;
};

enum class Align
{
    left,
    right
};

namespace io
{

void init();

void cleanup();

void update_scr();

void clear_scr();

void clear_area(const P& p0, const P& dims);
void clear_area(const R& r);

P scr_dim();

void draw_char(const P& p,
               const char c,
               const Clr fg,
               const Clr bg = clr_black);

void draw_text(P p,
               const std::string& str,
               const Clr fg,
               const Clr bg = clr_black,
               const Align align = Align::left);

void sleep(const unsigned int ms);

Input get_input();

void wait_for_proceed();

} // io

#endif // IO_HPP
