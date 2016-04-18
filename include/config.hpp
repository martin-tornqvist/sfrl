#ifndef CONFIG_HPP
#define CONFIG_HPP

const int msg_area_min_w        = 100;
const int msg_area_default_w    = msg_area_min_w;
const int msg_area_h            = 1;

const int inf_area_y0           = 1;
const int inf_area_w            = 12;
const int inf_area_min_h        = 21;
const int inf_area_default_h    = 40;

const int map_area_x0           = inf_area_w;
const int map_area_y0           = msg_area_h;

const int scr_default_w         = msg_area_default_w;
const int scr_default_h         = msg_area_h + inf_area_default_h;

#endif // CONFIG_HPP
