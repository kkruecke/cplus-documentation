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

Given the array ``some_array``, ``&some_array[0]`` and ``some_array`` are identical pointers that both point to the first element in the array. The code below,
compiled using **g++ -std=c++2a**, illustrates this.

.. code-block:: cpp

    #include <cxxabi.h>
    #include <string>
    #include <utility>
    #include <iostream>
    #include <sstream>

    // Demangles the output of std::typeid().
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

    // ptr_diff() is a wrapper function that accetps both rvalues and lvalues parameters for its 2nd parameter,
    // which is forwarded to get_typeof()
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

.. note: When ``c1`` is used as a pointer in code without any index operators present, it is  the same as using ``&c1[0]``. It has the same type as
   ``&c1[0]``, it holds the same address.  

One Dimensional Arrays
^^^^^^^^^^^^^^^^^^^^^^

Given the one dimensional array ``int a[] = {1, 2, 3, 4, 5}``, the address of its first element ``&a[0]`` is of type **int \***, as the code below illustrates, and ``a`` is exactly equivalent to ``&a[0]``:

.. code-block:: cpp

    int a[] = {1, 2, 3, 4, 5};
    int *p1 = &a[0]; // p point to the first int in the block of elements comprising a  
    int *p2 = a;     // equivalent to line above.
    int *q = new int{9}; // q points to int on the heap with a value of 9

The index operator ``a[n]`` is equivalent to ``*(a + n)``. Adding one to a pointer does advances it by ``sizeof(int)`` bytes because pointer addition is scaled based on the underlying pointed-to type. In the case of ``*p1 + 1``, the pointer is
advanced to the address of next element in the array, to ``&a[1]``.

.. code-block:: cpp

    int a[] = {1, 2, 3, 4, 5};
    int *p = &a[0];      // p point to the first int in the block of elements comprising a  
    p = p + 4;
    cout << "p is equal to 5 is " << (*p == 5 ? "true" : "false"); // "p is equal to 5 is true"

Again, the name of the array itself, here ``a``, is synonymous with ``&a[0]``. Thus we can loop through the array with following for\ |ndash|\ loop:

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
 
Two dimensional and higher arrays are still stored like one dimensional arrays, as one contiguous linear block, with the first row or block of values followed by the next row. When the implicit array-to-pointer decay is applied, a multidimensional
array is converted to a pointer to its first element, i.e., a pointer to its first row or to its first plane. The code below illustrates this:

.. code-block:: cpp

    int a[2] = { 1, 2};  // array of 2 int
    
    int* p1 = a;         // a decays to a pointer to the first element of a, which is an int.
    
    cout << "*p1 = " << *p1 << endl;
     
    int b[2][3] = { {1, 2, 3}, {10, 20, 30}}; // array of 2 arrays of 3 int
    
    int** p2 = b;        // error: b does not decay to int**
    
    int (*p2)[3] = b;    // b decays to a pointer to the first 3-element row of b
    
    // While b and &b[0][0] have the same address, they are not of the same pointer type.
    string str = (reinterpret_cast<void*>(&b[0][0]) == reinterpret_cast<void*>(b)) ? "true" : "false";
    
    cout << "b is 'int b[2][3]', and the result of \n";
    cout << "reinterpret_cast<void*>(&b[0][0]) == reinterpret_cast<void*>(v) is " << str << endl;
    
    // This loop shows how muli-dimentional arrays can be accessed via pointer notation, and it
    // p2 is equivalent to b.
    for (auto row = 0; row < 2; ++row) {
        cout << "{ ";
        for (auto column = 0; column < 3; ++column)
            cout << *(*(p2 + row) + column) << ", ";
        cout << "}, ";
    }
    
    cout << endl;

    // Access the array using index operator    
    for (auto row = 0; row < 2; ++row) {
        cout << "{ ";
        for (auto column = 0; column < 3; ++column)
            cout << p2[row][column] << ", ";
        cout << "}, ";
    }
    
    int c[2][3][4];       // array of 2 arrays of 3 arrays of 4 int
    
    int*** p3 = c;        // error: c does not decay to int***

    int (*p3)[3][4] = c;  // c decays to a pointer to the first 3 × 4-element plane of c

    // p4 points to the first 3 x 4 array, so *p4 points to the first row, and **p4 points to the first element
    int *p5 = **p4;        
