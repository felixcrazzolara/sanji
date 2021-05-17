#include <iostream>
#include <limits>

extern float turbo_srgb_floats[256][3];

int main() {
    float min =  std::numeric_limits<float>::max();
    float max = -std::numeric_limits<float>::max();
    for (uint i = 0; i < 256; ++i)
        for (uint j = 0; j < 3; ++j) {
            if (turbo_srgb_floats[i][j] < min) min = turbo_srgb_floats[i][j];
            if (turbo_srgb_floats[i][j] > max) max = turbo_srgb_floats[i][j];
        }
    std::cout << "The minimum is: " << min << std::endl;
    std::cout << "The maximum is: " << max << std::endl;
}
