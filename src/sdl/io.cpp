#include "io.hpp"

#include <SDL_image.h>

#include "config.hpp"

namespace io
{

namespace
{

// TODO: Load some of this stuff from a Lua script or something

const std::string font_dir          = "font";
const std::string font_name         = "terminal10x16.png"; //"terminal10x16.png";
const std::string font_path         = font_dir + "/" + font_name;

const std::string window_title      = "SF RL v0.0.1";

SDL_Window*     sdl_window_         = nullptr;
SDL_Renderer*   sdl_renderer_       = nullptr;

SDL_Surface*    scr_srf_            = nullptr;
SDL_Texture*    scr_texture_        = nullptr;

const int       cell_w              = 10;
const int       cell_h              = 16;
const int       font_img_x_cells    = 16;
const int       font_img_y_cells    = 16;
const size_t    pixel_data_w        = font_img_x_cells * cell_w;
const size_t    pixel_data_h        = font_img_y_cells * cell_h;

const int       screen_bpp          = 32;

bool font_px_data_[pixel_data_w][pixel_data_h];

SDL_Event sdl_event_;

P scr_px_dim()
{
    P out;

    SDL_GetWindowSize(sdl_window_, &out.x, &out.y);

    return out;
}

Uint32 px(SDL_Surface& srf, const int pixel_x, const int pixel_y)
{
    const int BPP = srf.format->BytesPerPixel;

    //p is the address to the pixel we want to retrieve
    Uint8* p = (Uint8*)srf.pixels + pixel_y * srf.pitch + pixel_x * BPP;

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
            const int pixel_x,
            const int pixel_y,
            Uint32 px)
{
    const int BPP = srf.format->BytesPerPixel;

    //p is the address to the pixel we want to set
    Uint8* const p = (Uint8*)srf.pixels + pixel_y * srf.pitch + pixel_x * BPP;

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
    TRACE_FUNC_BEGIN;

    SDL_Surface* font_srf_tmp = IMG_Load(font_path.c_str());

    Uint32 img_clr = SDL_MapRGB(font_srf_tmp->format, 255, 255, 255);

    for (int x = 0; x < font_srf_tmp->w; ++x)
    {
        for (int y = 0; y < font_srf_tmp->h; ++y)
        {
            const bool is_img_px = px(*font_srf_tmp, x, y) == img_clr;

            font_px_data_[x][y] = is_img_px;
        }
    }

    SDL_FreeSurface(font_srf_tmp);

    TRACE_FUNC_END;
}

void put_pixels_on_scr(const bool px_data[pixel_data_w][pixel_data_h],
                       const P& sheet_pos,
                       const P& scr_px_pos,
                       const Clr& clr)
{
    const int px_clr = SDL_MapRGB(scr_srf_->format, clr.r, clr.g, clr.b);

    const int sheet_px_x0 = sheet_pos.x * cell_w;
    const int sheet_px_y0 = sheet_pos.y * cell_h;
    const int sheet_px_x1 = sheet_px_x0 + cell_w - 1;
    const int sheet_px_y1 = sheet_px_y0 + cell_h - 1;
    const int scr_px_x0   = scr_px_pos.x;
    const int scr_px_y0   = scr_px_pos.y;

    int scr_px_x = scr_px_x0;

    for (int sheet_px_x = sheet_px_x0; sheet_px_x <= sheet_px_x1; sheet_px_x++)
    {
        int scr_px_y = scr_px_y0;

        for (int sheet_px_y = sheet_px_y0; sheet_px_y <= sheet_px_y1; sheet_px_y++)
        {
            if (px_data[sheet_px_x][sheet_px_y])
            {
                put_px(*scr_srf_, scr_px_x, scr_px_y, px_clr);
            }
            ++scr_px_y;
        }
        ++scr_px_x;
    }
}

P font_sheet_pos(const unsigned char c)
{
    const int y = c / font_img_x_cells;

    const int x = y == 0 ? c : (c % (y * font_img_x_cells));

    return P(x, y);
}

void draw_rectangle_solid(const P& px_pos,
                          const P& px_dims,
                          const Clr& clr)
{
    SDL_Rect sdl_rect =
    {
        (Sint16)px_pos.x,
        (Sint16)px_pos.y,
        (Uint16)px_dims.x,
        (Uint16)px_dims.y
    };

    SDL_FillRect(scr_srf_,
                 &sdl_rect,
                 SDL_MapRGB(scr_srf_->format, clr.r, clr.g, clr.b));
}

void put_char_pixels_on_scr(const char c,
                            const P& scr_px_pos,
                            const Clr& clr)
{
    const P sheet_pos(font_sheet_pos(c));

    put_pixels_on_scr(font_px_data_,
                      sheet_pos,
                      scr_px_pos,
                      clr);
}

void draw_char_at_px(const char c,
                     const P& px_pos,
                     const Clr& clr)
{
    put_char_pixels_on_scr(c,
                           px_pos,
                           clr);
}

void sleep(const Uint32 duration)
{
    if (duration == 1)
    {
        SDL_Delay(duration);
    }
    else // Duration > 1
    {
        const Uint32 wait_until = SDL_GetTicks() + duration;

        while (SDL_GetTicks() < wait_until)
        {
            SDL_PumpEvents();
        }
    }
}

void clear_events()
{
    while (SDL_PollEvent(&sdl_event_)) {}
}

} // namespace

void init()
{
    TRACE_FUNC_BEGIN;

    cleanup();

    if (SDL_Init(SDL_INIT_EVERYTHING) == -1)
    {
        TRACE << "Failed to init SDL" << std::endl;
        ASSERT(false);
    }

    if (IMG_Init(IMG_INIT_PNG) == -1)
    {
        TRACE << "Failed to init SDL_image" << std::endl;
        ASSERT(false);
    }

    TRACE << "Setting up rendering window" << std::endl;

    const int scr_px_w = scr_default_w * cell_w; //config::scr_px_w();
    const int scr_px_h = scr_default_h * cell_h; //config::scr_px_h();

//    if (config::is_fullscreen())
//    {
//        sdl_window_ = SDL_CreateWindow(window_title.c_str(),
//                                       SDL_WINDOWPOS_UNDEFINED,
//                                       SDL_WINDOWPOS_UNDEFINED,
//                                       scr_px_w, scr_px_h,
//                                       SDL_WINDOW_FULLSCREEN_DESKTOP);
//    }
//
//    if (!config::is_fullscreen() || !sdl_window_)
//    {
    sdl_window_ = SDL_CreateWindow(window_title.c_str(),
                                   SDL_WINDOWPOS_CENTERED,
                                   SDL_WINDOWPOS_CENTERED,
                                   scr_px_w, scr_px_h,
                                   SDL_WINDOW_SHOWN);
//    }

    if (!sdl_window_)
    {
        TRACE << "Failed to create window" << std::endl;
        ASSERT(false);
    }

    sdl_renderer_ = SDL_CreateRenderer(sdl_window_,
                                       -1,
                                       SDL_RENDERER_ACCELERATED);

    if (!sdl_renderer_)
    {
        TRACE << "Failed to create SDL renderer" << std::endl;
        ASSERT(false);
    }

    scr_srf_ = SDL_CreateRGBSurface(0,
                                    scr_px_w,
                                    scr_px_h,
                                    screen_bpp,
                                    0x00FF0000,
                                    0x0000FF00,
                                    0x000000FF,
                                    0xFF000000);

    if (!scr_srf_)
    {
        TRACE << "Failed to create screen surface" << std::endl;
        ASSERT(false);
    }

    scr_texture_ = SDL_CreateTexture(sdl_renderer_,
                                     SDL_PIXELFORMAT_ARGB8888,
                                     SDL_TEXTUREACCESS_STREAMING,
                                     scr_px_w,
                                     scr_px_h);

    if (!scr_texture_)
    {
        TRACE << "Failed to create screen texture" << std::endl;
        ASSERT(false);
    }

    load_font();

    TRACE_FUNC_END;
}

void cleanup()
{
    TRACE_FUNC_BEGIN;

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

    IMG_Quit();

    SDL_Quit();

    TRACE_FUNC_END;
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

void clear_area(const P& p0, const P& dims)
{
    const P cell_px_dim(cell_w, cell_h);

    const P px_p0   (p0     * cell_px_dim);
    const P px_dim  (dims   * cell_px_dim);

    draw_rectangle_solid(px_p0, px_dim, clr_black);
}

void clear_area(const R& r)
{
    const P cell_px_dim(cell_w, cell_h);

    const P px_p0   (r.p0               * cell_px_dim);
    const P px_dim  ((r.p1 - r.p0 + 1)  * cell_px_dim);

    draw_rectangle_solid(px_p0, px_dim, clr_black);
}

P scr_dim()
{
    const P px_dim(scr_px_dim());

    return px_dim / P(cell_w, cell_h);
}

void draw_char(const P& p,
               const char c,
               const Clr fg,
               const Clr bg)
{
    const P px_pos(p * P(cell_w, cell_h));

    draw_rectangle_solid(px_pos,
                         P(cell_w, cell_h),
                         bg);

    draw_char_at_px(c, px_pos, fg);
}

void draw_text(P p,
               const std::string& str,
               const Clr fg,
               const Clr bg,
               const Align align)
{
    const size_t msg_w = str.size();

    // Adjust the x position if right aligned
    if (align == Align::right)
    {
        p.x -= (msg_w - 1);
    }

    const P cell_px_dim(cell_w, cell_h);

    P px_pos(p * cell_px_dim);

    const P scr_px_dim(scr_dim() * cell_px_dim);

    if (px_pos.y < 0 || px_pos.y >= scr_px_dim.y)
    {
        return;
    }

    const int msg_px_w = msg_w * cell_w;

    draw_rectangle_solid(px_pos,
                         P(msg_px_w, cell_h),
                         bg);

    const int   msg_px_x1           = px_pos.x + msg_px_w - 1;
    const bool  msg_w_fit_on_scr    = msg_px_x1 < scr_px_dim.x;

    //X position to start drawing dots instead when the message does not fit horizontally.
    const int   px_x_dots           = scr_px_dim.x - (cell_w * 3);

    for (size_t i = 0; i < msg_w; ++i)
    {
        if (px_pos.x < 0 || px_pos.x >= scr_px_dim.x)
        {
            return;
        }

        const bool draw_dots = !msg_w_fit_on_scr && px_pos.x >= px_x_dots;

        if (draw_dots)
        {
            draw_char_at_px('.', px_pos, clr_gray);
        }
        else //Whole message fits, or we are not yet near the screen edge
        {
            draw_char_at_px(str[i], px_pos, fg);
        }

        px_pos.x += cell_w;
    }
}

Input get_input()
{
    SDL_StartTextInput();

    Input inp;

    bool is_done = false;

    while (!is_done)
    {
        sleep(1);

        const bool did_poll_event = SDL_PollEvent(&sdl_event_);

        if (!did_poll_event)
        {
            continue;
        }

        switch (sdl_event_.type)
        {
        case SDL_QUIT:
        {

        }
        break;

        case SDL_WINDOWEVENT:
        {
            switch (sdl_event_.window.event)
            {
            case SDL_WINDOWEVENT_FOCUS_GAINED:
            case SDL_WINDOWEVENT_RESTORED:
                update_scr();
                break;

            case SDL_WINDOWEVENT_RESIZED:
                //on_window_resized();
                break;

            default:
                break;
            }
        }
        break;

        case SDL_MOUSEBUTTONDOWN:
        {
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
        }
        break;


        case SDL_MOUSEMOTION:
        {
//            d.mouse_px_p.set(sdl_event_.motion.x, sdl_event_.motion.y);
//            is_done = true;
        }
        break;

        // Text keyboard input (i.e. letter characters and such)
        case SDL_TEXTINPUT:
        {
            const char c = sdl_event_.text.text[0];

            if (c >= 33 && c < 126)
            {
                // Valid ASCII character entered
                // ('!' = 33, '~' = 126)
                inp.c = c;
                is_done = true;
            }
            else
            {
                continue;
            }
        }
        break;

        // Misc keyboard input (i.e. function keys)
        case SDL_KEYDOWN:
        {
            const SDL_Keycode sdl_key = sdl_event_.key.keysym.sym;

            // Do not return shift, control or alt as separate key events
            if (
                sdl_key == SDLK_LSHIFT ||
                sdl_key == SDLK_RSHIFT ||
                sdl_key == SDLK_LCTRL  ||
                sdl_key == SDLK_RCTRL  ||
                sdl_key == SDLK_LALT   ||
                sdl_key == SDLK_RALT)
            {
                continue;
            }

            Uint16 mod = SDL_GetModState();

            const bool  IS_SHIFT_HELD = mod & KMOD_SHIFT;
            const bool  IS_CTRL_HELD  = mod & KMOD_CTRL;
            const bool  IS_ALT_HELD   = mod & KMOD_ALT;

            inp.keycode = sdl_key;

            if (sdl_key >= SDLK_F1 && sdl_key <= SDLK_F9)
            {
                // F keys
                is_done = true;
            }
            else // Not an F key
            {
                switch (sdl_key)
                {
                case SDLK_RETURN:
                case SDLK_RETURN2:
                case SDLK_KP_ENTER:
                    if (IS_ALT_HELD)
                    {
                        // TODO: Toggle fullscreen
                        // config::toggle_fullscreen();
                        clear_events();
                        continue;
                    }
                    else // Alt is not held
                    {
                        inp.keycode = SDLK_RETURN;
                        is_done = true;
                    }
                    break;

                case SDLK_SPACE:
                case SDLK_BACKSPACE:
                case SDLK_TAB:
                case SDLK_PAGEUP:
                case SDLK_PAGEDOWN:
                case SDLK_END:
                case SDLK_HOME:
                case SDLK_INSERT:
                case SDLK_DELETE:
                case SDLK_LEFT:
                case SDLK_RIGHT:
                case SDLK_UP:
                case SDLK_DOWN:
                case SDLK_ESCAPE:
                    is_done = true;
                    break;

                case SDLK_MENU:
                case SDLK_PAUSE:
                default:
                    break;
                }
            }
        }
        break;

        default:
            break;
        } // switch

    } // while

    clear_events();

    SDL_StopTextInput();

    return inp;
}

void wait_for_proceed()
{
    while (true)
    {
        const Input d = get_input();

        if (
            d.keycode == SDLK_SPACE     ||
            d.keycode == SDLK_ESCAPE    ||
            d.keycode == SDLK_RETURN)
        {
            break;
        }
    }
}

} // io
