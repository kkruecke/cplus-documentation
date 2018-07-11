.. include:: <isopub.txt>

Template and Specializations
============================

Links to Template specialization and partial specialization
-----------------------------------------------------------

* https://en.cppreference.com/w/cpp/language/partial_specialization 
* https://www.ibm.com/support/knowledgecenter/en/SSLTBW_2.2.0/com.ibm.zos.v2r2.cbclx01/templates.htm 
* http://www.bogotobogo.com/cplusplus/templates.php
* https://www.cprogramming.com/tutorial/template_specialization.html 
* http://www.learncpp.com/cpp-tutorial/137-partial-template-specialization/

This example from `en.cppreference.com <https://en.cppreference.com/w/cpp/language/partial_specialization>`_ shows how a template is partially specialized:

.. code-block:: cpp

     template<class T1, class T2, int I>
     class A {};            // primary template
      
     template<class T, int I>
     class A<T, T*, I> {};  // #1: partial specialization where T2 is a pointer to T1
      
     template<class T, class T2, int I>
     class A<T*, T2, I> {}; // #2: partial specialization where T1 is a pointer
      
     template<class T>
     class A<int, T*, 5> {}; // #3: partial specialization where T1 is int, I is 5,
                             //     and T2 is a pointer
      
     template<class X, class T, int I>
     class A<X, T*, I> {};   // #4: partial specialization where T2 is a pointer

and how each applies:

.. code-block:: cpp

     // given the template A as defined above and its partial specializations:

     A<int, int, 1> a1;   // no specializations match, uses primary template

     A<int, int *, 1> a2;  // uses partial specialization #1 (T=int, I=1)

     A<int, char *, 5> a3; // uses partial specialization #3, (T=char)

     A<int, char *, 1> a4; // uses partial specialization #4, (X=int, T=char, I=1)

     A<int*, int*, 2> a5; // error: matches #2 (T=int, T2=int*, I=2)
                          //        matches #4 (X=int*, T=int, I=2)
                          // neither one is more specialized than the other

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
     
