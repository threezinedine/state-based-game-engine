#pragma once
#include <NTTEngine/defines.hpp>
#include <NTTEngine/core/parser/json.hpp>

namespace ntt
{
    struct RGBAColor
    {
        u8 r, g, b, a;

        RGBAColor(u8 r = 0, u8 g = 0, u8 b = 0, u8 a = 255)
            : r(r), g(g), b(b), a(a)
        {
        }

        JSON ToJSON() const
        {
            JSON json("{}");
            json.Set("r", r);
            json.Set("g", g);
            json.Set("b", b);
            json.Set("a", a);
            return json;
        }

        void FromJSON(const JSON &json)
        {
            r = json.Get<u8>("r", 0);
            g = json.Get<u8>("g", 0);
            b = json.Get<u8>("b", 0);
            a = json.Get<u8>("a", 255);
        }
    };

#define NTT_BLACK RGBAColor(0, 0, 0, 255)
#define NTT_WHITE RGBAColor(255, 255, 255, 255)
#define NTT_RED RGBAColor(255, 0, 0, 255)
#define NTT_GREEN RGBAColor(0, 255, 0, 255)
#define NTT_BLUE RGBAColor(0, 0, 255, 255)
#define NTT_YELLOW RGBAColor(255, 255, 0, 255)
#define NTT_MAGENTA RGBAColor(255, 0, 255, 255)
#define NTT_CYAN RGBAColor(0, 255, 255, 255)
#define NTT_TRANSPARENT RGBAColor(0, 0, 0, 0)
} // namespace ntt
