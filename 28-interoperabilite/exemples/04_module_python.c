/* ============================================================================
   Section 28.1 : Appeler du C depuis Python
   Description : Extension C pour Python (Python C API) avec addition et
                 division (gestion d'erreur ZeroDivisionError)
   Fichier source : 01-c-depuis-python.md
   ============================================================================ */

#define PY_SSIZE_T_CLEAN
#include <Python.h>

/* Fonction C qui sera exposee a Python */
static PyObject *addition_py(PyObject *self, PyObject *args) {
    (void)self;
    int a, b;

    if (!PyArg_ParseTuple(args, "ii", &a, &b)) {
        return NULL;
    }

    int resultat = a + b;
    return PyLong_FromLong(resultat);
}

/* Division avec gestion d'erreur */
static PyObject *diviser_py(PyObject *self, PyObject *args) {
    (void)self;
    int a, b;

    if (!PyArg_ParseTuple(args, "ii", &a, &b)) {
        return NULL;
    }

    if (b == 0) {
        PyErr_SetString(PyExc_ZeroDivisionError, "Division par zero !");
        return NULL;
    }

    return PyLong_FromLong(a / b);
}

/* Table des methodes du module */
static PyMethodDef ModuleMethods[] = {
    {"addition", addition_py, METH_VARARGS, "Additionne deux entiers"},
    {"diviser", diviser_py, METH_VARARGS, "Divise deux entiers"},
    {NULL, NULL, 0, NULL}
};

/* Definition du module */
static struct PyModuleDef moduleexemple = {
    PyModuleDef_HEAD_INIT,
    "module_exemple",
    "Module d'exemple C pour Python",
    -1,
    ModuleMethods,
    NULL, NULL, NULL, NULL
};

/* Fonction d'initialisation */
PyMODINIT_FUNC PyInit_module_exemple(void) {
    return PyModule_Create(&moduleexemple);
}
