#pragma once

#include "image.hh"

#include <png++/png.hpp>

struct Color
{
    unsigned char r;
    unsigned char g;
    unsigned char b;
};

struct PNGImage
{
    inline static PNGImage load(const std::string& filename)
    {
        PNGImage res;

        png::image<png::rgb_pixel> image(filename);

        res.width = image.get_width();
        res.height = image.get_height();
        res.pixels = std::vector<Color>(res.width * res.height);

        for (size_t j = 0; j < image.get_height(); ++j)
        {
            for (size_t i = 0; i < image.get_width(); ++i)
            {
                auto pix = image[j][i];
                res.pixels[i + j * res.width] = {pix.red, pix.green, pix.blue};
            }
        }

        return res;
    }

    inline void save(const std::string& filename)
    {
        png::image<png::rgb_pixel> image(width, height);

        for (size_t j = 0; j < height; ++j)
        {
            for (size_t i = 0; i < width; ++i)
            {
                auto pix = pixels[i + j * width];
                image[j][i] = png::rgb_pixel(pix.r, pix.g, pix.b);
            }
        }

        image.write(filename);
    }

    unsigned int height;
    unsigned int width;
    std::vector<Color> pixels;
};