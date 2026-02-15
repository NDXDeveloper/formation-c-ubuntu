/* ============================================================================
   Section 28.1 : Appeler du C depuis Python
   Description : Fonction d'addition compilee en bibliotheque partagee (.so)
                 pour utilisation via ctypes depuis Python
   Fichier source : 01-c-depuis-python.md
   ============================================================================ */

/* Fonction simple qui additionne deux entiers */
int addition(int a, int b) {
    return a + b;
}
