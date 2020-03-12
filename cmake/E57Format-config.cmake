include(CMakeFindDependencyMacro)

find_dependency(XercesC REQUIRED)
include(${CMAKE_CURRENT_LIST_DIR}/E57Format-export.cmake)
