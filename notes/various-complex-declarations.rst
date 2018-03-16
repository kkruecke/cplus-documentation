.. include:: <isopub.txt>

Understanding Vaious Complex Declarations and Expressions
=========================================================

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

A const point to const data is declared:

.. code-block:: cpp

    int x = 10;
    int y = 11; 
    const int * const cpintc = &x;
    *cpintc = 20; // Error: *cpintc is read only
    cpintc = &y;   // Error: cpintc is read only 

Pointers, Arrays and Multidimensional Arrays
--------------------------------------------

One Dimensional Arrays
^^^^^^^^^^^^^^^^^^^^^^

Given a one dimensional array such as ``int a[]  {1, 2, 3, 4, 5}``, the address of its first element ``&a[0]`` is of type ``int *``, thus:

.. code-block:: cpp

    int a[] = {1, 2, 3, 4, 5};
    int *p = &a[0];      // p point to the first int in the block of elements comprising a  
    int *q = new int{9}; // q points to int on the heap with a value of 9

We can advance to the next element in the array by adding one to ``p``, so ``p++`` or ``p = p + 1`` would now point to ``a[1]``, and ``*p`` is ``2``; or we can can add 4 to the initial value of ``p``:

.. code-block:: cpp

    int a[] = {1, 2, 3, 4, 5};
    int *p = &a[0];      // p point to the first int in the block of elements comprising a  
    p = p + 4;
    cout << "p is equal to 5 is " << (*p == 5 ? "true" : "false"); // "p is equal to 5 is true"

The name of the array itself, here ``a``, is synomous with ``&a[0]``. Thus we can loop through the array with this for loop:

.. code-block:: cpp

    int a[] = {1, 2, 3, 4, 5};
    int num_elements = sizeof(a)/sizeof(int);
    int *p = a;  
    for (int i = 0; i < num_elements; ++i) {
       
        cout << *(p + i) << ",";
    }

Higher Dimensional Arrays
^^^^^^^^^^^^^^^^^^^^^^^^^ 

This code illustrates pointer usage and meaning for higher dimesional arrays.

.. code-block:: cpp

    template<class T> void print_ptr(const T& pointer, const std::string& msg1,  const std::string& msg2)
    {
        cout << "pointer is " << msg1 << ". pointer type is '" << msg2 << "'.\n";
        
        cout << "pointer = " << pointer << ".  pointer + 1 = " << pointer + 1 << "\n";
       
        cout << "The difference in bytes between pointer and (pointer + 1) = " <<reinterpret_cast<unsigned long>(pointer + 1) - reinterpret_cast<unsigned long>(pointer) << "\n\n";;
    }
    
    int a[2][5] = {{1, 2, 3, 4, 5}, {6, 7, 8, 9, 10}}; // Same as: int a[][5] ={{1, 2, 3, 4, 5}, {6, 7, 8, 9, 10}}; 
    
    int *p1 = &a[0][0]; 
    int (*p2)[5] = &a[0]; 
    int (*p3)[5] = a;
    
    int (*p4)[2][5] = &a;

    cout << "This size of 'int' is " << sizeof(int) << ".\n\n"; 

    print_ptr(p1, "&a[0][0]", "int *");
    
    print_ptr(p2, "&a[0]", "int (*)[5]");
    
    print_ptr(p3, "a", "int (*)[5]");
    
    print_ptr(p4, "&a", "int (*)[2][5]");

produces the the output below:

.. raw:: html
  
    <pre>
    This size of 'int' is 4. 
    
    pointer is "&a[0][0]", and type is "int *"
    pointer = 0x7ffd9c507b60.  pointer + 1 = 0x7ffd9c507b64
    The difference in bytes between pointer and (pointer + 1) = 4
    
    pointer is '&a[0]', and type is 'int (*)[5]'.
    pointer = 0x7ffd9c507b60.  pointer + 1 = 0x7ffd9c507b74
    The difference in bytes between pointer and (pointer + 1) = 20
    
    pointer is 'a', and type is 'int (*)[5]'.
    pointer = 0x7ffd9c507b60.  pointer + 1 = 0x7ffd9c507b74
    The difference in bytes between pointer and (pointer + 1) = 20
    
    pointer is '&a', and type is 'int (*)[2][5]'.
    pointer = 0x7ffd9c507b60.  pointer + 1 = 0x7ffd9c507b88
    The difference in bytes between pointer and (pointer + 1) = 40
    </pre>

which show that for a two dimensional array:

1. ``&a[0][0]`` is an ``int *``, and adding one to it advances the pointer ``sizeof(int)`` or four byes to the next int, which is ``a[0][1]``.
2. Both ``&a[0]`` and ``a`` are of type ``int (*)[5]``, or pointer to a block of five consecutive integers, and adding one to such a pointer advances the pointer ``4 x sizeof(int)`` or 20 bytes to the next block of five integers
3. ``&a`` is of type ``int (*)[2][5]``, a pointer to two blocks of a block of five integers each. So adding one to such a pointer advances its address ``2 x (4 x sizeof(int))`` or 40 bytes to the next block of two block of a block of five integers each.

The same logic holds for higher dimensional arrays:

.. code-block:: cpp

    int b[3][2][5] = {{1, 2, 3, 4, 5}, {6, 7, 8, 9, 10}}; // Same as: int a[][5] ={{1, 2, 3, 4, 5}, {6, 7, 8, 9, 10}}; 
    
    int *ptr1 = &b[0][0][0]; 
    int (*ptr2)[5] = &b[0][0];
    int (*ptr3)[2][5] = &b[0]; 
    int (*ptr4)[2][5] = b;
    
    int (*ptr5)[3][2][5] = &b;

    cout << "This size of 'int' is " << sizeof(int) << ".\n\n"; 

    print_ptr(ptr1, "&b[0][0][0]", "int *");
    
    print_ptr(ptr2, "&b[0][0]", "int (*)[5]");

    print_ptr(ptr3, "&b[0]", "int (*)[2][5]");
    
    print_ptr(ptr4, "b", "int (*)[2][5]");
    
    print_ptr(ptr5, "&b", "int (*)[3][2][5]");

whose output is: 

.. raw:: html

    <pre>
    This size of 'int' is 4.
    
    pointer is '&b[0][0][0]', and type is 'int *'.
    pointer = 0x7fffffffe380.  pointer + 1 = 0x7fffffffe384
    The difference in bytes between pointer and (pointer + 1) = 4
    
    pointer is '&b[0][0]', and type is 'int (*)[5]'.
    pointer = 0x7fffffffe380.  pointer + 1 = 0x7fffffffe394
    The difference in bytes between pointer and (pointer + 1) = 20
    
    pointer is '&b[0]', and type is 'int (*)[2][5]'.
    pointer = 0x7fffffffe380.  pointer + 1 = 0x7fffffffe3a8
    The difference in bytes between pointer and (pointer + 1) = 40
    
    pointer is 'b', and type is 'int (*)[2][5]'.
    pointer = 0x7fffffffe380.  pointer + 1 = 0x7fffffffe3a8
    The difference in bytes between pointer and (pointer + 1) = 40
    
    pointer is '&b', and type is 'int (*)[3][2][5]'.
    pointer = 0x7fffffffe380.  pointer + 1 = 0x7fffffffe3f8
    The difference in bytes between pointer and (pointer + 1) = 120
    </pre>

which show that for a three dimensional array:

1. ``&b[0][0][0]`` is an ``int *``, and adding one to it advances the pointer ``sizeof(int)`` or four byes to the next int, which is ``b[0][0][1]``.
2. ``&b[0][0]`` is of ``int (*)[5]``, or pointer to a block of five consecutive integers, and adding one to such a pointer advances the pointer ``4 x sizeof(int)`` or 20 bytes to the next block of five integers
3. ``&b[0]`` is of type ``int (*)[2][5]``, a pointer to two blocks of a block of five integers each. So adding one to such a pointer advances its address ``2 x (4 x sizeof(int))`` or 40 bytes to the next block of two block of a block of five integers each.
4. ``b`` is synonomous to ``&b[0]`` and so is of type ``int (*)[2][5]``, a pointer to two blocks of a block of five integers each. So adding one to such a pointer advances its address ``2 x (4 x sizeof(int))`` or 40 bytes to the next block of two blocks of a block of five integers each.
5. ``&b`` is of type ``int (*)[3][2][5]``, a pointer to three blocks of two block of a block five integers each. So adding one to such a pointer advances its address ``3 x (2 x (4 x sizeof(int)))`` or 120 bytes to the next block of three blocks of two blocks of a block of five integers each.

Summary of Pointers and Arrays
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. todo::

     Synthesize the material in these articles with code examples that illustrate the main points. Copy and past the appropriate code, adding my own commnet, and including the output.

* `Pointer to an Array | Array Pointer <https://www.geeksforgeeks.org/pointer-array-array-pointer/>`_.
* `Multidimensional Pointer Arithmetic in C/C+ <https://www.geeksforgeeks.org/multidimensional-pointer-arithmetic-in-cc/>`_.
* `Pointer, Arrays and Multidimensional Arrays <https://www.cse.msu.edu/~cse251/lecture11.pdf>`_.

Deciphering Complex C/C++ Declarations
--------------------------------------

`How to interpret complex C/C++ declarations <https://www.codeproject.com/Articles/7042/How-to-interpret-complex-C-C-declarations>`_

https://news.ycombinator.com/item?id=12775735

`Reading Types in C Using the Right Left Walk Method <http://www.cs.uml.edu/%7Ecanning/101/RLWM.pdf>`_.
