.. include:: <isopub.txt>

.. role:: cpp(code)
   :language: cpp

Understanding Arrays, Pointers and Multi-Dimensional Arrays
===========================================================

.. todo:: proof read and correct.

Pointers, Arrays and Multidimensional Arrays
--------------------------------------------

General Comments on Array Addressess
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Given the array ``some_array``, then ``&some_array[0]`` and ``some_array`` are both pointers of identical type and value, that point to the first element in the array. This holds true regardless of the number of dimensions of ``some_array``. The code below,
compiled using **g++ -std=c++2a**, illustrates this.

.. code-block:: cpp

    #include <cxxabi.h>
    #include <string>
    #include <utility>
    #include <iostream>
    #include <sstream>
 
    template<class T> string get_typeof(const string& str, const T& t)
    {
      const std::type_info &ti = typeid(t);
      
      int status;

      /*
         GCC extension __cxa_demanage()--from <cxxabi.h>--demangles the output of std::typeid().
       */

      char *realname = abi::__cxa_demangle(ti.name(), 0, 0, &status);
      
      return string{"The type of '"} + str + "' is '" + realname + "'";
    }

    // Using T&& allows us to pass both rvalues and lvalues to ptr_diff(), where they are
    // forwarded to get_typeof()
    template<class T> std::string ptr_diff(const std::string& str, T&& ptr) 
    {
      ostringstream ostr;
       
      ostr << get_typeof(str, std::forward<T>(ptr)) << ", and (" << str << " + 1) - " << str << " in bytes is: " \
           << reinterpret_cast<unsigned long>(ptr + 1) - reinterpret_cast<unsigned long>(ptr);
      
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
        
and the ouput is:

.. raw:: html

    <pre>
    The type of 'a1' is 'int [3]', and (a1 + 1) - a1 in bytes is: 4
    The type of '&a1[0]' is 'int*', and (&a1[0] + 1) - &a1[0] in bytes is: 4
    
    The type of 'b1' is 'int [2][3]', and (b1 + 1) - b1 in bytes is: 12
    The type of '&b1[0]' is 'int (*) [3]', and (&b1[0] + 1) - &b1[0] in bytes is: 12
    
    The type of 'c1' is 'int [2][2][3]', and (c1 + 1) - c1 in bytes is: 24
    The type of '&c1[0]' is 'int (*) [2][3]', and (&c1[0] + 1) - &c1[0] in bytes is: 24
    </pre>    

.. note: While the types of, for example, ``c1`` and ``&c1[0]`` appear different above, actually they are not. When ``c1`` is used as a pointer in code without any index operators present, it is no different than using ``&c1[0]``. It is of the same type as
   ``&c1[0]`` and has the same pointer value.  

One Dimensional Arrays
^^^^^^^^^^^^^^^^^^^^^^

Given the one dimensional array ``int a[] = {1, 2, 3, 4, 5}``, the address of its first element ``&a[0]`` is of type **int \***, as the code below illustrates, and ``a`` is exactly equivalent to ``&a[0]``:

.. code-block:: cpp

    int a[] = {1, 2, 3, 4, 5};
    int *p1 = &a[0]; // p point to the first int in the block of elements comprising a  
    int *p2 = a;     // equivalent to line above.
    int *q = new int{9}; // q points to int on the heap with a value of 9

Adding one to a pointer does not increase the pointer's address by one but rather advances the address by ``sizeof(int)`` bytes, advancing it to the next integer, so addition is scaled based on the underlying pointed-to type.. In the case of ``*p1 + ```, the pointer 
is advanced to the next element in the array ``a[1]``, to the address ``&a[1]``.

In fact, ``a[n]`` is equivalent to ``*(a + b)``. ``a + n``, where ``n`` is an int, advances the pointer to the n + 1 :sup:`th` element (recall C/C++ arrays use zero-base indexing).

.. code-block:: cpp

    int a[] = {1, 2, 3, 4, 5};
    int *p = &a[0];      // p point to the first int in the block of elements comprising a  
    p = p + 4;
    cout << "p is equal to 5 is " << (*p == 5 ? "true" : "false"); // "p is equal to 5 is true"

Again, the name of the array itself, here ``a``, is synonymous with ``&a[0]``. Thus we can loop through the array with following for-loop:

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

    // exactly equivalent to the function above
    void print_array2(int *p, int size) 
    {
        for (int i = 0; i < size; ++i) {
           
            cout << p[i] << ",";
        }
    }
     
Higher Dimensional Arrays
^^^^^^^^^^^^^^^^^^^^^^^^^ 
 
Two dimensional and higher arrays are still stored, like one dimensional arrays, as one contiguous linear block, with the first row or block of values followed by the next row or block of values. The code below shows the various address types possible for a
2-dimensional arrays, and the difference in bytes when using pointer addition for each of these various pointer types. It also shows the corresponding dereference types. It uses the GCC extension ``abi::__cxa_demanage()`` from the header <cxxabi.h> to demangle
the output of ``typeid()``.

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
 
and the output is:

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
 
The code below shows the types of various pointer types of 2-dimensional arrays and what their difference in bytes are, when using pointer addtion. It aslo shows the corresponding dereferenced types: 

.. code-block:: cpp

    // Below same as: int b[][5] ={{1, 2, 3, 4, 5}, {6, 7, 8, 9, 10}}; 
    int b[2][5] = {{1, 2, 3, 4, 5}, {6, 7, 8, 9, 10}};
    
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
    
    cout << ptr_diff("b[0][0]", b[0][0]) << '\n';   // TODO: ??

    cout << ptr_diff("&b[0]", &b[0]) << '\n';   
    
    cout << ptr_diff("b", b) << '\n';  
    
    cout << ptr_diff("&b", &b) << '\n';  

    cout << "----------------------------------\n\n";
    
    cout << "b is: int b[2][5]. Dereferenced types:\n";
    
    cout << get_typeof("*&b[0]", *&b[0]) << '\n'; 
      
    cout << get_typeof("*b", *b) << "\n";
    
    cout << get_typeof("*&b", *&b) << "\n";
        
The output is:

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
2. Both ``&b[0]`` and ``b`` are of type ``int (*)[5]``, pointer to a block of five consecutive integers, and adding one to them advances the pointer ``4 x sizeof(int)`` or 20 bytes to the next block of five consecutive integers
3. ``&b`` is of type ``int (*)[2][5]``, a pointer to two blocks of 'a block of five integers', and adding one to it advances its address ``2 x (4 x sizeof(int))`` or 40 bytes to the next block of two blocks of 'a block of five integers(which in this
   case does not exist).

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

and the output is:

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

..   Note:: ``b[0][0]`` is not of the same type as ``&b[0][0]``, although ``&b[0][0][0]`` and ``b`` are, when ``b`` is used as a pointer and without any accompanying array index operators.

3. ``&b[0]`` is of type ``int (*)[3][2]``, a pointer to three blocks of a block of two integers each. So adding one to such a pointer advances its address **3 x (2 x sizeof(int))** or 24 bytes to the next block of three blocks of **a block of two integers**
   or ``&b[1]``, which is the array ``{{7, 8}, {9, 10}, {11, 12}}``.  
4. ``b`` is also synonomous to ``&b[0]`` and so is of type ``int (*)[3][2]``, a pointer to three blocks of a block of two integers each, and likewise adding one to such a pointer advances its address **3 x (2 x sizeof(int))** or 24 bytes to the next block of three
   blocks of a block of two integers each or ``&b[1]``, which is the array ``{{7, 8}, {9, 10}, {11, 12}}``.  
5. ``&b`` is of type ``int (*)[2][3][2]``, a pointer to two blocks of three blocks of a block two integers each. So adding one to such a pointer advances its address **2 x (3 x (2 x sizeof(int)))** or 48 bytes to the next block of two blocks of three blocks of
   a block of two integers each, whose physical address is ``sizeof(int) + &b[1][2][1]``, four bytes beyond the last entry in ``b``.

Summary Example of Various Pointer Types for Arrays
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Given

.. code-block:: cpp

    int myMatrix[2][4] = { {10,20,30,40},{50,60,70,80} };

then ``myMatrix[0]`` is a pointer to the first row of the 2D array. ``(MyMatrix + 0)`` is of type ``int (*)[4]``. It is a pointer to the entire first inner array of four integers. It is equivalent to ``(&myMatirx[0] + 0)``. To actually access elements of
 the first inner array it must be deferenced: ``*(myMatrix + 0)``, which yields an ``int *`` to the first element of the array. Adding two to it, ``(*(myMatrix +0)) + 2``, moves the pointer to the third value in the first inner array. 

.. todo:: Read these links below and incorporate any relevant points:

* `Order of operations for dereference and bracket-ref in C <https://stackoverflow.com/questions/3552844/order-of-operations-for-dereference-and-bracket-ref-in-c>`_ 
* `C Pointers <http://www.cs.yale.edu/homes/aspnes/pinewiki/C(2f)Pointers.html>`_ 
* `Everything you Need to Know about Pointers in C <https://edoras.sdsu.edu/doc/c/pointers-1.2.2/>`_

.. todo:: The statement below is wrong--I believe--in the sense that it might not be a pointer.

``*((*(myMatrix + 1)) + 2)``       

``myMatrix`` is the same as ``&myMatrix[0]``, which are of type ``int (*4)``. This is the the address of the first row of ``myMatrix``. Adding one ``(myMatrix + 1)`` advances the pointer to the second row of ``myMatrix``. Deferencing 
``*(myMatrix + 1)`` returns an ``int *`` to the first element of the one-dimensional array ``{5, 6, 7, 8}``. ``*(myMatrix + 1)`` is equivalent to ``int *p = &myMatrix[1][0]``. Then adding 2, ``(*(myMatrix + 1)) + 2``, advances the ``int *`` to the third element
of the array ``{5, 6, 7, 8}`` , and then deferencing it ``*((*(myMatrix + 1)) + 2)`` returns the integer at that position ``7`` 

``*(&myMatrix[0][0] + 4 * 1 + 2)``\ |mdash|\ ``&myMatrix[0][0]`` is of type ``int *``.... finish explanation.

``*(myMatrix[1] + 2)`` ... add explanation. 

``(*(myMatrix + 1))[2]`` ... add explanation. 

In general, index operators are equivlant to pointer arithmetic and dereferencing following the pattern used here with a 3-dimensional array but applicable to arrays of any higher dimensions. Given an array ``a`` of three dimensions, say, ``a[n][m][n]``,
the expxression **a[i][j][k]** is always equivalent to **\*(*(*(a + i)) + j) + k)**. For example:

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
