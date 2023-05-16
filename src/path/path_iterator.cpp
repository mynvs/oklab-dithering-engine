#include "path_iterator.h"
#include <cmath>

PathIterator::PathIterator(const std::shared_ptr<FloatRectangle> rectangle, std::unique_ptr<Path> path) { 
    this->path=path->clone();
    this->rectangle=this->path->pad(rectangle);
    currentIndex=path->startingOffset(this->rectangle);
};

//Copy constructor
PathIterator::PathIterator(const PathIterator &other) {
    *this = other;
}

PathIterator &PathIterator::operator =(const PathIterator &other) {
    this->rectangle = other.rectangle;
    this->currentIndex = other.currentIndex;
    this->path = other.path->clone();
    return (*this);
}

float PathIterator::operator*() const {
    if(!this->ended()) {
        return rectangle->get(currentIndex);
    } else {
        return std::nan("");
    }
}

float *PathIterator::operator->() {
    if(!this->ended()) {
        return rectangle->get_ptr(currentIndex);
    } else {    
        return NULL;
    }
}

//prefix increment
PathIterator &PathIterator::operator++() {
    this->path->advance(rectangle, this->currentIndex);
    return *this;     
};

PathIterator PathIterator::operator++(int) {
    PathIterator returnValue(*this); ++(*this); return returnValue;
};


