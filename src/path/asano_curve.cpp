#include <climits>

#include "recursive_space_filling_curve.h"
#include "asano_curve.h"

//Based on a space filling curve from:
//Theoretical Computer Science
//ELSEVIER Theoretical Computer Science 181 (1997) 3-I 5
//Space-filling curves and their use in the design of geometric data structures
//Tetsuo Asano, Desh Ranjan, Thomas Roos, Emo Welzl, Peter Widmayer

//forward references
extern const Tiling asano_B1;
extern const Tiling asano_B2;
extern const Tiling asano_C1;
extern const Tiling asano_C2;
extern const Tiling asano_D1;
extern const Tiling asano_D2;


const Tiling asano_A1 = {
    true,
    { {&asano_C1, &asano_B2},
      {&asano_D2, &asano_B1} },
    { {{0, -1}, {-1, 0}},
      {{1, 0}, {0, 1}} },
    { {{1, 0}, {INT_MAX, INT_MAX}},
      {{1, 0}, {-1, -1}} },
    { {{1, 1}, {-1, 0}},
      {{INT_MAX, INT_MAX}, {-1, 0}}  }
};

const Tiling asano_A2 = {
    true,
    { {&asano_C2, &asano_D1},
      {&asano_B1, &asano_B2} },
    { {{1, 0}, {0, -1}},
      {{0, 1}, {-1, 0}} },
    { {{1, 1}, {INT_MAX, INT_MAX}},
      {{0, -1},  {0, -1}} },
    { {{0, 1}, {0, 1}},
      {{INT_MAX, INT_MAX}, {-1, -1}}  }
};

const Tiling asano_B1 = {
    true,
    { {&asano_C2, &asano_B1},
      {&asano_D1, &asano_B2} },
    { {{1, 0}, {1, 0}},
      {{0, 1}, {0, -1}} },
    { {{1, 0}, {0, 1}},
      {{0, -1}, {INT_MAX, INT_MAX}} },
    { {{0, 1}, {-1, 0}},
      {{INT_MAX, INT_MAX}, {0, -1}}  }
};

const Tiling asano_B2 = {
    true,
    { {&asano_B2, &asano_C1},
      {&asano_B1, &asano_D2} },
    { {{1, 0}, {1, 0}},
      {{0, 1}, {0, -1}} },
    { {{1, 0}, {0, 1}},
      {{0, -1}, {INT_MAX, INT_MAX}} },
    { {{0, 1}, {-1, 0}},
      {{INT_MAX, INT_MAX}, {0, -1}}  }
};

const Tiling asano_C1 = {
    true,
    { {&asano_B1, &asano_A1},
      {&asano_A2, &asano_B2} },
    { {{0, -1}, {0, 1}},
      {{-1, 0}, {0, -1}} },
    { {{1, 0}, {0, 1}},
      {{0, -1}, {INT_MAX, INT_MAX}} },
    { {{0, 1}, {-1, 0}},
      {{INT_MAX, INT_MAX}, {0, -1}}  }
};

const Tiling asano_C2 = {
    true,
    { {&asano_A2, &asano_B2},
      {&asano_B1, &asano_A1} },
    { {{1, 0}, {0, 1}},
      {{0, 1}, {0, -1}} },
    { {{1, 0}, {0, 1}},
      {{0, -1}, {INT_MAX, INT_MAX}} },
    { {{0, 1}, {-1, 0}},
      {{INT_MAX, INT_MAX}, {0, -1}}  }
};

const Tiling asano_D1 = {
    true,
    { {&asano_A2, &asano_C2},
      {&asano_D1, &asano_A2} },
    { {{1, 0}, {0, 1}},
      {{0, 1}, {0, 1}} },
    { {{1, 0}, {0, 1}},
      {{0, -1}, {INT_MAX, INT_MAX}} },
    { {{0, 1}, {-1, 0}},
      {{INT_MAX, INT_MAX}, {0, -1}}  }
};

const Tiling asano_D2 = {
    true,
    { {&asano_C1, &asano_A1},
      {&asano_A1, &asano_D2} },
    { {{0, -1}, {0, 1}},
      {{1, 0}, {0, -1}} },
    { {{1, 0}, {0, 1}},
      {{0, -1}, {INT_MAX, INT_MAX}} },
    { {{0, 1}, {-1, 0}},
      {{INT_MAX, INT_MAX}, {0, -1}}  }
};


Coordinate2D<int> asanoFindStart(int width, int height) {
    return {0, height-1};
}

Coordinate2D<int> asanoFindEnd(int width, int height) {
    return {width-1, 0};
}

const RecursiveSpaceFillingCurve asanoCurve = RecursiveSpaceFillingCurve(2, 2, {1,0}, {1, 0}, asanoFindStart, asanoFindEnd, &asano_A1);


