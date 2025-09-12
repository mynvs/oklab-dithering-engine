#ifndef RECURSIVE_SPACE_FILLING_CURVE_H 
#define RECURSIVE_SPACE_FILLING_CURVE_H 1

#include <vector>

template <class T>
struct Coordinate2D {
    public:
        Coordinate2D() {            
        }

        Coordinate2D(T x, T y) {
            this->x = x;
            this->y = y;
        }
    
        T x;
        T y;
};
 
struct Tiling {
    bool flips;
    std::vector<std::vector<const Tiling *>> mapping;
    std::vector<std::vector<Coordinate2D<int>>> orient;
    std::vector<std::vector<Coordinate2D<int>>> next;
    std::vector<std::vector<Coordinate2D<int>>> previous;
};

class RecursiveSpaceFillingCurve {
    protected:
        int xOrder; //the x width of the buffer must be of a width xOrder^n where n is an integer.
        int yOrder; //the y height of the buffer must be of a height yOrder^n where n is an integer (and the same n 
                    //as used for xOrder).

        Coordinate2D<int> incomingVector; //Incoming step direction x,y vector. can usually be any valid direction from off buffer
                                          //to the first coordinate (at iteration 0)
 
        Coordinate2D<int> outgoingVector; //Outgoing step direction x,y vector. can usually be any valid direction to off buffer
                                          //from the last coordinate (at iteration 0)


        Coordinate2D<int>(& findStartCoordinate)(int width, int height); //Reference to a routine to find first coordinate
        Coordinate2D<int>(& findEndCoordinate)(int width, int height); //Reference to a routine to find last coordinate
        const Tiling * const rootTiling;

    public:
        RecursiveSpaceFillingCurve(int xOrder, 
                                   int yOrder,
                                   Coordinate2D<int> incoming,
                                   Coordinate2D<int> outgoing,
                                   Coordinate2D<int>(& findStartCoordinate)(int width, int height),
                                   Coordinate2D<int>(& findEndCoordinate)(int width, int height),
                                   const Tiling * const rootTiling);
        int getXOrder() const {
            return xOrder;
        }
        
        int getYOrder() const {
            return yOrder;
        }

        Coordinate2D<int> getIncoming()  const {
            return incomingVector;
        }
        
        Coordinate2D<int> getOutgoing() const {
            return outgoingVector;
        }

        /* First coordinate processed by RSFC given size of field */
        Coordinate2D<int> start(int width, int height) const;

        /* Last coordinate processed by RSFC given size of field */
        Coordinate2D<int> end(int width, int height) const;

        const Tiling * getRootTiling() const {
            return rootTiling;
        }

        int getTilingIterations(int width, int height) const;
 
};

#endif


