#define _USE_MATH_DEFINES
#include <cmath>
#include <memory>
#include <vector>

// no IMPLEMENTATION because that encourages openGL
#include "../external/dj_fft.h"

size_t next_pow2(size_t v) { // mitigate for dj_fft
    if (v == 0) return 1;
    --v;
    v |= v >> 1;
    v |= v >> 2;
    v |= v >> 4;
    v |= v >> 8;
    v |= v >> 16;
#if SIZE_MAX > UINT32_MAX
    v |= v >> 32;
#endif
    return ++v;
}


std::shared_ptr<std::vector<float>> blackmanWindow(unsigned int size) {
    auto returnBuffer = std::make_shared<std::vector<float>>();
    for(unsigned i=0; i<size; ++i) {
        float offset = float(i)/(size-1.0);
        returnBuffer->push_back(.42 - .5 * cos(2.0*M_PI*offset) + .08 * cos(4.0*M_PI*offset));
    }
    return returnBuffer;
}

/* Makes all the values in buffer add up to 1 */
void normalize(std::vector<float> &buffer) {
    float sum = 0.0;

    unsigned int i;
    for(i=0; i<buffer.size(); ++i)
        sum += buffer[i];

    for(i=0; i<buffer.size(); ++i)
        buffer[i] /= sum;
}

float sinc(float x) {
    if(x==0) {
        return 1.0;
    } else {
        float normalizedX = M_PI*x;
        return sin(normalizedX)/normalizedX;
    }
}

std::shared_ptr<std::vector<float>> decimationFilter(const int numberOfTaps,
                                                     const float decimationRatio,
                                                     const float offset,
                                                     const bool halfFilter,
                                                     float &flatness,
                                                     int &cornerDistance)
{
    float inverseDecimation = 1.0 / decimationRatio;
    std::vector<float> buffer;
    auto window = blackmanWindow(numberOfTaps);
    for(int i=0; i<numberOfTaps; ++i) {
        float x = float(i) + offset;
        if(i < numberOfTaps/2) {
            buffer.push_back( sinc( inverseDecimation * x ) * inverseDecimation * (*window)[i + numberOfTaps/2] );
        } else {
            if (halfFilter) {
                buffer.push_back( 0.0 );
            } else {
                x = (float)i+offset -numberOfTaps;
                buffer.push_back( sinc( inverseDecimation * x ) * inverseDecimation * (*window)[i - (int)(std::ceil(((float)numberOfTaps)/2))] );
            }
        }
    }
    normalize(buffer);

    std::vector<std::complex<float>> fftBuffer;
    fftBuffer.reserve(buffer.size() * 2);
    for(unsigned int i=0; i<buffer.size(); ++i) {
        fftBuffer.emplace_back(buffer[i], 0.0f);
    }

    size_t n2 = next_pow2(fftBuffer.size());
    if(n2 != fftBuffer.size()){
        fftBuffer.resize(n2, std::complex<float>(0.0f, 0.0f));
    }

    std::vector<std::complex<float>> fftOutputBuffer = dj::fft1d(fftBuffer, dj::fft_dir::DIR_FWD);

    std::vector<float> magnitudes;
    for(unsigned i=0; i<fftBuffer.size(); ++i) {
        magnitudes.push_back(abs(fftOutputBuffer[i]));
    }

    //Measure flatness.
    int halfWidth = fftBuffer.size()/decimationRatio/2;

    float totalError = 0.0;
    float total = 0.0;
    for (int i=-halfWidth; i<halfWidth; ++i) {
        float value;
        if(i>=0) {
            value = magnitudes[i];
        } else {
            value = magnitudes[magnitudes.size() + i];
        }
        total += value;
    }
    float average = total / (float(halfWidth) * 2.0);

    for (int i=-halfWidth; i<halfWidth; ++i) {
        float value;
        if(i>=0) {
            value = magnitudes[i];
        } else {
            value = magnitudes[magnitudes.size() + i];
        }
        totalError  += pow((average-value)/average, 2);
    }

    int cornerDistance0 = 0;
    for (int i=-halfWidth-1; i>=-(int)(magnitudes.size()/2); --i) {
        cornerDistance0 += 1;
        float value = magnitudes[magnitudes.size() + i];
        if(value > average) {
            totalError += pow((average-value)/average, 2);
        }
        if(value < .5) {
            break;
        }
    }
    int cornerDistance1 = 0;
    for(int i=halfWidth; i<(int)(magnitudes.size()/2); ++i) {
        cornerDistance1 += 1;
        float value = magnitudes[i];
        if(value > average) {
            totalError += pow((average-value)/average, 2);
        }
        if(value < .5) {
            break;
        }
    }
    cornerDistance = std::max(cornerDistance0, cornerDistance1);
    flatness = totalError / float(halfWidth * 2);

    auto returnBuffer = std::make_shared<std::vector<float>>();

    //Reverse filter.
    if(halfFilter) {
        int size = buffer.size()/2 - std::floor(offset);
        for(int i=size-1; i>=0; --i) {
            returnBuffer->push_back( buffer[i] );
        }
    } else {
        for(int i=buffer.size()-1; i>=0; --i) {
            returnBuffer->push_back( buffer[i] );
        }
    }
    return returnBuffer;
}

std::shared_ptr<std::vector<float>> designDecimationFilter(float decimationRatio, unsigned &filterDelay) {
    int numberOfTaps = ((int)(std::ceil(decimationRatio*100)) & ~1)+1; //Ensure odd number of taps.
    float flatness;
    int cornerDistance;
    filterDelay = numberOfTaps/2;

    auto output = std::make_shared<std::vector<float>>();
    auto filter_in_fft_order = decimationFilter(numberOfTaps, decimationRatio, 0.0, false, flatness, cornerDistance);


    int size = filter_in_fft_order->size();
    int half_size_down = (int)std::floor((float)size/2);
    int half_size_up = (int)std::ceil((float)size/2);


    for(int i = 0; i< size; ++i) {
        if (i < half_size_up) {
            output->push_back((*filter_in_fft_order)[half_size_down+i]);
        } else {
            //We want round down which is what integer division does.
            output->push_back((*filter_in_fft_order)[i - half_size_up]);
        }
    }
    return output;
}

std::shared_ptr<std::vector<float>> designHalfDecimationFilter(float decimationRatio) {
    int numberOfTaps = std::ceil(decimationRatio*12);
    float topOffset = 0.0;
    float bottomOffset = -decimationRatio/2.0;

    int cornerDistance;
    float bestFlatness = std::numeric_limits<float>::infinity();
    float bestOffset = 0.0;
    bool topChanged = true;
    bool bottomChanged = true;
    float topFlatness;
    float bottomFlatness;

    for(int i=0; i<32; ++i) {
        if(topChanged) {
            decimationFilter(numberOfTaps, decimationRatio, topOffset, true, topFlatness, cornerDistance);
            topChanged = false;
            if(topFlatness < bestFlatness) {
                bestFlatness = topFlatness;
                bestOffset = topOffset;
            }
        }
        if(bottomChanged) {
            decimationFilter(numberOfTaps, decimationRatio, bottomOffset, true, bottomFlatness, cornerDistance);
            bottomChanged = false;
            if(bottomFlatness < bestFlatness) {
                bestFlatness = bottomFlatness;
                bestOffset = bottomOffset;
            }
        }
        if(topFlatness < bottomFlatness) {
            bottomChanged = true;
            bottomOffset = bottomOffset + .25 * (topOffset-bottomOffset);
        } else {
            topChanged = true;
            topOffset = topOffset - .25 * (topOffset-bottomOffset);
        }
    }
    return decimationFilter(numberOfTaps, decimationRatio, bestOffset, true, bottomFlatness, cornerDistance);
}
