#!/usr/bin/env python3
"""
Section 28.1 : Appeler du C depuis Python
Description : Script de test de l'extension Python C API (module_exemple)
Fichier source : 01-c-depuis-python.md
"""

import sys
import os

# Ajouter le repertoire courant au path pour trouver le module
script_dir = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, script_dir)

print("=== Test extension Python C API ===\n")

import module_exemple

# Test addition
print("--- Test addition ---")
resultat = module_exemple.addition(10, 20)
print(f"  10 + 20 = {resultat}")
assert resultat == 30

resultat2 = module_exemple.addition(-5, 5)
print(f"  -5 + 5 = {resultat2}")
assert resultat2 == 0

# Test division
print("\n--- Test division ---")
resultat3 = module_exemple.diviser(10, 3)
print(f"  10 / 3 = {resultat3}")
assert resultat3 == 3

# Test division par zero
print("\n--- Test division par zero ---")
try:
    module_exemple.diviser(10, 0)
    print("  ERREUR : pas d'exception levee !")
except ZeroDivisionError as e:
    print(f"  Exception ZeroDivisionError : {e}")

print("\n=== Tous les tests extension passes ! ===")
