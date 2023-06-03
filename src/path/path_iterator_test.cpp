#include <cstdio>
#include <stdlib.h>
#include "path_iterator.h"
#include "sequential_path.h"
#include "zigzag_path.h"
#include "spiral_path.h"
#include "recursive_space_filling_path.h"
#include "murray_curve.h"
#include "hilbert_curve.h"
#include "asano_curve.h"

void walk(PathIterator &iterator) {
    int counter =0;

    do {
        printf("%u, %u\n", iterator.getX(), iterator.getY());
        counter ++;
        if (counter > (int)(iterator.getRectangle()->width * iterator.getRectangle()->height)) {
            printf("TOO MANY STEPS\n");
            break;
        }
    } while(!(++iterator).ended());
}

int main() {
    std::shared_ptr<FloatRectangle> rectangle = std::make_shared<FloatRectangle>(8, 8);

    PathIterator pathIterator;
    printf("Sequential\n");
    pathIterator = PathIterator(rectangle, std::make_unique<SequentialPath>());
    walk(pathIterator);

    printf("BidiSequential\n");
    pathIterator = PathIterator(rectangle, std::make_unique<BidiSequentialPath>());
    walk(pathIterator);

    printf("Zigzag\n");
    pathIterator = PathIterator(rectangle, std::make_unique<ZigZagPath>());
    walk(pathIterator);

    printf("Spiral\n");
    pathIterator = PathIterator(rectangle, std::make_unique<SpiralPath>());
    walk(pathIterator);

    printf("Double Spiral\n");
    pathIterator = PathIterator(rectangle, std::make_unique<DoubleSpiralPath>());
    walk(pathIterator);


    printf("Murray Polygon\n");
    printf("-------------------------\n");
    pathIterator = PathIterator(rectangle, std::make_unique<RecursiveSpaceFillingPath>(&murrayPolygonCurve));
    walk(pathIterator);

    printf("Hilbert Curve\n");
    printf("-------------------------\n");
    pathIterator = PathIterator(rectangle, std::make_unique<RecursiveSpaceFillingPath>(&hilbertCurve));
    walk(pathIterator);

    printf("Moore Curve\n");
    printf("-------------------------\n");
    pathIterator = PathIterator(rectangle, std::make_unique<RecursiveSpaceFillingPath>(&mooreCurve));
    walk(pathIterator);

    printf("Asano Curve\n");
    printf("-------------------------\n");
    pathIterator = PathIterator(rectangle, std::make_unique<RecursiveSpaceFillingPath>(&asanoCurve));
    walk(pathIterator);

}

