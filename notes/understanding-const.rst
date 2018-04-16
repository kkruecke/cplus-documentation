.. include:: <isopub.txt>

.. role:: cpp(code)
   :language: cpp

Understanding const and const Correctness
=========================================

const and const Pointers
------------------------

``const`` indciates a variable is read-only. ``const`` can appear before or after the actual type:

.. code-block:: cpp

     const int m = 10;
     int const n = 11;
      m = 3; // error
      n = 2; // error

A pointer to constant data is declared one of two ways:

.. code-block:: cpp

    const int *pint1;
    int const *pint2;
    int n = 11;

    pint1 = &n;
    *pint1 = 4; // Error: *pint1 is const, ie read-only

    pint2 = &n;
    *pint2 = 4; // Error: *pint2 is const, ie read-only

A const pointer whose assigned address value cannot be reassigned is declared and defined as below:

.. code-block:: cpp

     int x = 10;    
     int z = 11;
     int * const cpint = &x;
     cpint = &z; // Error, pint is read-only
     *cpint = 20; // o.k., *pint is not const. It is read/write  

A const pointer to const data is declared:

.. code-block:: cpp

    int x = 10;
    int y = 11; 
    const int * const cpintc = &x;
    *cpintc = 20; // Error: *cpintc is read only
    cpintc = &y;   // Error: cpintc is read only, too 
