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

inline void perspective(mygl::matrix4& mat,
                        float angle, float ratio,
                        const float& z_near,
                        const float& z_far)
{
    float scale = tan(angle * 0.5 * M_PI / 180) * z_near;
    float right = ratio * scale;
    float left = -right;
    float top = scale;
    float bottom = -top;

    float A = (right + left) / (right - left);
    float B = (top + bottom) / (top - bottom);
    float C = -(z_far + z_near) / (z_far - z_near);
    float D = -(2.f * z_far * z_near) / (z_far - z_near);

    mat *= mygl::matrix4({(2.f * z_near) / (right - left), 0, 0, 0,
                          0, (2.f * z_near) / (top - bottom), 0, 0,
                          A, B, C, -1,
                          0, 0, D, 0});
}

inline void look_at(mygl::matrix4& mat,
                    const mygl::vec3& eye,
                    const mygl::vec3& center,
                    const mygl::vec3& up)
{
    mygl::vec3 f = normalize(center - eye);
    mygl::vec3 s = normalize(cross(f, normalize(up)));
    mygl::vec3 u = cross(s, f);

    mat *= mygl::matrix4({s[0], u[0], -f[0], 0,
                          s[1], u[1], -f[1], 0,
                          s[2], u[2], -f[2], 0,
                          -dot(s, eye), -dot(u, eye), dot(f, eye), 1});
}