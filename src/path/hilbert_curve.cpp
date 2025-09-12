#include <climits>

#include "recursive_space_filling_curve.h"
#include "hilbert_curve.h"

const Tiling hilbert_A = {
    true,  //Definitely applies flips
    {
      {&hilbert_A, &hilbert_A},   //Broken down into curves of same type
      {&hilbert_A, &hilbert_A} },
    { {{1,0}, {1,0}},              //orientation of the curves
      {{0,1}, {0,-1}} },
    { {{1,0}, {0,1}},              //direction of travel forward
      {{0,-1}, {INT_MAX,INT_MAX}} },
    { {{0,1},{-1,0}},             //direction of travel backward
      {{INT_MAX,INT_MAX}, {0, -1}} }
};

Coordinate2D<int> hilbertFindStart(int width, int height) {
    return {0, height-1};
}

Coordinate2D<int> hilbertFindEnd(int width, int height) {
    return {width-1, height-1};
}

const RecursiveSpaceFillingCurve hilbertCurve = RecursiveSpaceFillingCurve(2, 2, {1,0}, {1, 0}, hilbertFindStart, hilbertFindEnd, &hilbert_A);


const Tiling moore_A = {
    false, //Will not work with True
    { {&hilbert_A, &hilbert_A},
      {&hilbert_A, &hilbert_A} },
    { {{0, -1}, {0,1}},           //orientation of curves
      {{0, -1}, {0,1}} },
    { {{1, 0}, {0, 1}},           // direction of travel forward
      {{0, -1}, {INT_MAX, INT_MAX}} },
    { {{0, 1}, {-1, 0}},
      {{INT_MAX, INT_MAX}, {0, -1}} }
};

Coordinate2D<int> mooreFindStart(int width, int height) {
    return {width/2-1, height-1};
}

Coordinate2D<int> mooreFindEnd(int width, int height) {
    return {width/2, height-1};
}

const RecursiveSpaceFillingCurve mooreCurve = RecursiveSpaceFillingCurve(2, 2, {-1,0}, {-1,0}, mooreFindStart, mooreFindEnd, &moore_A);


