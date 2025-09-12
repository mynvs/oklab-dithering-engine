#include "recursive_space_filling_curve.h"

RecursiveSpaceFillingCurve::RecursiveSpaceFillingCurve(int xOrder, 
                                                       int yOrder,
                                                       Coordinate2D<int> incoming,
                                                       Coordinate2D<int> outgoing,
                                                       Coordinate2D<int>(& findStartCoordinate)(int width, int height),
                                                       Coordinate2D<int>(& findEndCoordinate)(int width, int height),
                                                       const Tiling * const rootTiling) : findStartCoordinate(findStartCoordinate), 
                                                                                          findEndCoordinate(findEndCoordinate),
                                                                                          rootTiling(rootTiling)
{
    this->xOrder = xOrder;
    this->yOrder = yOrder;
    this->incomingVector = incoming;
    this->outgoingVector = outgoing;
}

/* First coordinate processed by RSFC given size of field */
Coordinate2D<int> RecursiveSpaceFillingCurve::start(int width, int height) const {
    return this->findStartCoordinate(width, height);
}

/* Last coordinate processed by RSFC given size of field */
Coordinate2D<int> RecursiveSpaceFillingCurve::end(int width, int height) const {
    return this->findEndCoordinate(width, height);
}

int RecursiveSpaceFillingCurve::getTilingIterations(int width, int height) const {
    int x=1; 
    int y=1;
    int counter = 0;
    while(width>x || height>y) {
        x *= this->xOrder;
        y *= this->yOrder;
        counter +=1;
    }            
    return counter;
}



