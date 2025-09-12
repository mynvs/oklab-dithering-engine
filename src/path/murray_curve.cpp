#include <climits>

#include "recursive_space_filling_curve.h" 
#include "murray_curve.h"


extern const Tiling murray_B; //forward ref

const Tiling murray_A = {
    false,  //FOR THE TIME BEING DOESN'T FLIP
    {
      {&murray_A, &murray_B, &murray_A}, //Broken down into curves of same types
      {&murray_B, &murray_A, &murray_B},
      {&murray_A, &murray_B, &murray_A} },
    { {{1,0}, {-1,0}, {1,0}},         //orientation of the curves
      {{1,0}, {-1,0}, {1,0}},
      {{1,0}, {-1,0}, {1,0}} },
    { {{1,0}, {0,1},  {INT_MAX, INT_MAX}},  //direction of travel forward
      {{0,-1}, {0,1}, {0,-1}},
      {{0,-1}, {1,0}, {0,-1}} },
    { {{0, 1}, {-1, 0}, {0, 1}},  //direction of travel backward
      {{0, 1}, {0, -1}, {0, 1}},
      {{INT_MAX, INT_MAX}, {0, -1}, {-1, 0}} }
};

//This is a mirror image of murray_A
const Tiling murray_B = {
    false, //FOR THE TIME BEING DOESN'T FLIP
    { 
      {&murray_B, &murray_A, &murray_B}, //Broken down into curves of same types
      {&murray_A, &murray_B, &murray_A},
      {&murray_B, &murray_A, &murray_B} },
    { {{1,0}, {-1,0}, {1,0}},         //orientation of the curves
      {{1,0}, {-1,0}, {1,0}},
      {{1,0}, {-1,0}, {1,0}} },

    { {{INT_MAX, INT_MAX}, {0,1},  {-1, 0}},  //direction of travel forward
      {{0,-1}, {0,1},  {0,-1}},
      {{0,-1}, {-1,0}, {0,-1}} },
 
    { {{0, 1}, {1, 0}, {0, 1}},  //direction of travel backward
      {{0, 1}, {0, -1}, {0, 1}},
      {{1, 0}, {0, -1}, {INT_MAX, INT_MAX}} }
};
 
Coordinate2D<int> murrayPolygonFindStart(int width, int height) {
    return {0, height-1};
}

Coordinate2D<int> murrayPolygonFindEnd(int width, int height) {
    return {width-1, 0};
}

const RecursiveSpaceFillingCurve murrayPolygonCurve = 
    RecursiveSpaceFillingCurve(3, 3, {0,-1}, {0,-1}, murrayPolygonFindStart, murrayPolygonFindEnd, &murray_A);


