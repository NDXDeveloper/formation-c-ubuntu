cmake_minimum_required(VERSION 3.10)
project(MonProjet VERSION 1.0.0 LANGUAGES C)

# Standard C17
set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Option pour les tests
option(BUILD_TESTS "Compiler les tests" ON)

# Sous-répertoires
add_subdirectory(lib)
add_subdirectory(src)

if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

# Installation
install(FILES include/mon_projet.h
    DESTINATION include
)
