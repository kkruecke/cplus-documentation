.. include:: <isopub.txt>

Templates and Specializations
=============================

Links to Template specialization and partial specialization
-----------------------------------------------------------

* https://en.cppreference.com/w/cpp/language/partial_specialization 

Other links:
------------

* https://www.ibm.com/support/knowledgecenter/en/SSLTBW_2.2.0/com.ibm.zos.v2r2.cbclx01/templates.htm 
* http://www.bogotobogo.com/cplusplus/templates.php
* https://www.cprogramming.com/tutorial/template_specialization.html 
* http://www.learncpp.com/cpp-tutorial/137-partial-template-specialization/


Full Template Specializations
-----------------------------

.. todo:: use links above to complete this section

Partial Template Specializations examples
-----------------------------------------

This modified example is from the en.cppreference.com article on `partial template specialization <https://en.cppreference.com/w/cpp/language/partial_specialization>`_. It shows how a template can be partially specialized:

.. code-block:: cpp

     template<class T1, class T2, int I>
     class A {
         public:
        void describe()
        {
          cout << "uses primary template." << endl;
     
        } 
     };            // primary template
      
     template<class T, int I> class A<T, T*, I> { // #1: partial specialization in which the 2nd parameter is a pointer to 'type of the first parameter.' 
     public:
        void describe()
        {
          cout << "uses partial template specialization #1 'class A<T, T*, I>', in which the 2nd parameter is a pointer to 'type of the first parameter'." << endl;
     
        } 
     }; 
      
     template<class T, class T2, int I>
     class A<T*, T2, I> { // #2: partial specialization in which the first parameter is a pointer.
     public:
        void describe()
        {
          cout << "uses partial template specialization #2 'class A<T*, T2, I>', in which the first parameter is a pointer." << endl;
        } 
     
     };
      
     template<class T>
     class A<int, T*, 5> { // #3: partial specialization in which first parameter is an int, 2nd is a pointer and the third is the scalar 5
     public:
        void describe()
        {
          cout << "uses partial template specialization #3 'class A<int, T*, 5>', in which the first parameter is an int, and the 2nd is a pointer and the third is the scalar 5." << endl;
        } 
     
     }; 
      
     template<class X, class T, int I>
     class A<X, T*, I> {  // #4: partial specialization in which the second parameter is a pointer.
     public:
        void describe()
        {
          cout << "uses partial template specialization #4 'class A<X, T*, I>', in which The second parameter is a pointer." << endl;
        } 
     }; 
     
and this code shows when the the primary template or its specializations is instantiated:

.. code-block:: cpp

     // given the template A as defined above and its partial specializations:

     A<int, int, 1> a1;   // no specializations match, uses primary template
     cout << "A<int, int, 1> a1 ";
     a1.describe();

     A<int, int *, 1> a2;  // uses partial specialization #1 (T=int, I=1)
     cout << "A<int, int *, 1> a2 ";
     a2.describe();

     A<int, char *, 5> a3; // uses partial specialization #3, (T=char)
     cout << "A<int, char *, 5> a3 ";
     a3.describe();

     A<int, char *, 1> a4; // uses partial specialization #4, (X=int, T=char, I=1)
     cout << "A<int, char *, 1> a4 ";
     a4.describe();

     A<int*, int*, 2> a5; // error: matches #2 (T=int, T2=int*, I=2)
                          //        matches #4 (X=int*, T=int, I=2)
                          // neither one is more specialized than the other

Comments
--------

The output from the above will be:

     A<int, int, 1> a1 uses primary template.
     A<int, int *, 1> a2 uses partial template specialization #1 'class A<T, T*, I>', in which the 2nd parameter is a pointer to 'type of the first parameter'.
     A<int, char *, 5> a3 uses partial template specialization #3 'class A<int, T*, 5>', in which the first parameter is an int, and the 2nd is a pointer and the third is the scalar 5
     A<int, char *, 1> a4 uses partial template specialization #4 'class A<X, T*, I>', in which The second parameter is a pointer.
     
For ``a1`` no specializations exist that match the template parameters, so the primary template is used. In the case of ``a2``, where the second parameter is ``int *`` and the first parameter is ``int``, partial specialization #1 is more specialized than the primary
template. To say  "A is more specialized than B" means "A accepts a subset of the types that B accepts". In the case of ``a3``, the third parameter of ``5`` and the second parameter of pointer type, make #3 the only template partial specialization that matches.
For ``a5`` above, no most-specialized template can be found since ``a5`` matches #2 and #4 equally.

std::vector Example of Partial Template Specializaition
-------------------------------------------------------

Another, real-world example of partial template specialization comes from the GNU implementation of the C++ standard library container ``std::vector``. GNU g++ defines the primary ``std::vector`` template container like:

.. code-block:: cpp

 template<typename _Tp, typename _Alloc = std::allocator<_Tp> >
    class vector : protected _Vector_base<_Tp, _Alloc>
    {
    //...
    };

and it declares a partial specialization of ``vector<T, Alloc>`` for type ``bool`` like this:

.. code-block:: cpp

     template<typename _Alloc>
       class vector<bool, _Alloc> : protected _Bvector_base<_Alloc>
       {
       //... 
       };







     
