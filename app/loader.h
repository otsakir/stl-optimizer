#ifndef UTILS_LOADER_H
#define UTILS_LOADER_H

#include "mesh.h"

namespace Utils {

class Loader
{
public:
    Loader();

    /**
     * @brief Loads an stl file into a Core::Mesh object
     *
     * If more than one solids are found in the stl file, only the
     * first one will be loaded.
     *
     * @param filename path and filename of the .stl file to load
     * @param mesh allocated Core::Mesh object to hold new mesh data
     */
    void loadStl(QString filename, Core::Mesh& mesh); // add options parameter
};

} // namespace Utils

#endif // UTILS_LOADER_H
