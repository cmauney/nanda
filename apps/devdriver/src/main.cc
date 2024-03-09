#include "nanda/ndarray.hh"
#include <array>
#include <fmt/core.h>

int main(int argc, char* argv[])
{
    fmt::print("HI----\n");

    std::array<int, 16> arr;
    std::fill(arr.begin(), arr.end(), 1);

    return 0;
}
