#pragma once

#include <cmath>
#include <iomanip>
#include <sstream>
#include <utility>
#include <vector>

#include "vec3.hh"

namespace mygl
{
    struct matrix4
    {
        matrix4() { data = std::vector<float>(4 * 4); }

        explicit matrix4(const float (&list)[16]) : data(std::begin(list), std::end(list))
        {
        }

        inline static matrix4 identity()
        {
            return matrix4({1, 0, 0, 0,
                            0, 1, 0, 0,
                            0, 0, 1, 0,
                            0, 0, 0, 1});
        }

        inline void operator*=(const matrix4& rhs)
        {
            std::vector<float> tmp(4 * 4, 0);

            for (int i = 0; i < 4; ++i)
            {
                for (int j = 0; j < 4; ++j)
                {
                    for (int k = 0; k < 4; ++k)
                        tmp[i + j * 4] += data[i + k * 4] * rhs.data[k + j * 4];
                }
            }

            data = tmp;
        }

        std::vector<float> data;
    };
} // namespace mygl

inline static size_t number_of_digits(double n)
{
    std::ostringstream strs;
    strs << n;
    return strs.str().size();
}

inline std::ostream& operator<<(std::ostream& out, const mygl::matrix4& m)
{
    size_t max_len_per_column[4];

    for (size_t j = 0; j < 4; ++j)
    {
        size_t max_len{};

        for (size_t i = 0; i < 4; ++i)
            if (const auto num_length{number_of_digits(m.data[i + j * 4])}; num_length > max_len)
                max_len = num_length;

        max_len_per_column[j] = max_len;
    }

    for (size_t i = 0; i < 4; ++i)
        for (size_t j = 0; j < 4; ++j)
            out << (j == 0 ? "\n| " : "") << std::setw(max_len_per_column[j]) << m.data[i + j * 4] << (j == 3 ? " |" : " ");

    return out << '\n';
}

inline void frustum(mygl::matrix4& mat,
                    const float& left,
                    const float& right,
                    const float& bottom,
                    const float& top,
                    const float& z_near,
                    const float& z_far)
{
    float A = (right + left) / (right - left);
    float B = (top + bottom) / (top - bottom);
    float C = (z_far + z_near) / (z_far - z_near);
    float D = (2.f * z_far * z_near) / (z_far - z_near);

    auto rhs = mygl::matrix4({(2.f * z_near) / (right - left), 0, A, 0,
                              0, (2.f * z_near) / (top - bottom), B, 0,
                              0, 0, C, D,
                              0, 0, -1, 0});

    mat *= rhs;
}

inline void look_at(mygl::matrix4& mat,
                    const mygl::vec3& eye,
                    const mygl::vec3& center,
                    const mygl::vec3& up)
{
    mygl::vec3 forward = normalize(center - eye);
    mygl::vec3 side = normalize(cross(forward, normalize(up)));
    mygl::vec3 up_ = normalize(cross(side, forward));

    mat *= mygl::matrix4({side[0], side[1], side[2], 0,
                          up_[0], up_[1], up_[2], 0,
                          -forward[0], -forward[1], -forward[2], 0,
                          0, 0, 0, 1});

    mat *= mygl::matrix4({1, 0, 0, -eye[0],
                          0, 1, 0, -eye[1],
                          0, 0, 1, -eye[2],
                          0, 0, 0, 1});
}