#ifndef E57VERSION_H
#define E57VERSION_H

#include <cstdint>

/// Version numbers of ASTM standard that this library supports
const uint32_t E57_FORMAT_MAJOR = 1;
const uint32_t E57_FORMAT_MINOR = 0;

/// REVISION_ID should be passed from compiler command line

#ifndef REVISION_ID
#error "Need to specify REVISION_ID on command line"
#endif

const char E57_LIBRARY_ID[] = REVISION_ID;

#endif
