#include "matrix4.hh"
#include <iostream>

using namespace mygl;

int main()
{
    auto id = matrix4::identity();
    std::cout << id;
    return 0;
}