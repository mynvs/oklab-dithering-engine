#ifndef PATH_H
#define PATH_H 1

#include <memory>
#include "float_rectangle.h"

class Path {    
    public:
        Path();

        virtual void advance(std::shared_ptr<FloatRectangle> rectangle, int &current_offset);
        virtual bool ended(std::shared_ptr<FloatRectangle> rectangle, int current_offset) const;
 
        virtual std::shared_ptr<FloatRectangle> pad(std::shared_ptr<FloatRectangle> source);

        virtual int startingOffset(std::shared_ptr<FloatRectangle> rectangle) const;

        // msvc runtime doesn't like this and causes an access violation. cross-platform development is crazy
        virtual std::unique_ptr<Path> clone() const {
            return std::make_unique<Path>();
        }
};

#endif


