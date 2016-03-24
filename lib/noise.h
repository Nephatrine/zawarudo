/*
This is free and unencumbered software released into the public domain.

Anyone is free to copy, modify, publish, use, compile, sell, or
distribute this software, either in source code form or as a compiled
binary, for any purpose, commercial or non-commercial, and by any
means.

In jurisdictions that recognize copyright laws, the author or authors
of this software dedicate any and all copyright interest in the
software to the public domain. We make this dedication for the benefit
of the public at large and to the detriment of our heirs and
successors. We intend this dedication to be an overt act of
relinquishment in perpetuity of all present and future rights to this
software under copyright law.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.
*/

#ifndef NOISE_H
#define NOISE_H

#include <random>
#include <array>

namespace noise {

class Perlin {
public:
    Perlin(uint32_t seed=0);

    double noise(double x) const { return noise(x, 0, 0); }
    double noise(double x, double y) const { return noise(x, y, 0); }
    double noise(double x, double y, double z) const;

private:
    std::array<int, 512> p;
};

class PerlinOctave {
public:
    PerlinOctave(int octaves, uint32_t seed=0);

    double noise(double x) const { return noise(x, 0, 0); }
    double noise(double x, double y) const { return noise(x, y, 0); }
    double noise(double x, double y, double z) const;

private:
    Perlin perlin_;
    int octaves_;
};

}

#endif // NOISE_H
