#pragma once

#include <cmath>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <utility>
#include <vector>

namespace mygl
{
    struct vec3
    {
        vec3() { data = std::vector<float>(3); }

        explicit vec3(const float (&list)[3]) : data(std::begin(list), std::end(list))
        {
        }

        float& operator[](int index)
        {
            if (index >= 3)
            {
                std::cerr << "Array index out of bound, exiting";
                exit(0);
            }
            return data[index];
        }

        float operator[](int index) const
        {
            if (index >= 3)
            {
                std::cerr << "Array index out of bound, exiting";
                exit(0);
            }
            return data[index];
        }

        std::vector<float> data;
    };
} // namespace mygl

inline mygl::vec3 operator+(const mygl::vec3& lhs, const mygl::vec3& rhs)
{
    mygl::vec3 res;
    for (int i = 0; i < 3; ++i)
        res[i] = lhs[i] + rhs[i];
    return res;
}

inline mygl::vec3 operator-(const mygl::vec3& lhs, const mygl::vec3& rhs)
{
    mygl::vec3 res;
    for (int i = 0; i < 3; ++i)
        res[i] = lhs[i] - rhs[i];
    return res;
}

inline mygl::vec3 operator*(const mygl::vec3& lhs, const mygl::vec3& rhs)
{
    mygl::vec3 res;
    for (int i = 0; i < 3; ++i)
        res[i] = lhs[i] * rhs[i];
    return res;
}

inline mygl::vec3 operator*(const mygl::vec3& lhs, float n)
{
    mygl::vec3 res;
    for (int i = 0; i < 3; ++i)
        res[i] = lhs[i] * n;
    return res;
}

inline mygl::vec3 operator*(float n, const mygl::vec3& rhs)
{
    mygl::vec3 res;
    for (int i = 0; i < 3; ++i)
        res[i] = n * rhs[i];
    return res;
}

inline mygl::vec3 cross(const mygl::vec3& lhs, const mygl::vec3& rhs)
{
    return mygl::vec3({lhs[1] * rhs[2] - lhs[2] * rhs[1],
                       lhs[2] * rhs[0] - lhs[0] * rhs[2],
                       lhs[0] * rhs[1] - lhs[1] - rhs[0]});
}

inline mygl::vec3 normalize(const mygl::vec3& v)
{
    float norm = sqrt(pow(v[0], 2) + pow(v[1], 2) + pow(v[2], 2));
    return mygl::vec3({v[0] / norm, v[1] / norm, v[2] / norm});
}