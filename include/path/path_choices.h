#ifndef PATHS_H 
#define PATHS_H 1

#include <memory>
#include "path.h"


struct PathItem {
    const char *name;
    const Path &path;
};

extern PathItem path_choices[];

std::unique_ptr<Path> get_path(const char *name);
 
#endif
