#include "sequential_path.h"
#include "zigzag_path.h"
#include "spiral_path.h"
#include "recursive_space_filling_path.h"
#include "murray_curve.h"
#include "hilbert_curve.h"

struct PathItem {
    const char *name;
    const Path &path;
};

PathItem path_choices[] = {
    "sequential", SequentialPath(),
    "back_and_forth", BidiSequentialPath(),
    "zigzag", ZigZagPath(),
    "spiral", SpiralPath(),
    "double_spiral", DoubleSpiralPath(),
    "murray_polygon", RecursiveSpaceFillingPath(&murrayPolygonCurve),
    "hilbert", RecursiveSpaceFillingPath(&hilbertCurve),
    "moore", RecursiveSpaceFillingPath(&mooreCurve),
};


