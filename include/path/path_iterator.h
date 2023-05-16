#ifndef PATH_ITERATOR_H
#define PATH_ITERATOR_H 1

#include <memory>
#include "path.h"
#include "float_rectangle.h"
#include "sequential_path.h"

class PathIterator {    
    protected:
        std::shared_ptr<FloatRectangle> rectangle=NULL;
        std::unique_ptr<Path> path;
        int currentIndex;

    public:
        using iterator_category = std::forward_iterator_tag;
        using difference_type = int;
        using value_type = float;
        using pointer = int;
        using reference = float &;

        PathIterator() {
            rectangle = NULL;
            path=std::make_unique<SequentialPath>();
            currentIndex=0; 
        }
 
        //Copy constructor
        PathIterator(const PathIterator &other);
 
        PathIterator(const std::shared_ptr<FloatRectangle> rectangle, std::unique_ptr<Path> path);

        virtual PathIterator &operator =(const PathIterator &other);
      
        std::shared_ptr<FloatRectangle> getRectangle() {
            return rectangle;
        }
        
        unsigned getX() {
            return rectangle->getX(currentIndex);
        }
 
        unsigned getY() {
            return rectangle->getY(currentIndex);
        }

        float operator*() const;

        float *operator->();
       
        bool ended() const {  
            return path->ended(rectangle, currentIndex);
        }

        //prefix increment
        PathIterator &operator++();

        //postfix increment
        PathIterator operator++(int);

        friend bool operator== (const PathIterator& a, const PathIterator& b);

        friend bool operator!= (const PathIterator& a, const PathIterator& b);

};

#endif


