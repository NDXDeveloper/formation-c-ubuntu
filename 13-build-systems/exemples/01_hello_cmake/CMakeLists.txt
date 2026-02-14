# Version minimale de CMake requise
cmake_minimum_required(VERSION 3.15)

# Nom du projet et langage utilisé
project(HelloCMake C)

# Créer un exécutable nommé "hello" à partir de main.c
add_executable(hello main.c)
