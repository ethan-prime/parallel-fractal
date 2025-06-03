#pragma once
#include <cmath>
#include <functional>
#include "render.h"

#define COLOR_BLACK color_t{0, 0, 0}
#define COLOR_WHITE color_t{255, 255, 255}
#define COLOR_RED color_t{255, 0, 0}
#define COLOR_GREEN color_t{0, 255, 0}
#define COLOR_BLUE color_t{0, 0, 255}
#define COLOR_YELLOW color_t{255, 255, 0}
#define COLOR_CYAN color_t{0, 255, 255}
#define COLOR_MAGENTA color_t{255, 0, 255}
#define COLOR_ORANGE color_t{255, 128, 0}
#define COLOR_PURPLE color_t{128, 0, 255}
#define COLOR_PINK color_t{255, 64, 192}
#define COLOR_BROWN color_t{128, 64, 0}
#define COLOR_DARK_BLUE color_t{10, 20, 80}
#define COLOR_DARK_RED color_t{80, 10, 10}
#define COLOR_GOLD color_t{255, 215, 0}
#define COLOR_TEAL color_t{0, 128, 128}
#define COLOR_LIME color_t{128, 255, 0}
#define COLOR_SKY color_t{135, 206, 235}
#define COLOR_VIOLET color_t{148, 0, 211}
#define COLOR_SILVER color_t{192, 192, 192}
#define COLOR_GRAY color_t{128, 128, 128}

typedef std::function<color_t(f64)> colormap_t;

inline color_t wikipedia_colormap(f64 t)
{
    if (t < 0)
        t = 0;
    if (t > 1)
        t = 1;
    if (t >= 1.0 - 1e-8)
        return COLOR_DARK_BLUE;
    f64 t_gamma = std::pow(t, GAMMA);
    f64 r = 9 * (1 - t_gamma) * t_gamma * t_gamma * t_gamma;
    f64 g = 15 * (1 - t_gamma) * (1 - t_gamma) * t_gamma * t_gamma;
    f64 b = 8.5 * (1 - t_gamma) * (1 - t_gamma) * (1 - t_gamma) * t_gamma + (1 - t_gamma) * 0.2;
    auto clamp = [](f64 x)
    { if (x < 0) return 0.0; if (x > 1) return 1.0; return x; };
    return color_t{
        static_cast<channel>(clamp(r) * 0xFF),
        static_cast<channel>(clamp(g) * 0xFF),
        static_cast<channel>(clamp(b) * 0xFF)};
}

inline color_t buddha_bright_colormap_hue(f64 t, color_t hue)
{
    if (t < 0)
        t = 0;
    if (t > 1)
        t = 1;
    f64 t_gamma = std::pow(t, GAMMA);
    f64 val = std::pow(t_gamma, 0.8);
    channel r = static_cast<channel>(hue.Red * val);
    channel g = static_cast<channel>(hue.Green * val);
    channel b = static_cast<channel>(hue.Blue * val);
    return color_t{r, g, b};
}

inline color_t buddha_bright_colormap(f64 t) { return buddha_bright_colormap_hue(t, COLOR_WHITE); }
inline color_t buddha_bright_blue(f64 t) { return buddha_bright_colormap_hue(t, COLOR_BLUE); }
inline color_t buddha_bright_red(f64 t) { return buddha_bright_colormap_hue(t, COLOR_RED); }
inline color_t buddha_bright_gold(f64 t) { return buddha_bright_colormap_hue(t, COLOR_GOLD); }
inline color_t buddha_bright_sky(f64 t) { return buddha_bright_colormap_hue(t, COLOR_SKY); }

inline color_t grayscale_colormap(f64 t)
{
    if (t < 0)
        t = 0;
    if (t > 1)
        t = 1;
    channel c = static_cast<channel>(t * 255);
    return color_t{c, c, c};
}

inline color_t linear_gradient_colormap(f64 t, color_t color1, color_t color2)
{
    if (t < 0)
        t = 0;
    if (t > 1)
        t = 1;
    channel r = static_cast<channel>(color1.Red + (color2.Red - color1.Red) * t);
    channel g = static_cast<channel>(color1.Green + (color2.Green - color1.Green) * t);
    channel b = static_cast<channel>(color1.Blue + (color2.Blue - color1.Blue) * t);
    return color_t{r, g, b};
}

#define GRADIENT(C1, C2) ([](f64 t) { return linear_gradient_colormap(t, C1, C2); })

inline color_t black_to_sky(f64 t) { return linear_gradient_colormap(t, COLOR_BLACK, COLOR_SKY); }

// Burning Ship "fire" colormap: black ship, orange/yellow/white background, with more visible color gradation
inline color_t burning_ship_fire_colormap(f64 t)
{
    if (t > .95)
        return linear_gradient_colormap((t-.95)/.05, COLOR_RED, COLOR_BLACK);
    else if (t > .05)
        return linear_gradient_colormap((t-.05)/.9, COLOR_ORANGE, COLOR_RED);
    else if (t > .01)
        return linear_gradient_colormap((t-.01)/.05, COLOR_YELLOW, COLOR_ORANGE);
    else
        return linear_gradient_colormap(t/.01, COLOR_RED, COLOR_YELLOW);
}