.. include:: <isopub.txt>

.. role:: cpp(code)
   :language: cpp

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

Array Addressess in General
^^^^^^^^^^^^^^^^^^^^^^^^^^^

For a array named, say, ``some_array``, both **&some_array[0]** and **some_array** are pointers of identical type and identical initial values. This is true regardless of the dimensions of ``som_array``\ {ndash}\ one, two, three, or higher dimensions. 
The code below illustartes this:

.. code-block:: cpp

    template<class T> string ptr_diff(T&& ptr)
    {
      ostringstream ostr;
      
      ostr << "When the pointer is '" << get_typeof(forward<T>(ptr)) << "', (pointer + 1) - pointer in bytes is: " << reinterpret_cast<unsigned long>(ptr + 1) - reinterpret_cast<unsigned long>(ptr);
      
      return ostr.str(); 	
    }

    int a1[3] = { 1, 2, 3};
    
    int b2[2][3] = {{ 1, 2, 3}, {4, 5, 6}};
    
    int c[2][2][3] = {  {{ 1, 2, 3}, {4, 5, 6}}, {{ 7, 8, 9}, {10, 11, 12}} };

    cout << ptr_diff("a1", a1) << '\n';
    cout << ptr_diff("&a1[0]", &a1[0]) << "\n\n";

    cout << ptr_diff("b2", b2) << '\n';
    cout << ptr_diff("&b2[0]", &b2[0]) <<  "\n\n";

    cout << ptr_diff("c", c) << '\n';
    cout << ptr_diff("&c[0]", &c[0]) <<  "\n\n";

whose ouput is:

.. raw:: html

    <pre>
    Pointer a1 is of type 'int [3]', and (pointer + 1) - pointer in bytes is: 4
    Pointer &a1[0] is of type 'int*', and (pointer + 1) - pointer in bytes is: 4
    
    Pointer b2 is of type 'int [2][3]', and (pointer + 1) - pointer in bytes is: 12
    Pointer &b2[0] is of type 'int (*) [3]', and (pointer + 1) - pointer in bytes is: 12
    
    Pointer c is of type 'int [2][2][3]', and (pointer + 1) - pointer in bytes is: 24
    Pointer &c[0] is of type 'int (*) [2][3]', and (pointer + 1) - pointer in bytes is: 24
    </pre>    

One Dimensional Arrays
^^^^^^^^^^^^^^^^^^^^^^

Given a one dimensional array such as **int a[]  {1, 2, 3, 4, 5}**, the address of its first element **&a[0]** is of type **int \*** as the code below illustrates. **a** is equivalent to **&a[0]**:

.. code-block:: cpp

    int a[] = {1, 2, 3, 4, 5};
    int *p1 = &a[0]; // p point to the first int in the block of elements comprising a  
    int *p2 = a;     // equivalent to line above.
    int *q = new int{9}; // q points to int on the heap with a value of 9

Adding one to a pointer does not increase the pointer's address by one but rather advances the address by **sizeof(int)** bytes, advancing it to the next integer, so adding one to the pointer **int *p1** above advances it to the next element in the array **a[1]**.
In fact, **a[1]** is equivalent to **\*(a + 1)**. In general **p + n**, where **n** is an int advances to the n + 1 :sup:`th` element (recall: arrays use zero-base indexing).

.. code-block:: cpp

    int a[] = {1, 2, 3, 4, 5};
    int *p = &a[0];      // p point to the first int in the block of elements comprising a  
    p = p + 4;
    cout << "p is equal to 5 is " << (*p == 5 ? "true" : "false"); // "p is equal to 5 is true"

The name of the array itself, here **a**, is synomous with **&a[0]**. Thus we can loop through the array with this for loop:

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
It uses the GCC extension __cxa_demanage() from the header <cxxabi.h> to demangle the output of **typeid()**.

.. code-block:: cpp

    #include <string>
    #include <iostream>
    #include <cxxabi.h>
    #include <sstream>
    #include <utility>
    using namespace std;
    
    template<class T> string get_typeof(const T& t)
    {
      const std::type_info  &ti = typeid(t);
      
      int status;
    
      char *realname = abi::__cxa_demangle(ti.name(), 0, 0, &status);   
      
      return string{realname};
    }
    
    template<class T> string ptr_diff(T&& ptr)
    {
      ostringstream ostr;
      
      ostr << "When the pointer is '" << get_typeof(forward<T>(ptr)) << "', (pointer + 1) - pointer in bytes is: " << reinterpret_cast<unsigned long>(ptr + 1) - reinterpret_cast<unsigned long>(ptr);
      
      return ostr.str(); 	
    }
    
    int a[5] = {1, 2, 3, 4, 5};
    
    cout << "Type of a is: int a[5]" << ". \n";
    
    cout << "The type of a is: " << get_typeof(a) << "\n";
    
    cout << "The type of &a[0] is: " << get_typeof(&a[0]) << '\n'; 
    
    cout << "The type of &a is: " << get_typeof(&a) << "\n";
    
    cout << "---------------------------------\n";
    
    cout << ptr_diff(&a[0]) << '\n';   
    
    cout << ptr_diff(&a) << '\n';  

    cout << "---------------------------------\n";
            
    cout << "Below are the above types when dereferenced:\n\n";
    
    cout << "a is: int a[5]\n";
    
    cout << "The type of *a is: " << get_typeof(*a) << "\n";
    
    cout << "The type of *&a[0] is: " << get_typeof(*&a[0]) << '\n'; 
      
    cout << "The type of *&a is: " << get_typeof(*&a) << "\n";
    
    cout << "The type of **&a is: " << get_typeof(**&a) << "\n";
  
Two dimensional and higher arrays are still stored, like one dimensional arrays, as one contiguous linear block, with the first row or block of values followed by the next row or block of values.
The code below shows the types of various addresses of 2-dimensional arrays (using a GCC extension to demangle the ouput of **typeid()**), the difference in bytes when using pointer addtion, and it shows the corresponding dereference types. 

.. code-block:: cpp

    #include <string>
    #include <iostream>
    #include <cxxabi.h>
    #include <sstream>
    #include <utility>
    using namespace std;
    
    // GCC special extension to convert mangled name to real name
    // See https://gcc.gnu.org/onlinedocs/libstdc++/manual/ext_demangling.html
    template<class T> string get_typeof(const T& t)
    {
      const std::type_info  &ti = typeid(t);
      
      int status;
    
      char *realname = abi::__cxa_demangle(ti.name(), 0, 0, &status);   
      
      return string{realname};
    }
    
    template<class T> string ptr_diff(T&& ptr)
    {
      ostringstream ostr;
      
      ostr << "When the pointer is '" << get_typeof(forward<T>(ptr)) << "', (pointer + 1) - pointer in bytes is: " << reinterpret_cast<unsigned long>(ptr + 1) - reinterpret_cast<unsigned long>(ptr);
      
      return ostr.str(); 	
    }
    
    int a[5] = {1, 2, 3, 4, 5};
    
    cout << "Type of a is: int a[5]" << ". \n";
    
    cout << "The type of a is: " << get_typeof(a) << "\n";
    
    cout << "The type of &a[0] is: " << get_typeof(&a[0]) << '\n'; 
    
    cout << "The type of &a is: " << get_typeof(&a) << "\n";
    
    cout << "---------------------------------\n";
    
    cout << ptr_diff(&a[0]) << '\n';   
    
    cout << ptr_diff(&a) << '\n';  

    cout << "---------------------------------\n";
            
    cout << "Below are the above types when dereferenced:\n\n";
    
    cout << "a is: int a[5]\n";
    
    cout << "The type of *a is: " << get_typeof(*a) << "\n";
    
    cout << "The type of *&a[0] is: " << get_typeof(*&a[0]) << '\n'; 
      
    cout << "The type of *&a is: " << get_typeof(*&a) << "\n";
    
    cout << "The type of **&a is: " << get_typeof(**&a) << "\n";
    
Produces this output

.. raw:: html

    <pre>
    Type of a is: int a[5]. 
    The type of a is: int [5]
    The type of &a[0] is: int*
    The type of &a is: int (*) [5]
    ---------------------------------
    When the pointer is 'int*', (pointer + 1) - pointer in bytes is: 4
    When the pointer is 'int (*) [5]', (pointer + 1) - pointer in bytes is: 20
    ---------------------------------
    a is: int a[5]
    The type of *a is: int
    The type of *&a[0] is: int
    The type of *&a is: int [5]
    The type of **&a is: int
    </pre> 

.. todo:: Add comments on above

This code:

.. code-block:: cpp
    
        int b[2][5] = {{1, 2, 3, 4, 5}, {6, 7, 8, 9, 10}}; // Same as: int a[][5] ={{1, 2, 3, 4, 5}, {6, 7, 8, 9, 10}}; 
        
        int *p1 = &b[0][0]; 
    
        int (*p2)[5] = &b[0]; 
    
        int (*p3)[5] = b;
        
        int (*p4)[2][5] = &b;
        
        cout << "b is: int b[2][5]\n";
        
        cout << "The type of &b[0][0] is: " << get_typeof(&b[0][0]) << '\n'; 
    
        cout << "The type of &b[0] is: " << get_typeof(&b[0]) << '\n'; 
          
        cout << "The type of b is: " << get_typeof(b) << "\n";
        
        cout << "The type of &b is: " << get_typeof(&b) << "\n";
        
        cout << "----------------------------------\n\n";
        
        cout << ptr_diff(&b[0][0]) << '\n';   
    
        cout << ptr_diff(&b[0]) << '\n';   
        
        cout << ptr_diff(b) << '\n';  
        
        cout << ptr_diff(&b) << '\n';  
             
        cout << "----------------------------------\n\n";
        
        cout << "b is: int b[2][5]\n";
        
        cout << "The type of *&b[0] is: " << get_typeof(*&b[0]) << '\n'; 
          
        cout << "The type of *b is: " << get_typeof(*b) << "\n";
        
        cout << "The type of *&b is: " << get_typeof(*&b) << "\n";

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

.. todo:: Add comments on above

.. todo:: 1. Continue working from https://www.cse.msu.edu/~cse251/lecture11.pdf, slide 26.
 
Preliminary Summary of 2-dimensional array pointers
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
  
This shows that for a two dimensional array:

1. **&a[0][0]** is an **int \*** pointing to the first element of the array, and adding one to it advances the pointer **sizeof(int)** bytes (or four bytes) to the next int **a[0][1]**. 
2. Both **&a[0]** and **a** are of type **int (*)[5]**, pointer to a block of five consecutive integers, and adding one to it advances the pointer **4 x sizeof(int)** or 20 bytes to the next block of five consecutive integers
3. **&a** is of type **int (*)[2][5]**, a pointer to two blocks of 'a block of five integers', and adding one to it advances its address **2 x (4 x sizeof(int))** or 40 bytes to the next block of two blocks of 'a block of five integers'.

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
    pointer ptr1 is '&b[0][0][0]', and type is 'int *'.
    pointer = 0x7ffcbf5ad810.  pointer + 1 = 0x7ffcbf5ad814
    The difference in bytes between pointer and (pointer + 1) = 4
    
    pointer ptr2 is '&b[0][0]', and type is 'int (*)[5]'.
    pointer = 0x7ffcbf5ad810.  pointer + 1 = 0x7ffcbf5ad824
    The difference in bytes between pointer and (pointer + 1) = 20
    
    pointer ptr3 is '&b[0]', and type is 'int (*)[2][5]'.
    pointer = 0x7ffcbf5ad810.  pointer + 1 = 0x7ffcbf5ad838
    The difference in bytes between pointer and (pointer + 1) = 40
    
    pointer ptr4 is 'b', and type is 'int (*)[2][5]'.
    pointer = 0x7ffcbf5ad810.  pointer + 1 = 0x7ffcbf5ad838
    The difference in bytes between pointer and (pointer + 1) = 40
    
    pointer ptr5 is '&b', and type is 'int (*)[3][2][5]'.
    pointer = 0x7ffcbf5ad810.  pointer + 1 = 0x7ffcbf5ad888
    The difference in bytes between pointer and (pointer + 1) = 120
    </pre>

which show that for a three dimensional array:

1. **&b[0][0][0]** is an **int \***, pointing to **b[0][0][0]**, and adding one to it advances the pointer **sizeof(int)** or four byes to the next int **&b[0][0][1]**.
2. **&b[0][0]** is of **int (*)[5]**, or pointer to a block of five consecutive integers, and adding one to such a pointer advances the pointer **4 x sizeof(int)** or 20 bytes to the next block of five integers **&b[0][1]**
3. **&b[0]** is of type **int (*)[2][5]**, a pointer to two blocks of a block of five integers each. So adding one to such a pointer advances its address **2 x (4 x sizeof(int))** or 40 bytes to the next block of two blocks of **a block of five integers**
   or **&b[1]**.  
4. **b** is also synonomous to **&b[0]** and so is of type **int (*)[2][5]**, a pointer to two blocks of a block of five integers each, and likewise adding one to such a pointer advances its address **2 x (4 x sizeof(int))** or 40 bytes to the next block of two
    blocks of a block of five integers each or **&b[1]**.
5. **&b** is of type **int (*)[3][2][5]**, a pointer to three blocks of two block of a block five integers each. So adding one to such a pointer advances its address **3 x (2 x (4 x sizeof(int)))** or 120 bytes to the next block of three blocks of two blocks of
    a block of five integers each, which is the address of one beyond **b[1][1][4]**, the last element in the array.

Summary of Pointers and Arrays
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Notes from `Pointer, Arrays and Multidimensional Arrays <https://www.cse.msu.edu/~cse251/lecture11.pdf>`_:

Given

.. code-block:: cpp

    int myMatrix[2][4] = { {1,2,3,4},{5,6,7,8} };

.. todo:: Add explanations to each of these:

* **myMatrix[0]**: pointer to the first row of the 2D array
* **myMatrix[1]**: pointer to the second row of the 2D array
* **\*myMatrix[1]** is the address of element **myMatrix[1][0]**

Indexing: **myMatrix[i][j]** is same as:

.. todo:: Add explanations to each of these. Read `Multidimensional Pointer Arithmetic in C/C+ <https://www.geeksforgeeks.org/multidimensional-pointer-arithmetic-in-cc/>`_ and compare it with the output of ~/test code, keeping in my that when get_typeof() returns
    'int [4]' is actually refers to the array of the array, with the info in the first link below

* **\*((*(myMatrix + 1)) + 2)**       

  **myMatrix** is the same as **&myMatrix[0]**, which are of type **int (*4)**. This is the the address of the first row of **myMatrix**. Adding one **(myMatrix + 1)** advances the pointer to the second row of **myMatrix**. Deferencing 
  **\*(myMatrix + 1)** returns an **int \*** to the first element of the one-dimensional array **{5, 6, 7, 8}**. **\*(myMatrix + 1)** is equivalent to **int *p = &myMatrix[1][0]**. Then adding 2, **(*(myMatrix + 1)) + 2**, advances the **int \*** to the third element
  of the array **{5, 6, 7, 8}** , and then deferencing it **\*((*(myMatrix + 1)) + 2)** returns the integer at that position **7**.

* **\*(&myMatrix[0][0] + 4 * i + j)** where i = 1 and j = 2; 

* **\*(myMatrix[1] + 2)**

* **(*(myMatrix + 1))[2]**


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
