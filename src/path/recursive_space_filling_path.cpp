
#include "recursive_space_filling_path.h"

RecursiveSpaceFillingPath::RecursiveSpaceFillingPath(const RecursiveSpaceFillingCurve * const curve) : Path(), curve(curve) {
    tilingLevel = 0;
    doneFlag = false;
}

RecursiveSpaceFillingPath::RecursiveSpaceFillingPath(const RecursiveSpaceFillingPath &other) {
    this->curve = other.curve;
    this->tilingLevel = other.tilingLevel;
    this->doneFlag = other.doneFlag;
}

std::shared_ptr<FloatRectangle> RecursiveSpaceFillingPath::pad(std::shared_ptr<FloatRectangle> source) {
    Padding padding=Padding();
    this->tilingLevel = curve->getTilingIterations(source->width, source->height);
    int width = std::pow(curve->getXOrder(), tilingLevel);
    int height = std::pow(curve->getYOrder(), tilingLevel);

    padding.left = (width - source->width)/2;
    padding.top = (height - source->height)/2;
    padding.right = width - padding.left - source->width;
    padding.bottom = height - padding.top - source->height;

    //Get start coordinate
    return std::make_shared<FloatRectangle>(*source, padding);
}

int RecursiveSpaceFillingPath::startingOffset(std::shared_ptr<FloatRectangle> source) const {
    Coordinate2D<int> startCoordinate = curve->start(source->width, source->height);
    return source->getOffset(startCoordinate.x, startCoordinate.y);           
}

bool RecursiveSpaceFillingPath::ended(std::shared_ptr<FloatRectangle> rectangle, int current_offset) const {
    return this->doneFlag;
}

void RecursiveSpaceFillingPath::advance(std::shared_ptr<FloatRectangle> rectangle, int &currentOffset) {
    if(!this->ended(rectangle, currentOffset)) {
        int y = rectangle->getY(currentOffset);
        int x = rectangle->getX(currentOffset);


        Coordinate2D<int> endCoordinate =  curve->end(rectangle->width, rectangle->height);
        if(endCoordinate.x==x && endCoordinate.y==y) {
            doneFlag=true;
        }

        Coordinate2D<int> direction = findPixelDirection(x, y);
        if(! doneFlag ) {
            x += direction.x;
            y += direction.y;
        }

        currentOffset = rectangle->getOffset(x, y);           
    }
}

Coordinate2D<int> RecursiveSpaceFillingPath::mirrorDirection(const Coordinate2D<int> newDirection) {
    if(newDirection.x == INT_MAX) {
        return newDirection; //Do not modify
    } else {
        Coordinate2D<int> returnDirection = newDirection;

        returnDirection.x = -returnDirection.x;
        returnDirection.y = - returnDirection.y;
        return returnDirection;
    }
}

Coordinate2D<int> RecursiveSpaceFillingPath::rotateDirection( const Coordinate2D<int> &orientation, const Coordinate2D<int> newDirection ) {
    if(newDirection.x == INT_MAX) {
        return newDirection; //Do not change value
    }

    /* Do complex multiply */
    Coordinate2D<int> returnDirection = { orientation.x * newDirection.x - orientation.y * newDirection.y,
                                          orientation.y * newDirection.x + orientation.x * newDirection.y };
    return returnDirection;
}

/* This assumes the inpuiArray is curve->XOrder * curve->YOrder in size
 * arrange so y coordinate is listed first.
 */
template <typename T> 
std::vector<std::vector<T>> RecursiveSpaceFillingPath::rotateArray(const Coordinate2D<int> orientation, 
                                                                   std::vector<std::vector<T>> inputArray) {
    T dummyValue = inputArray[0][0];                    
    int xOrder = curve->getXOrder();
    int yOrder = curve->getYOrder();


    std::vector<std::vector<T>> rotatedArray;
    for(int y=0; y<yOrder; ++y) {
        std::vector<T> newRow = {};
        for(int x=0; x<xOrder; ++x) {
            newRow.push_back(dummyValue);
        }
        rotatedArray.push_back(newRow);
    }
    // Use coordinate system that ensures centered around middle..if we go from [-(max-1), (max-1)]             
    // We can renormalize by adding (max-1) and dividing by 2.
    for(int x= -(xOrder-1); x < xOrder; x+=2) {
         for(int y=-(yOrder-1); y< yOrder; y+=2) {
            int inXIndex = (x+(xOrder-1))>>1;
            int inYIndex = (y+(yOrder-1))>>1;

            /* Complex multiply */
            int newX = x*orientation.x - y*orientation.y;
            int newY = x*orientation.y + y*orientation.x;

            int outXIndex = (newX+(xOrder-1))>>1;
            int outYIndex = (newY+(yOrder-1))>>1;
            rotatedArray[outYIndex][outXIndex] = inputArray[inYIndex][inXIndex];
        }
    }
    return rotatedArray; 
}

void RecursiveSpaceFillingPath::rotateArrayElements(Coordinate2D<int> orientation, 
                                                    std::vector<std::vector<Coordinate2D<int>>> &workArray) {
    for(int y=0; y<curve->getYOrder(); ++y) {
        for(int x=0; x<curve->getXOrder(); ++x) {
            workArray[y][x] = rotateDirection(orientation, workArray[y][x]);
        }
    }
}
 
bool RecursiveSpaceFillingPath::mirrorCheck(Coordinate2D<int> direction, const std::vector<std::vector<Coordinate2D<int>>> &inputArray) {
    int xOrder = curve->getXOrder();
    int yOrder = curve->getYOrder();

    //CURRENTLY assumes we always exit a space filling curve tile on a corner point

    if(direction.x > 0) { 
        if((inputArray[0][0].x==INT_MAX) || (inputArray[yOrder-1][0].x==INT_MAX)) {
            return true;
        }
    }
    else if (direction.x < 0) {
        if((inputArray[0][xOrder-1].x==INT_MAX) || (inputArray[yOrder-1][xOrder-1].x==INT_MAX)) {
            return true;
        }
    }
    if(direction.y < 0) {
        if((inputArray[yOrder-1][0].x==INT_MAX) || (inputArray[yOrder-1][xOrder-1].x==INT_MAX)) {
            return true;
        }
    } else if( direction.y > 0) {
        if((inputArray[0][0].x==INT_MAX) || (inputArray[0][xOrder-1].x==INT_MAX)) {
            return true;
        }
    }
    return false;
}

void RecursiveSpaceFillingPath::replaceUnknown(std::vector<std::vector<Coordinate2D<int>>> &workingArray, const Coordinate2D<int> currentDirection) {
    int xOrder = curve->getXOrder();
    int yOrder = curve->getYOrder();

    for(int y=0; y<yOrder; ++y) {
        for(int x=0; x<xOrder; ++x) {
            if (workingArray[y][x].x==INT_MAX) {
                workingArray[y][x] = currentDirection;
                return;
            }
        }
    }
}

const Coordinate2D<int> RecursiveSpaceFillingPath::findPixelDirection(int x, int y) {
    int xOrder = curve->getXOrder();
    int yOrder = curve->getYOrder();
    
    Coordinate2D<int> previousDirection = mirrorDirection(curve->getIncoming());
    Coordinate2D<int> currentDirection = curve->getIncoming();
    Coordinate2D<int> currentOrientation = {1, 0}; //No rotation, identity complex number.
    const Tiling * currentTiling = curve->getRootTiling();

    int lowX = 0;
    int highX = std::pow(xOrder, tilingLevel);
    int lowY = 0;
    int highY = std::pow(yOrder, tilingLevel);

    for(int i=0; i<tilingLevel; ++i) {
#if 0
        printf("Tiling level %d for (%d,%d)\n", i, x, y);
#endif

        /* Get current tiling info array */
        std::vector<std::vector<const Tiling *>> mapping = rotateArray(currentOrientation, currentTiling->mapping);               
        std::vector<std::vector<Coordinate2D<int>>> orient = rotateArray(currentOrientation, currentTiling->orient);               
        rotateArrayElements(currentOrientation, orient);               
        std::vector<std::vector<Coordinate2D<int>>> initialPrevious = rotateArray(currentOrientation, currentTiling->previous);
        rotateArrayElements(currentOrientation, initialPrevious);               
        std::vector<std::vector<Coordinate2D<int>>>initialNext = rotateArray(currentOrientation, currentTiling->next);
        rotateArrayElements(currentOrientation, initialNext);               
        auto previous = &initialPrevious;
        auto next = &initialNext;

       
#if 0  
        //KEPT in for debugging for the moment, at least till Asano curve works.      
        for(int y=0; y<yOrder; ++y) {
            for(int x=0; x<xOrder; ++x) {
                printf("(%d, %d) ", orient[y][x].x, orient[y][x].y);
            }
            for(int x=0; x<xOrder; ++x) {
                printf("(%d, %d) ", (*next)[y][x].x, (*next)[y][x].y);
            }
            for(int x=0; x<xOrder; ++x) {
                printf("(%d, %d) ", (*previous)[y][x].x, (*previous)[y][x].y);
            }
        }
        printf("\n");
#endif
        bool mirror = false;

        if(currentTiling->flips) {
            mirror = mirrorCheck(previousDirection, *previous);
            mirror = mirror || mirrorCheck(currentDirection, *next);
            
            if(mirror) {
                auto temp = previous;
                previous = next;
                next = temp;
                
            }
        }
        replaceUnknown(*previous, previousDirection); 
        replaceUnknown(*next, currentDirection); 
        
        //IDENTIFY SECTOR/Quadrant
        int ySectorSize = ((highY-lowY)/curve->getYOrder());
        int xSectorSize = ((highX-lowX)/curve->getXOrder());

        int sectorY = (y - lowY) / ySectorSize;
        int sectorX = (x - lowX) / xSectorSize;

#if 0
        printf("Sector: %d, %d\n", sectorX, sectorY);
#endif

        currentTiling = mapping[sectorY][sectorX];
        currentOrientation = orient[sectorY][sectorX];

#if 0
        for(int y=0; y<yOrder; ++y) {
            for(int x=0; x<xOrder; ++x) {
                printf("(%d, %d) ", orient[y][x].x, orient[y][x].y);
            }
            printf("\t");
            for(int x=0; x<xOrder; ++x) {
                printf("(%d, %d) ", (*next)[y][x].x, (*next)[y][x].y);
            }
            printf("\n");
        }
        printf("\n");
#endif

        currentDirection = (*next)[sectorY][sectorX];
        previousDirection = (*previous)[sectorY][sectorX];
      
#if 0  
        printf("Orientation: (%d,%d) Direction: (%d, %d)\n", currentOrientation.x, currentOrientation.y, currentDirection.x, currentDirection.y);
#endif
        //Fix ranges.
        lowY = lowY + sectorY*ySectorSize;
        highY = lowY + ySectorSize;

        lowX = lowX + sectorX*ySectorSize;
        highX = lowX + xSectorSize;
    }
#if 0
    printf("Final direction: (%d, %d)\n" , currentDirection.x, currentDirection.y);
#endif
    return currentDirection;
}



