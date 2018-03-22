.. include:: <isopub.txt>

.. role:: cpp(code)
   :language: cpp

Understanding Vaious Complex Declarations and Expressions
=========================================================

.. todo:: proof read and correct.

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

Array Addressess in General
^^^^^^^^^^^^^^^^^^^^^^^^^^^

For a array named, say, ``some_array``, both ``some_array[0]`` and ``some_array`` are pointers of identical type and have identical initial values. This holds true regardless of the dimensions of ``some_array``\ |Mdash|\ be it one, two, three, or even higher dimensions. 
The code below, compiled using **g++ -std=c++1z**, illustrates this:

.. code-block:: cpp

    #include <cxxabi.h>
    #include <string>
    #include <utility>
    #include <iostream>
    #include <sstream>
 
    template<class T> string get_typeof(const string& str, const T& t)
    {
      const std::type_info  &ti = typeid(t);
      
      int status;
    
      char *realname = abi::__cxa_demangle(ti.name(), 0, 0, &status);// GCC extension __cxa_demanage() is from the header <cxxabi.h>
                                                                     // and is used to demangle the output of **typeid()**. 
      
      return string{"The type of '"} + str + "' is '" + realname + "'";
    }
    
    template<class T> std::string ptr_diff(const std::string& str, T&& ptr)
    {
      ostringstream ostr;
       
      ostr << get_typeof(str, forward<T>(ptr)) << ", and (" << str << " + 1) - " << str << " in bytes is: " << reinterpret_cast<unsigned long>(ptr + 1) - reinterpret_cast<unsigned long>(ptr);
      
      return ostr.str(); 	
    }
    
    int a1[3] = { 1, 2, 3};
    
    int b1[2][3] = {{ 1, 2, 3}, {4, 5, 6}};
    
    int c1[2][2][3] = {  {{ 1, 2, 3}, {4, 5, 6}}, {{ 7, 8, 9}, {10, 11, 12}} };

    cout << ptr_diff("a1", a1) << '\n';
    cout << ptr_diff("&a1[0]", &a1[0]) << "\n\n";

    cout << ptr_diff("b1", b1) << '\n';
    cout << ptr_diff("&b1[0]", &b1[0]) <<  "\n\n";

    cout << ptr_diff("c1", c1) << '\n';
    cout << ptr_diff("&c1[0]", &c1[0]) <<  "\n\n";
        
whose ouput is:

.. raw:: html

    <pre>
    The type of 'a1' is 'int [3]', and (a1 + 1) - a1 in bytes is: 4
    The type of '&a1[0]' is 'int*', and (&a1[0] + 1) - &a1[0] in bytes is: 4
    
    The type of 'b1' is 'int [2][3]', and (b1 + 1) - b1 in bytes is: 12
    The type of '&b1[0]' is 'int (*) [3]', and (&b1[0] + 1) - &b1[0] in bytes is: 12
    
    The type of 'c1' is 'int [2][2][3]', and (c1 + 1) - c1 in bytes is: 24
    The type of '&c1[0]' is 'int (*) [2][3]', and (&c1[0] + 1) - &c1[0] in bytes is: 24
    </pre>    

Note: While the types of ``c1`` and ``&c1[0]`` appear different above, they actually are not. When ``c1`` is used as a pointer in code, it is no different than using ``&c1[0]``.  

One Dimensional Arrays
^^^^^^^^^^^^^^^^^^^^^^

Given a one dimensional array such as ``int a[] = {1, 2, 3, 4, 5}``, the address of its first element ``&a[0]`` is of type **int \*** as the code below illustrates. ``a`` is equivalent to ``&a[0]``:

.. code-block:: cpp

    int a[] = {1, 2, 3, 4, 5};
    int *p1 = &a[0]; // p point to the first int in the block of elements comprising a  
    int *p2 = a;     // equivalent to line above.
    int *q = new int{9}; // q points to int on the heap with a value of 9

Adding one to a pointer does not increase the pointer's address by one but rather advances the address by ``sizeof(int)`` bytes, advancing it to the next integer, so adding one to the pointer ``int *p1`` above advances it to the next element in the array ``a[1]``.
In general, ``a[n]`` is equivalent to ``\*(a + b)``. ``p + n``, where ``n`` is an int, advances the pointer to the n + 1 :sup:`th` element (recall C/C++ arrays use zero-base indexing).

.. code-block:: cpp

    int a[] = {1, 2, 3, 4, 5};
    int *p = &a[0];      // p point to the first int in the block of elements comprising a  
    p = p + 4;
    cout << "p is equal to 5 is " << (*p == 5 ? "true" : "false"); // "p is equal to 5 is true"

The name of the array itself, here ``a``, is synomous with ``&a[0]``. Thus we can loop through the array with this for loop:

.. code-block:: cpp

    int a[] = {1, 2, 3, 4, 5};
    int *p = a;  
    for (int i = 0; i < sizeof(a)/sizeof(int); ++i) {
       
        cout << *(p + i) << ",";
    }
    // The above is equivalent to
    for (int i = 0; i < sizeof(a)/sizeof(int); ++i) {
       
        cout << a[i] << ",";
    }

Passing One Dimensional Arrays
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

One dimensional array can be passed using either syntax below.

.. code-block:: cpp

    int a[] = {1, 2, 3, 4, 5};
    int *p = &a[0]; // p point to the first int in the block of elements comprising a  

    void print_array1(int a[], int size) // passes the address of the array not the entire array.
    {
        for (int i = 0; i < size; ++i) {
           
            cout << a[i] << ",";
        }
    }
 
    void print_array2(int *p, int size)
    {
        for (int i = 0; i < size; ++i) {
           
            cout << p[i] << ",";
        }
    }
     
Higher Dimensional Arrays
^^^^^^^^^^^^^^^^^^^^^^^^^ 
 
Two dimensional and higher arrays are still stored, like one dimensional arrays, as one contiguous linear block, with the first row or block of values followed by the next row or block of values.
The code below shows the types of various addresses of 2-dimensional arrays, the difference in bytes when using pointer addtion, and it shows the corresponding dereference types. 
It uses the GCC extension ``abi::__cxa_demanage()`` from the header <cxxabi.h> to demangle the output of ``typeid()``.

.. code-block:: cpp

    #include <string>
    #include <iostream>
    #include <cxxabi.h>
    #include <sstream>
    #include <utility>
    using namespace std;

    template<class T> string get_typeof(const string& str, const T& t)
    {
      const std::type_info  &ti = typeid(t);
      
      int status;
    
      char *realname = abi::__cxa_demangle(ti.name(), 0, 0, &status); 
      
      return string{"The type of '"} + str + "' is '" + realname + "'";
    }
    
    template<class T> std::string ptr_diff(const std::string& str, T&& ptr)
    {
      ostringstream ostr;
       
      ostr << get_typeof(str, forward<T>(ptr)) << ", and (" << str << " + 1) - " << str << " in bytes is: " << reinterpret_cast<unsigned long>(ptr + 1) - reinterpret_cast<unsigned long>(ptr);
      
      return ostr.str(); 	
    }

    int a[5] = {1, 2, 3, 4, 5};
    
    cout << ptr_diff("&a[0]", &a[0]) << '\n';   
    
    cout << ptr_diff("&a", &a) << '\n';  
    
    cout << "\n\n";
       
    cout << "Below are the above types when dereferenced:\n";
 
    cout << "a is: int a[5]\n";
    
    cout << get_typeof("*a", *a) << "\n";
    
    cout << get_typeof("*&a[0]", *&a[0]) << "\n"; 
      
    cout << get_typeof("*&a", *&a) << "\n";

    cout <<  get_typeof("**&a", **&a) << "\n";
 
Produces this output:

.. raw:: html

    <pre>
    The type of '&a[0]' is 'int*', and (&a[0] + 1) - &a[0] in bytes is: 4
    The type of '&a' is 'int (*) [5]', and (&a + 1) - &a in bytes is: 20

    Below are the above types when dereferenced:
    a is: int a[5]
    The type of '*a' is 'int'
    The type of '*&a[0]' is 'int'
    The type of '*&a' is 'int [5]'
    The type of '**&a' is 'int'
    </pre>
 
The code below shows the types of various pointer types of 2-dimensional arrays and what their difference in bytes are, when using pointer addtion. It aslo shows the corresponding dereferenced types. 

This code:

.. code-block:: cpp
    
    int b[2][5] = {{1, 2, 3, 4, 5}, {6, 7, 8, 9, 10}}; // Same as: int a[][5] ={{1, 2, 3, 4, 5}, {6, 7, 8, 9, 10}}; 
    
    int *p1 = &b[0][0]; 
    
    int (*p2)[5] = &b[0]; 
    
    int (*p3)[5] = b;
    
    int (*p4)[2][5] = &b;
    
    cout << "b is: int b[2][5]\n";
        
    cout << get_typeof("&b[0][0]", &b[0][0]) << '\n'; 

    cout << get_typeof("&b[0]", &b[0]) << '\n'; 
    
    cout << get_typeof("b", b) << "\n";
    
    cout << get_typeof("&b", &b) << "\n";
    
    cout << "----------------------------------\nPointer addtion facts for array 'int b[2][5]'\n\n";
      
    cout << ptr_diff("&b[0][0]", &b[0][0]) << '\n';   
    
    cout << ptr_diff("b[0][0]", b[0][0]) << '\n';   // TODO: ???????????????????????????????????

    cout << ptr_diff("&b[0]", &b[0]) << '\n';   
    
    cout << ptr_diff("b", b) << '\n';  
    
    cout << ptr_diff("&b", &b) << '\n';  

    cout << "----------------------------------\n\n";
    
    cout << "b is: int b[2][5]. Dereferenced types:\n";
    
    cout << get_typeof("*&b[0]", *&b[0]) << '\n'; 
      
    cout << get_typeof("*b", *b) << "\n";
    
    cout << get_typeof("*&b", *&b) << "\n";
        
produces this output:

.. raw:: html

    <pre>
    b is: int b[2][5]
    The type of &b[0][0] is: int*
    The type of &b[0] is: int (*) [5]
    The type of b is: int [2][5]
    The type of &b is: int (*) [2][5]
    ----------------------------------
    
    When the pointer is 'int*', (pointer + 1) - pointer in bytes is: 4
    When the pointer is 'int (*) [5]', (pointer + 1) - pointer in bytes is: 20
    When the pointer is 'int [2][5]', (pointer + 1) - pointer in bytes is: 20
    When the pointer is 'int (*) [2][5]', (pointer + 1) - pointer in bytes is: 40
    ----------------------------------
    
    b is: int b[2][5]
    This: size of 'int' is: 4. 
    The type of *&b[0] is: int [5]
    The type of *b is: int [5]
    The type of *&b is: int [2][5]
    </pre>
 
Preliminary Summary of 2-dimensional array pointers
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
  
This shows that for a two dimensional array:

1. ``b[0][0]`` is an ``int *`` pointing to the first element of the array, and adding one to it advances the pointer **sizeof(int)** bytes (or four bytes) to the next int **b[0][1]**. 
2. Both ``&b[0]`` and ``a`` are of type ``int (*)[5]``, pointer to a block of five consecutive integers, and adding one to it advances the pointer ``4 x sizeof(int)`` or 20 bytes to the next block of five consecutive integers
3. ``&b`` is of type ``int (*)[2][5]``, a pointer to two blocks of 'a block of five integers', and adding one to it advances its address ``2 x (4 x sizeof(int))`` or 40 bytes to the next block of two blocks of 'a block of five integers'.

The same logic holds for higher dimensional arrays:

.. code-block:: cpp

    int b[2][3][2] = { {{1, 2}, {3, 4}, {5, 6}},   {{7, 8}, {9, 10}, {11, 12}}  }; 
     
    int *ptr1 = &b[0][0][0]; 
    
    int (*ptr2)[2] = &b[0][0];
    
    int (*ptr3)[3][2] = &b[0]; 
    
    int (*ptr4)[3][2] = b;
    
    int (*ptr5)[2][3][2] = &b;

    cout << "This size of 'int' is " << sizeof(int) << ".\n\n"; 

    cout << ptr_diff("&b[0][0][0]", &b[0][0][0]) << "\n";
    
    cout << ptr_diff("&b[0][0]", &b[0][0]) << "\n";

    cout << ptr_diff("&b[0]", &b[0]) << "\n";
    
    cout << ptr_diff("b", b) << "\n";
    
    cout << ptr_diff("&b", &b) << "\n"; 

whose output is: 

.. raw:: html

    <pre>
    This size of 'int' is 4.
    
    The type of '&b[0][0][0]' is 'int*', and (&b[0][0][0] + 1) - &b[0][0][0] in bytes is: 4
    The type of '&b[0][0]' is 'int (*) [2]', and (&b[0][0] + 1) - &b[0][0] in bytes is: 8
    The type of '&b[0]' is 'int (*) [3][2]', and (&b[0] + 1) - &b[0] in bytes is: 24
    The type of 'b' is 'int [2][3][2]', and (b + 1) - b in bytes is: 24
    The type of '&b' is 'int (*) [2][3][2]', and (&b + 1) - &b in bytes is: 48
    </pre>

which show that for a three dimensional array:

1. ``&b[0][0][0]`` is an ``int *``, pointing to ``b[0][0][0]``, and adding one to it advances the pointer ``sizeof(int)`` or four byes to the next int, whose address is  ``&b[0][0][1]``.
2. ``&b[0][0]`` is of ``int (*)[2]``, or pointer to a block of two consecutive integers, and adding one to such a pointer advances the pointer ``2 x sizeof(int)`` or 8 bytes to the next block of two integers at ``&b[0][1]``, which is the array ``{3, 4}``.
3. ``&b[0]`` is of type ``int (*)[3][2]``, a pointer to three blocks of a block of two integers each. So adding one to such a pointer advances its address **3 x (2 x sizeof(int))** or 24 bytes to the next block of three blocks of **a block of two integers**
   or ``&b[1]``, which is the array ``{{7, 8}, {9, 10}, {11, 12}}``.  
4. ``b`` is also synonomous to ``&b[0]`` and so is of type ``int (*)[3][2]``, a pointer to three blocks of a block of two integers each, and likewise adding one to such a pointer advances its address **3 x (2 x sizeof(int))** or 24 bytes to the next block of three
   blocks of a block of two integers each or ``&b[1]``, which is the array ``{{7, 8}, {9, 10}, {11, 12}}``.  
5. ``&b`` is of type ``int (*)[2][3][2]``, a pointer to two blocks of three blocks of a block two integers each. So adding one to such a pointer advances its address **2 x (3 x (2 x sizeof(int)))** or 48 bytes to the next block of two blocks of three blocks of
   a block of two integers each, whose physical address is ``sizeof(int) + &b[1][2][1]``, four bytes beyond the last entry in ``b``.

Summary of Pointers and Arrays
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Given

.. code-block:: cpp

    int myMatrix[2][4] = { {1,2,3,4},{5,6,7,8} };

``myMatrix[0]`` is a pointer to the first row of the 2D array. ``(MyMatrix + 0)`` is of type ``int (*)[4]``. It is a pointer to the entire first inner array of four integers. It is equivalent to ``(&myMatirx[0] + 0)``. To actually access elements of
 the first inner array it must be deferenced: ``*(myMatrix + 0)``, which yields an ``int *`` to the first element of the array. Adding two to it, ``(*(myMatrix +0)) + 2``, moves the pointer to the third value in the first inner array. Since dereferencing
 is always applied before any pointer arithmetic, ``(*(myMatrix +0)) + 2`` can be simplified as ``*(myMatrix +0) + 2``, and dereferencing it ``*(*(myMatrix +0) + 2)`` returns the int ``3``.

``myMatrix[1]`` is likewise a pointer to the second row of the 2D array, and ``*myMatrix[1]`` is  of type ``int (*)[4]``. It is a pointer to the entire second inner array of four integers. It is equivalent to ``&myMatirx[0] + 1``.

``*((*(myMatrix + 1)) + 2)``       

``myMatrix`` is the same as ``&myMatrix[0]``, which are of type ``int (*4)``. This is the the address of the first row of ``myMatrix``. Adding one ``(myMatrix + 1)`` advances the pointer to the second row of ``myMatrix``. Deferencing 
``*(myMatrix + 1)`` returns an ``int *`` to the first element of the one-dimensional array ``{5, 6, 7, 8}``. ``*(myMatrix + 1)`` is equivalent to ``int *p = &myMatrix[1][0]``. Then adding 2, ``(*(myMatrix + 1)) + 2``, advances the ``int *`` to the third element
of the array ``{5, 6, 7, 8}`` , and then deferencing it ``*((*(myMatrix + 1)) + 2)`` returns the integer at that position ``7``.

``*(&myMatrix[0][0] + 4 * 1 + 2)`` ... add explanation. 

``*(myMatrix[1] + 2)`` ... add explanation. 

``(*(myMatrix + 1))[2]`` ... add explanation. 

In general, index operators are equivlant to pointer arithmetic and dereferencing following this pattern, here using a 3-dimensional array as the example: the expxression **a[i][j][k]** is equivalent to **\*(*(*(a + i) + j) + k)**. For example:

.. code-block:: cpp

    int a[2][2][3] = {  {{ 1, 2, 3}, {4, 5, 6}}, {{ 7, 8, 9}, {10, 11, 12}} };
    cout << "a[0][1][2] = " <<  a[0][1][2] << ", and *( (*(*(a + 0) + 1)) + 2) = " <<   *((*(*(a + 0) + 1)) + 2);

.. raw:: html 

    <pre>
    a[0][1][2] = 6, and *( (*(*(a + 0) + 1)) + 2) = 6
    </pre>

``a`` is of type ``int (*)[2][3]``. ``(a + 0)`` therefore points to the first of the two inner 2  x 3 arrays. Dereferencing it yields a pointer of type ``int (*) [3]`` that points to the first array (of the first of the two inner 2 x 3 array of ``a``), namely {1, 2, 3}. 
``(*(a + 0) + 1)`` adds one to this pointer, and moves the pointer to the second array (of the first of two inner arrays of ``a``). Dereferencing it ``(*(*(a + 0) + 1))`` yields an ``int *`` pointer that points to ``{4, 5, 6}``, and adding two
``((*(*(a + 0) + 1))   + 2)`` moves it to ``6``. Finally, deferencing the pointer yields the pointer-to value of ``6``.

In passing a multi‐dimensional array, the first array size does not have to be specified. The second (and any subsequent) dimensions must be given:  **int myFun(int list[][10]);**

.. todo::

     Synthesize the material in these articles with code examples that illustrate the main points. Copy and past the appropriate code, adding my own commnet, and including the output.

* `Multidimensional Pointer Arithmetic in C/C+ <https://www.geeksforgeeks.org/multidimensional-pointer-arithmetic-in-cc/>`_.
* `Pointer to an Array | Array Pointer <https://www.geeksforgeeks.org/pointer-array-array-pointer/>`_.

Deciphering Complex C/C++ Declarations
--------------------------------------

`How to interpret complex C/C++ declarations <https://www.codeproject.com/Articles/7042/How-to-interpret-complex-C-C-declarations>`_

https://news.ycombinator.com/item?id=12775735

`Reading Types in C Using the Right Left Walk Method <http://www.cs.uml.edu/%7Ecanning/101/RLWM.pdf>`_.
