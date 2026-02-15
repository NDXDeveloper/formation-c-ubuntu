#!/usr/bin/env python3
"""
Section 28.1 : Appeler du C depuis Python
Description : Script de test des bibliotheques C via ctypes
              Teste addition, somme_tableau et strings
Fichier source : 01-c-depuis-python.md
"""

import ctypes
import os

# Repertoire courant
script_dir = os.path.dirname(os.path.abspath(__file__))

print("=== Test ctypes : C depuis Python ===\n")

# --- Test 1 : Addition ---
print("--- Test 1 : Addition ---")
lib_add = ctypes.CDLL(os.path.join(script_dir, "libaddition.so"))
lib_add.addition.argtypes = [ctypes.c_int, ctypes.c_int]
lib_add.addition.restype = ctypes.c_int

resultat = lib_add.addition(5, 7)
print(f"  5 + 7 = {resultat}")
assert resultat == 12, f"Erreur: attendu 12, obtenu {resultat}"

resultat2 = lib_add.addition(-3, 10)
print(f"  -3 + 10 = {resultat2}")
assert resultat2 == 7

# --- Test 2 : Somme de tableau ---
print("\n--- Test 2 : Somme de tableau ---")
lib_somme = ctypes.CDLL(os.path.join(script_dir, "libsomme.so"))
lib_somme.somme_tableau.argtypes = [ctypes.POINTER(ctypes.c_int), ctypes.c_size_t]
lib_somme.somme_tableau.restype = ctypes.c_int

nombres = [10, 20, 30, 40, 50]
tableau_c = (ctypes.c_int * len(nombres))(*nombres)
resultat = lib_somme.somme_tableau(tableau_c, len(nombres))
print(f"  Somme de {nombres} = {resultat}")
assert resultat == 150

# --- Test 3 : Strings ---
print("\n--- Test 3 : Chaines de caracteres ---")
lib_str = ctypes.CDLL(os.path.join(script_dir, "libstrings.so"))

lib_str.longueur_chaine.argtypes = [ctypes.c_char_p]
lib_str.longueur_chaine.restype = ctypes.c_int
lib_str.mettre_en_majuscules.argtypes = [ctypes.c_char_p]
lib_str.mettre_en_majuscules.restype = None

texte = b"Bonjour Python"
longueur = lib_str.longueur_chaine(texte)
print(f"  Longueur de '{texte.decode()}' = {longueur}")
assert longueur == 14

texte_mutable = ctypes.create_string_buffer(b"hello world")
lib_str.mettre_en_majuscules(texte_mutable)
print(f"  Majuscules : {texte_mutable.value.decode()}")
assert texte_mutable.value == b"HELLO WORLD"

print("\n=== Tous les tests ctypes passes ! ===")
