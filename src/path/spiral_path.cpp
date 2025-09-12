
#include "spiral_path.h"

SpiralPath::SpiralPath() {
    direction = right;
    inset = 0;
}

SpiralPath::SpiralPath(const SpiralPath &other) {
    this->direction = other.direction;
    this->inset = inset;
}

std::shared_ptr<FloatRectangle> SpiralPath::pad(std::shared_ptr<FloatRectangle> source) {
    auto rectangle = Path::pad(source); 

    if((rectangle->width == 0) || (rectangle->height==0)) {
        direction = done;
    }

    //Deal with single column images
    if (rectangle->width==1) {
        direction = down;

        if(rectangle->height==1) {
            direction = last; //deal with 1x1
        } 
    }
    return rectangle;
}

bool SpiralPath::ended(std::shared_ptr<FloatRectangle> rectangle, int current_offset) const {
    return direction == done;
}
 
void SpiralPath::advance(std::shared_ptr<FloatRectangle> rectangle, int &currentOffset) {
    if(!this->ended(rectangle, currentOffset)) {

        int y = rectangle->getY(currentOffset);
        int x = rectangle->getX(currentOffset);

        switch(direction) {
            case right:
                if (x<(int)rectangle->width-1-inset) {
                    x+=1;
                    if(x >= (int)rectangle->width-1-inset) {
                       direction = down;
                    }
                } else {    
                    direction = done;
                }
                break;

            case down:
                if(y < (int)rectangle->height-1-inset) {
                    y+=1;
                    if(y >= (int)rectangle->height-1-inset) {
                        direction = left;
                    }
                } else {
                    direction = done;
                }
                break;

            case left: 
                if(x> inset) {
                    x-=1;
                    if(x <= inset) {                            
                        inset+=1;
                        direction = up;
                    }
                } else {
                    direction = done;
                }
                break;

            case up:
                if(y> inset) {
                    y-=1;
                    if(y <= inset) {
                        direction = right;
                    } 
                } else {
                    direction = done;
                }
                break;

            case last:
                direction = done;
                break;

            case done:
                break;
        }
        currentOffset = rectangle->getOffset(x, y);           
    }
}

/* Now for double spiral */
DoubleSpiralPath::DoubleSpiralPath(): SpiralPath() {
    turnDirection = 1; //Right turns
    stepOffset = 0;
}

DoubleSpiralPath::DoubleSpiralPath(const DoubleSpiralPath &other) : SpiralPath(other) {
    this->turnDirection = other.turnDirection;
    this->stepOffset = other.stepOffset;
}

void DoubleSpiralPath::advance(std::shared_ptr<FloatRectangle> rectangle, int &currentOffset) {
    if(!this->ended(rectangle, currentOffset)) {

        int y = rectangle->getY(currentOffset);
        int x = rectangle->getX(currentOffset);

        switch(direction) {
            case right:
                if (x<(int)rectangle->width-1-inset) {
                    x+=1;
                    if(x >= (int)rectangle->width-1-inset) {
                        if(turnDirection==1) {
                            inset += 1;
                        }
                        direction =static_cast<Direction> ((static_cast<int>(direction) + turnDirection+4) % 4);
                    }
                } else {                            
                    x+=1; 
                    this->turnDirection = -1;
                }
                break;

            case down:  
                if(y < (int)rectangle->height-1-inset) {
                    y+=1;
                    if(y >= (int)rectangle->height-1-inset) {
                        if(turnDirection==-1) {
                            inset -= 1;
                        }
                        direction =static_cast<Direction> ((static_cast<int>(direction) + turnDirection+4) % 4);
                    }
                } else {
                    y+=1; 
                    this->turnDirection = -1;
                }
                break;

            case left: 
                if(x> inset) {
                    x-=1;
                    if(x <= inset) {
                        if(turnDirection==1) {
                            inset += 1;
                        }
                        direction =static_cast<Direction> ((static_cast<int>(direction) + turnDirection+4) % 4);
                    }
                } else {
                    x-=1; 
                    this->turnDirection = -1;
                }
                break;

            case up:
                if(y> inset) {
                    y-=1;
                    if(y <= inset) {
                        if(turnDirection==-1) {
                            inset -= 1;
                        }
                        direction =static_cast<Direction> ((static_cast<int>(direction) + turnDirection+4) % 4);
                    } 
                } else {
                    y-=1;
                    this->turnDirection = -1;
                }
                break;

            case last:
                direction = done;
                break;

            case done:
                break;
        }
        stepOffset+=1;
        if((turnDirection == 1) && (stepOffset+1 >= (int)(rectangle->width * rectangle->height)/2)) {
            turnDirection = -1;
            inset -= 1;
        }
        if((x<0) || (y<0) || (x>=(int)rectangle->width) || (y>=(int)rectangle->height)) {
            direction = done;
        }
        currentOffset = rectangle->getOffset(x, y);           
    }
}

std::shared_ptr<FloatRectangle> DoubleSpiralPath::pad(std::shared_ptr<FloatRectangle> source) {
    //The smaller axis needs to be odd or double spiral will not work.
    Padding padding=Padding();
    int smaller = std::min(source->width, source->height);
    if ((smaller & 1) == 0) {

        //Sometimes both are even and the same size. In that case both have to be bumped,
        if ((int)source->width == smaller) {
            padding.right = 1;
        }
        if ((int)source->height == smaller) {
            padding.bottom = 1; 
        }
    }
    std::shared_ptr<FloatRectangle> returnRect = SpiralPath::pad(std::make_shared<FloatRectangle>(*source, padding));
    returnRect->depad(); //Calling the function above does one extra layer of padding as side effect, remove it.
    return returnRect;
}


