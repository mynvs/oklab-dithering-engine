#ifndef SPIRAL_PATH_H 
#define SPIRAL_PATH_H 1

#include "path.h"
 
enum Direction { right=0, down=1, left=2, up=3, last=4, done=5 };

/* Spirals through data set */
class SpiralPath: public Path { 
    protected:
        enum Direction direction;
        unsigned inset = 0;

       
    public:
        SpiralPath();

        SpiralPath(SpiralPath &other);

        virtual std::shared_ptr<FloatRectangle> pad(std::shared_ptr<FloatRectangle> source) override;

        bool ended(std::shared_ptr<FloatRectangle> rectangle, int current_offset) const;
 
        virtual void advance(std::shared_ptr<FloatRectangle> rectangle, int &currentOffset) override;

        virtual std::unique_ptr<Path> clone() override {
            return std::make_unique<SpiralPath>(*this);
        }

};

/* Double Spirals through data set */
class DoubleSpiralPath: public SpiralPath {
    protected:
        int turnDirection;
        unsigned stepOffset =0;

    public:
        DoubleSpiralPath();

        DoubleSpiralPath(DoubleSpiralPath &other);

        virtual void advance(std::shared_ptr<FloatRectangle> rectangle, int &currentOffset) override;
                                    
        virtual std::shared_ptr<FloatRectangle> pad(std::shared_ptr<FloatRectangle> source) override;

        virtual std::unique_ptr<Path> clone() override {
            return std::make_unique<DoubleSpiralPath>(*this);
        }
};

#endif

