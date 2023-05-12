#include "design_filter_1d.cpp"

int main() {
    float decimation = 2.0;
    float offset = -.5;
    int number_of_taps = std::ceil(decimation*6);
    float flatness;
    int cornerDistance;
    auto filter = decimationFilter(number_of_taps, decimation, offset, true, flatness, cornerDistance);
    printf("flatness: %f\n", flatness);
    printf("corner distance: %d\n", cornerDistance);
}
