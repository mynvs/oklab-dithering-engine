#include <memory>
#include <vector>

#ifndef DESIGN_FILTER_1D_H
#define DESIGN_FILTER_1D_H 1

std::shared_ptr<std::vector<float>> blackmanWindow(unsigned int size);

std::shared_ptr<std::vector<float>> decimationFilter(const int numberOfTaps,
                                                     const float decimationRatio,
                                                     const float offset,
                                                     const bool halfFilter,
                                                     float &flatness,
                                                     int &cornerDistance);

std::shared_ptr<std::vector<float>> designDecimationFilter(float decimationRatio, unsigned &filterDelay);

std::shared_ptr<std::vector<float>> designHalfDecimationFilter(float decimationRatio);

#endif
