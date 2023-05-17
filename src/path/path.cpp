
#include "path.h"

Path::Path() {
    //Empty constructor
}
void Path::advance(std::shared_ptr<FloatRectangle> rectangle, int &currentOffset) {        
    if(!this->ended(rectangle, currentOffset)) {
        currentOffset+=1;
    }
}

bool Path::ended(std::shared_ptr<FloatRectangle> rectangle, int currentOffset) const {
    if(currentOffset >= (int)rectangle->getPixels()) {   
        return true;
    } 
    return false;
}

std::shared_ptr<FloatRectangle> Path::pad(std::shared_ptr<FloatRectangle> source) {
    //Add Empty padding
    Padding padding=Padding();
    return std::make_shared<FloatRectangle>(*source, padding);
}

int Path::startingOffset(std::shared_ptr<FloatRectangle> source) const {
    return 0;
}

std::unique_ptr<Path> clone() {
    return std::make_unique<Path>();
}


