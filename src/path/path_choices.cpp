#include <cstddef>
#include <cstring>
#include <memory>
#include "sequential_path.h"
#include "zigzag_path.h"
#include "spiral_path.h"
#include "recursive_space_filling_path.h"
#include "murray_curve.h"
#include "hilbert_curve.h"
#include "asano_curve.h"

#include "path_choices.h"

PathItem path_choices[] = {
    "sequential", SequentialPath(),
    "back_and_forth", BidiSequentialPath(),
    "zigzag", ZigZagPath(),
    "spiral", SpiralPath(),
    "double_spiral", DoubleSpiralPath(),
    "murray_polygon", RecursiveSpaceFillingPath(&murrayPolygonCurve),
    "hilbert", RecursiveSpaceFillingPath(&hilbertCurve),
    "moore", RecursiveSpaceFillingPath(&mooreCurve),
    "asano", RecursiveSpaceFillingPath(&asanoCurve),
    NULL, SequentialPath()
};

std::unique_ptr<Path> get_path(const char *name) {
    int choiceIndex = 0;
    auto foundName = path_choices[choiceIndex].name;

    while(foundName != NULL) {
        if(strcmp(name, foundName)==0) {
            return std::make_unique<Path>(path_choices[choiceIndex].path);
        }
        foundName = path_choices[++choiceIndex].name;
    }
    return NULL;
}
