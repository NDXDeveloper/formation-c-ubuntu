# Section  : 03 - Make vs CMake
# Description : CMakeLists.txt pour la calculatrice (approche declarative)
# Fichier source : 05_make_vs_cmake/CMakeLists.txt

cmake_minimum_required(VERSION 3.15)
project(Calculator C)

# Standard C
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Warnings
add_compile_options(-Wall -Wextra)

# Créer l'exécutable
add_executable(calculator main.c operations.c)
