#include "io.hpp"

#include <SDL_image.h>
#include <cassert>

namespace io
{

namespace
{

// TODO: Load some of this stuff from a Lua script or something

const std::string font_dir      = "font";
const std::string font_name     = "terminal10x16.png";
const std::string font_path     = font_dir + "/" + font_name;

const std::string window_title  = "SF RL v0.0.1";

SDL_Window*     sdl_window_     = nullptr;
SDL_Renderer*   sdl_renderer_   = nullptr;

SDL_Surface*    scr_srf_        = nullptr;
SDL_Texture*    scr_texture_    = nullptr;

const int CELL_W                = 10;
const int CELL_H                = 16;
const int FONT_IMG_X_CELLS      = 16;
const int FONT_IMG_Y_CELLS      = 16;
const size_t PIXEL_DATA_W       = FONT_IMG_X_CELLS* CELL_W;
const size_t PIXEL_DATA_H       = FONT_IMG_Y_CELLS* CELL_H;

const int SCREEN_BPP            = 32;

bool font_px_data_[PIXEL_DATA_W][PIXEL_DATA_H];

SDL_Event sdl_event_;

P scr_px_dim()
{
    P out;

    SDL_GetWindowSize(sdl_window_, &out.x, &out.y);

    return out;
}

Uint32 px(SDL_Surface& srf, const int PIXEL_X, const int PIXEL_Y)
{
    const int BPP = srf.format->BytesPerPixel;

    //p is the address to the pixel we want to retrieve
    Uint8* p = (Uint8*)srf.pixels + PIXEL_Y * srf.pitch + PIXEL_X * BPP;

    switch (BPP)
    {
    case 1:
        return *p;

    case 2:
        return *(Uint16*)p;

    case 3:
        if (SDL_BYTEORDER == SDL_BIG_ENDIAN)
        {
            return p[0] << 16 | p[1] << 8 | p[2];
        }
        else
        {
            return p[0] | p[1] << 8 | p[2] << 16;
        }
        break;

    case 4:
        return *(Uint32*)p;

    default:
        break;
    }

    return -1;
}

void put_px(const SDL_Surface& srf,
            const int PIXEL_X,
            const int PIXEL_Y,
            Uint32 px)
{
    const int BPP = srf.format->BytesPerPixel;

    //p is the address to the pixel we want to set
    Uint8* const p = (Uint8*)srf.pixels + PIXEL_Y * srf.pitch + PIXEL_X * BPP;

    switch (BPP)
    {
    case 1:
        *p = px;
        break;

    case 2:
        *(Uint16*)p = px;
        break;

    case 3:
        if (SDL_BYTEORDER == SDL_BIG_ENDIAN)
        {
            p[0] = (px >> 16) & 0xff;
            p[1] = (px >> 8)  & 0xff;
            p[2] = px & 0xff;
        }
        else //Little endian
        {
            p[0] = px & 0xff;
            p[1] = (px >> 8)  & 0xff;
            p[2] = (px >> 16) & 0xff;
        }
        break;

    case 4:
        *(Uint32*)p = px;
        break;

    default:
        break;
    }
}

void load_font()
{
//    TRACE_FUNC_BEGIN;

    SDL_Surface* font_srf_tmp = IMG_Load(font_path.c_str());

    Uint32 img_clr = SDL_MapRGB(font_srf_tmp->format, 255, 255, 255);

    for (int x = 0; x < font_srf_tmp->w; ++x)
    {
        for (int y = 0; y < font_srf_tmp->h; ++y)
        {
            const bool IS_IMG_PX = px(*font_srf_tmp, x, y) == img_clr;

            font_px_data_[x][y] = IS_IMG_PX;
        }
    }

    SDL_FreeSurface(font_srf_tmp);

//    TRACE_FUNC_END;
}

void put_pixels_on_scr(const bool px_data[PIXEL_DATA_W][PIXEL_DATA_H],
                       const P& sheet_pos,
                       const P& scr_px_pos,
                       const Clr& clr)
{
    const int PX_CLR = SDL_MapRGB(scr_srf_->format, clr.r, clr.g, clr.b);

    const int SHEET_PX_X0 = sheet_pos.x * CELL_W;
    const int SHEET_PX_Y0 = sheet_pos.y * CELL_H;
    const int SHEET_PX_X1 = SHEET_PX_X0 + CELL_W - 1;
    const int SHEET_PX_Y1 = SHEET_PX_Y0 + CELL_H - 1;
    const int SCR_PX_X0   = scr_px_pos.x;
    const int SCR_PX_Y0   = scr_px_pos.y;

    int scr_px_x = SCR_PX_X0;

    for (int sheet_px_x = SHEET_PX_X0; sheet_px_x <= SHEET_PX_X1; sheet_px_x++)
    {
        int scr_px_y = SCR_PX_Y0;

        for (int sheet_px_y = SHEET_PX_Y0; sheet_px_y <= SHEET_PX_Y1; sheet_px_y++)
        {
            if (px_data[sheet_px_x][sheet_px_y])
            {
                put_px(*scr_srf_, scr_px_x, scr_px_y, PX_CLR);
            }
            ++scr_px_y;
        }
        ++scr_px_x;
    }
}

P font_sheet_pos(const unsigned char C)
{
    const int Y = C / FONT_IMG_X_CELLS;

    const int X = Y == 0 ? C : (C % (Y * FONT_IMG_X_CELLS));

    return P(X, Y);
}

void put_char_pixels_on_scr(const char C,
                            const P& scr_px_pos,
                            const Clr& clr)
{
    const P sheet_pos(font_sheet_pos(C));

    put_pixels_on_scr(font_px_data_,
                      sheet_pos,
                      scr_px_pos,
                      clr);
}

void draw_char_at_px(const char C,
                     const P& px_pos,
                     const Clr& clr,
                     const Clr& bg_clr = clr_black)
{
//    if (DRAW_BG_CLR)
//    {
//        const P cell_dims(CELL_W, CELL_H);
//
//        draw_rectangle_solid(px_pos, cell_dims, bg_clr);
//    }

    put_char_pixels_on_scr(C,
                           px_pos,
                           clr);
}

void sleep(const Uint32 DURATION)
{
    if (DURATION == 1)
    {
        SDL_Delay(DURATION);
    }
    else // Duration > 1
    {
        const Uint32 WAIT_UNTIL = SDL_GetTicks() + DURATION;

        while (SDL_GetTicks() < WAIT_UNTIL)
        {
            SDL_PumpEvents();
        }
    }
}

} // namespace

void init()
{
    cleanup();

//    TRACE << "Setting up rendering window" << std::endl;

    const int SCR_PX_W = 80 * CELL_W; //config::scr_px_w();
    const int SCR_PX_H = 40 * CELL_H; //config::scr_px_h();

//    if (config::is_fullscreen())
//    {
//        sdl_window_ = SDL_CreateWindow(title.c_str(),
//                                       SDL_WINDOWPOS_UNDEFINED,
//                                       SDL_WINDOWPOS_UNDEFINED,
//                                       SCR_PX_W, SCR_PX_H,
//                                       SDL_WINDOW_FULLSCREEN_DESKTOP);
//    }
//
//    if (!config::is_fullscreen() || !sdl_window_)
//    {
    sdl_window_ = SDL_CreateWindow(window_title.c_str(),
                                   SDL_WINDOWPOS_CENTERED,
                                   SDL_WINDOWPOS_CENTERED,
                                   SCR_PX_W, SCR_PX_H,
                                   SDL_WINDOW_SHOWN);
//    }

    if (!sdl_window_)
    {
//        TRACE << "Failed to create window" << std::endl;
        assert(false);
    }

    sdl_renderer_ = SDL_CreateRenderer(sdl_window_, -1, SDL_RENDERER_ACCELERATED);

    if (!sdl_renderer_)
    {
//        TRACE << "Failed to create SDL renderer" << std::endl;
        assert(false);
    }

    scr_srf_ = SDL_CreateRGBSurface(0,
                                    SCR_PX_W, SCR_PX_H,
                                    SCREEN_BPP,
                                    0x00FF0000,
                                    0x0000FF00,
                                    0x000000FF,
                                    0xFF000000);

    if (!scr_srf_)
    {
//        TRACE << "Failed to create screen surface" << std::endl;
        assert(false);
    }

    scr_texture_ = SDL_CreateTexture(sdl_renderer_,
                                     SDL_PIXELFORMAT_ARGB8888,
                                     SDL_TEXTUREACCESS_STREAMING,
                                     SCR_PX_W,
                                     SCR_PX_H);

    if (!scr_texture_)
    {
//        TRACE << "Failed to create screen texture" << std::endl;
        assert(false);
    }

    load_font();
}

void cleanup()
{
//    TRACE_FUNC_BEGIN;

    if (sdl_renderer_)
    {
        SDL_DestroyRenderer(sdl_renderer_);
        sdl_renderer_ = nullptr;
    }

    if (sdl_window_)
    {
        SDL_DestroyWindow(sdl_window_);
        sdl_window_ = nullptr;
    }

    if (scr_texture_)
    {
        SDL_DestroyTexture(scr_texture_);
        scr_texture_ = nullptr;
    }

    if (scr_srf_)
    {
        SDL_FreeSurface(scr_srf_);
        scr_srf_ = nullptr;
    }

//    TRACE_FUNC_END;
}

void update_scr()
{
    SDL_UpdateTexture(scr_texture_,
                      nullptr,
                      scr_srf_->pixels,
                      scr_srf_->pitch);

    SDL_RenderCopy(sdl_renderer_,
                   scr_texture_,
                   nullptr,
                   nullptr);

    SDL_RenderPresent(sdl_renderer_);
}

void clear_scr()
{
    SDL_FillRect(scr_srf_,
                 nullptr,
                 SDL_MapRGB(scr_srf_->format, 0, 0, 0));
}

P scr_dim()
{
    const P px_dim(scr_px_dim());

    return px_dim / P(CELL_W, CELL_H);
}

void draw_char(const P& p,
               const char C,
               const Clr fg,
               const Clr bg)
{
    const P px_pos(p * P(CELL_W, CELL_H));

    draw_char_at_px(C, px_pos, fg, bg);
}

void draw_text(const P& p,
               const std::string& str,
               const Clr fg,
               const Clr bg)
{
    const P cell_px_dim(CELL_W, CELL_H);

    P px_pos(p * cell_px_dim);

    const P scr_px_dim(scr_dim() * cell_px_dim);

    if (px_pos.y < 0 || px_pos.y >= scr_px_dim.y)
    {
        return;
    }

    const size_t    MSG_W       = str.size();
    const int       MSG_PX_W    = MSG_W * CELL_W;

//    draw_rectangle_solid(px_pos, {MSG_PX_W, CELL_PX_H}, bg);

    const int   MSG_PX_X1           = px_pos.x + MSG_PX_W - 1;
    const bool  MSG_W_FIT_ON_SCR    = MSG_PX_X1 < scr_px_dim.x;

    //X position to start drawing dots instead when the message does not fit horizontally.
    const int   PX_X_DOTS           = scr_px_dim.x - (CELL_W * 3);

    for (size_t i = 0; i < MSG_W; ++i)
    {
        if (px_pos.x < 0 || px_pos.x >= scr_px_dim.x)
        {
            return;
        }

        const bool DRAW_DOTS = !MSG_W_FIT_ON_SCR && px_pos.x >= PX_X_DOTS;

        if (DRAW_DOTS)
        {
            draw_char_at_px('.', px_pos, clr_gray);
        }
        else //Whole message fits, or we are not yet near the screen edge
        {
            draw_char_at_px(str[i], px_pos, fg);
        }

        px_pos.x += CELL_W;
    }
}

Input get_input()
{
    Input inp;

    bool is_done = false;

    while (!is_done)
    {
        SDL_PollEvent(&sdl_event_);

        switch (sdl_event_.type)
        {
        case SDL_QUIT:
            break;

        case SDL_WINDOWEVENT:
            switch (sdl_event_.window.event)
            {
            case SDL_WINDOWEVENT_RESIZED:
                //on_window_resized();
                break;

            default:
                break;
            }
            break;

        case SDL_MOUSEBUTTONDOWN:
//        {
//            const auto button = sdl_event_.button.button;
//
//            if (button == SDL_BUTTON_LEFT || button == SDL_BUTTON_RIGHT)
//            {
//                d.mouse_btn = button == SDL_BUTTON_LEFT ?
//                              Mouse_Btn::left :
//                              Mouse_Btn::right;
//
//                d.mouse_px_p.set(sdl_event_.button.x, sdl_event_.button.y);
//
//                is_done = true;
//            }
//        }
            break;


        case SDL_MOUSEMOTION:
//            d.mouse_px_p.set(sdl_event_.motion.x, sdl_event_.motion.y);
//            is_done = true;
            break;

        // Text keyboard input (i.e. letter characters and such)
        case SDL_TEXTINPUT:
            inp.c   = sdl_event_.text.text[0];
            is_done = true;
            break;

        // Misc keyboard input (i.e. function keys)
        case SDL_KEYDOWN:
            inp.keycode = sdl_event_.key.keysym.sym;
            is_done     = true;

            // Re-initialize the io module (including script parsing)?
//            if (d.key == SDLK_F5)
//            {
//                // Clear the input data first
//                inp = Input();
//
//                init();
//            }
//            break;

        default:
            break;
        } // switch

        sleep(1);

    } // while

    return inp;
}

} // io
