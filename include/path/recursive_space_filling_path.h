#ifndef RECURSIVE_SPACE_FILLING_PATH_H 
#define RECURSIVE_SPACE_FILLING_PATH_H 1

#include <cstdio> //TODO--remove
#include <climits>
#include <cmath>
#include <memory>

#include "recursive_space_filling_curve.h"
#include "path.h"

/* General Tile Based Recursively Defined Curve Walker */
class RecursiveSpaceFillingPath: public Path {
    protected:
        const RecursiveSpaceFillingCurve *curve;    
        int tilingLevel;
        bool doneFlag;

    public:
        RecursiveSpaceFillingPath(const RecursiveSpaceFillingCurve * const curve);

        RecursiveSpaceFillingPath(RecursiveSpaceFillingPath &other);

        std::shared_ptr<FloatRectangle> pad(std::shared_ptr<FloatRectangle> source) override;

        virtual int startingOffset(std::shared_ptr<FloatRectangle> source) const override;

        bool ended(std::shared_ptr<FloatRectangle> rectangle, int current_offset) const;

        virtual void advance(std::shared_ptr<FloatRectangle> rectangle, int &currentOffset) override;

        virtual std::unique_ptr<Path> clone() override {
            return std::make_unique<RecursiveSpaceFillingPath>(*this);
        }

    private:
        Coordinate2D<int> mirrorDirection(const Coordinate2D<int> newDirection);

        Coordinate2D<int> rotateDirection( const Coordinate2D<int> &orientation, const Coordinate2D<int> newDirection );

        /* This assumes the inpuiArray is curve->XOrder * curve->YOrder in size
         * arrange so y coordinate is listed first.
         */
        template <typename T> 
        std::vector<std::vector<T>> rotateArray( const Coordinate2D<int> orientation, 
                                                 std::vector<std::vector<T>> inputArray);
       
        void rotateArrayElements( Coordinate2D<int> orientation, 
                                  std::vector<std::vector<Coordinate2D<int>>> &workArray);
         
        bool mirrorCheck(Coordinate2D<int> direction, const std::vector<std::vector<Coordinate2D<int>>> &inputArray);

        void replaceUnknown(std::vector<std::vector<Coordinate2D<int>>> &workingArray, const Coordinate2D<int> currentDirection);

        const Coordinate2D<int> findPixelDirection(int x, int y);
};

#endif


