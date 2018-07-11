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

Partial Template Specializations examples
-----------------------------------------

This modified example is from the `en.cppreference.com article on partial template specialization <https://en.cppreference.com/w/cpp/language/partial_specialization>`_. It shows how a template can be partially specialized:

.. code-block:: cpp

     template<class T1, class T2, int I>
     class A {};            // primary template
      
     template<class T, int I>
     class A<T, T*, I> { // #1: partial specialization where T2 is a pointer to T1; i.e., T2 is 'T1 *' 

        void describe()
        {
          cout << "In partial template specialization #1 'class A<T, T*, I>'" << endl;
        } 
     }; 
      
     template<class T, class T2, int I>
     class A<T*, T2, I> { // #2: partial specialization where T1 is a pointer, i.e., the first parameter
                          // is of pointer type.

        void describe()
        {
          cout << "In partial template specialization #2 'class A<T*, T2, I>'" << endl;
        } 

     };
      
     template<class T>
     class A<int, T*, 5> { // #3: partial specialization where T1 is int, I is 5, and T2 is a pointer

        void describe()
        {
          cout << "In partial template specialization #3 'class A<int, T*, 5>'" << endl;
        } 
  
     }; 
      
     template<class X, class T, int I>
     class A<X, T*, I> {  // #4: partial specialization where T2 is a pointer

        void describe()
        {
          cout << "In partial template specialization #4 'class A<X, T*, I>'" << endl;
        } 
     }; 

and this code shows when the the primary template or its specializations is instantiated:

.. code-block:: cpp

     // given the template A as defined above and its partial specializations:

     A<int, int, 1> a1;   // no specializations match, uses primary template
     a1.describe();

     A<int, int *, 1> a2;  // uses partial specialization #1 (T=int, I=1)
     a2.describe();

     A<int, char *, 5> a3; // uses partial specialization #3, (T=char)
     a3.describe();

     A<int, char *, 1> a4; // uses partial specialization #4, (X=int, T=char, I=1)
     a4.describe();

     A<int*, int*, 2> a5; // error: matches #2 (T=int, T2=int*, I=2)
                          //        matches #4 (X=int*, T=int, I=2)
                          // neither one is more specialized than the other

The output will be:

     In primary template 'class A<T1, T2, I>'
     In partial template specialization #1 'class A<T, T*, I>'
     In partial template specialization #3 'class A<int, T*, 5>'
     In partial template specialization #4 'class A<X, T*, I>'

An example from the GNU implementation of the C++ standard library. GNU g++ defines the ``std::vector`` template like:

.. code-block:: cpp

 template<typename _Tp, typename _Alloc = std::allocator<_Tp> >
    class vector : protected _Vector_base<_Tp, _Alloc>
    {
    //...
    };

And it declares a partial specialization of ``vector<T, Alloc>`` for type ``bool`` like this:

.. code-block:: cpp

     template<typename _Alloc>
       class vector<bool, _Alloc> : protected _Bvector_base<_Alloc>
       {
       //... 
       };
     
