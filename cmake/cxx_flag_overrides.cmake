if (MSVC)
    # Compiler switches for C++, needed for static runtime linking
    set(CMAKE_CXX_FLAGS_DEBUG_INIT "/MTd /D_DEBUG /Zi /Ob0 /Od /RTC1")
    set(CMAKE_CXX_FLAGS_RELEASE_INIT "/MT /O2 /Ob2 /D NDEBUG")
    set(CMAKE_CXX_FLAGS_MINSIZEREL_INIT "/MT /O1 /Ob1 /D NDEBUG")
    set(CMAKE_CXX_FLAGS_RELWITHDEBINFO_INIT "/MT /Zi /O2 /Ob1 /D NDEBUG")
endif (MSVC)
