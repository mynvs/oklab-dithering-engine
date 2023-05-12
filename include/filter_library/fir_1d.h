#include <vector>

#ifndef FIR_1D_H
#define FIR_1D_H 1

template <class T> 
class FirFilter1d {
    private:
        std::vector<T> taps;
        std::vector<T> inputBuffer;
        unsigned int inputOffset;

    public:
        FirFilter1d(std::vector<T> taps) {
            this->taps = taps; //Makes a copy
            for(unsigned i=0; i<taps.size(); ++i) {
                inputBuffer.push_back(0);
            }
            inputOffset = 0;
        } 

        ~FirFilter1d() {
            taps.clear();
            inputBuffer.clear();
        }

        T filter(T input) {
            inputBuffer[inputOffset++]=input;
            if(inputOffset >= inputBuffer.size()) {
                inputOffset = 0;
            }
            T value = 0;
            int counter = 0;
            for(unsigned i=inputOffset; i<inputBuffer.size(); ++i) {
                value += taps[counter++] * inputBuffer[i];
            }
            for(unsigned i=0; i<inputOffset; ++i) {
                value += taps[counter++] * inputBuffer[i];
            }
            return value;
        }
};

#endif

