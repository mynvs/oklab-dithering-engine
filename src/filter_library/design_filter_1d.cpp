#include <cmath>
#include <iterator>
#include <memory>
#include <vector>
#include "pocketfft_hdronly.h"

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

    for(unsigned i=0; i<buffer.size(); ++i) {
        sum += buffer[i];
    }
    
    for(unsigned i=0; i<buffer.size(); ++i) {
        buffer[i] = buffer[i]/sum;
    }
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
                x = numberOfTaps - x;
                buffer.push_back( sinc( inverseDecimation * x ) * inverseDecimation * (*window)[i - numberOfTaps/2] );
            }
        }
    }
    normalize(buffer);
   
    std::vector<std::complex<float>> fftBuffer;
    for(unsigned int i=0; i<buffer.size(); ++i) {
        fftBuffer.push_back( buffer[i] );
    }
    //Zero pad the fft buffer.
    std::vector<std::complex<float>>::iterator it = fftBuffer.begin();
    std::advance(it, numberOfTaps/2);
    fftBuffer.insert(it, 15*buffer.size(), 0.0);


    std::vector<std::complex<float>> fftOutputBuffer;
    fftOutputBuffer.insert(fftOutputBuffer.end(), fftBuffer.size(), 0.0); //Fill with zeroes.

    pocketfft::shape_t shape{fftBuffer.size()}; 
    pocketfft::stride_t strideInput(shape.size());
    pocketfft::stride_t strideOutput(shape.size());
    strideInput[0] = sizeof(std::complex<float>);
    strideOutput[0] = sizeof(std::complex<float>);
    pocketfft::shape_t axes;
    axes.push_back(0);

    //Time to do the fft.
    pocketfft::c2c(shape, strideInput, strideOutput, axes, true, fftBuffer.data(), fftOutputBuffer.data(), 1.0f); ///float(fftBuffer.size()));

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
        int size = halfWidth - std::floor(offset);
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

