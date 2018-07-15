.. include:: <isopub.txt>

Templates and Specializations
=============================

Links to Template specialization and partial specialization
-----------------------------------------------------------

* https://en.cppreference.com/w/cpp/language/partial_specialization 
* https://en.cppreference.com/w/cpp/language/template_specialization
* https://www.ibm.com/support/knowledgecenter/en/SSLTBW_2.3.0/com.ibm.zos.v2r3.cbclx01/template_specialization.htm

Other links:
++++++++++++

* http://www.bogotobogo.com/cplusplus/templates.php
* https://www.cprogramming.com/tutorial/template_specialization.html 
* http://www.learncpp.com/cpp-tutorial/137-partial-template-specialization/

Partial Template Specialization Examples
----------------------------------------

This modified example is from the http://en.cppreference.com article on `partial template specialization <https://en.cppreference.com/w/cpp/language/partial_specialization>`_. It shows by example how a template can be partially specialized:

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
++++++++

The output from the above will be:

::

     A<int, int, 1> a1 uses primary template.
     A<int, int *, 1> a2 uses partial template specialization #1 'class A<T, T*, I>', in which the 2nd parameter is a pointer to 'type of the first parameter'.
     A<int, char *, 5> a3 uses partial template specialization #3 'class A<int, T*, 5>', in which the first parameter is an int, and the 2nd is a pointer and the third is the scalar 5
     A<int, char *, 1> a4 uses partial template specialization #4 'class A<X, T*, I>', in which The second parameter is a pointer.
     
For ``a1`` no specializations exist that match the template parameters, so the primary template is used. In the case of ``a2``, where the second parameter is ``int *`` and the first parameter is ``int``, partial specialization #1 is more specialized than the primary
template. To say  "A is more specialized than B" means "A accepts a subset of the types that B accepts". In the case of ``a3``, the third parameter of ``5`` and the second parameter of pointer type, make #3 the only template partial specialization that matches.
For ``a5`` above, no most-specialized template can be found since ``a5`` matches #2 and #4 equally.

std::vector Example of Partial Template Specializaition
+++++++++++++++++++++++++++++++++++++++++++++++++++++++

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

Explicit (Full) Template Specializations examples
-------------------------------------------------

This material is from IBM Knowledge Base article on template `Explicit specialization <https://www.ibm.com/support/knowledgecenter/en/SSLTBW_2.3.0/com.ibm.zos.v2r3.cbclx01/explicit_specialization.htm>`_.

Definition and declaration of explicit specializations
++++++++++++++++++++++++++++++++++++++++++++++++++++++

The declaration of full template specialization in introduced with ``template<>`` as the example of ``template<> class Sample<int>;`` below shows:

.. code-block:: cpp

    template<class T> class Sample {
       T t;
     public: 
       Sample() {}
       std::ostream& print(std::ostream&) const;
    };
     
    template<> class Sample<int> {
       int i1;
       int i2;
     public: 
       Sample() : i1{10}, i2{20} {}
       std::ostream& print(std::ostream&) const;
    };
    
    template<class T> std::ostream& Sample<T>::print(std::ostream& ostr) const
    {
       return ostr << "Primary Sample template. t = " << t << std::endl;
    } 
    
    std::ostream& Sample<int>::print(std::ostream& ostr) const
    {
       return ostr << "Sample<int>. i1 = = " << i1 << " and i2 = " << i2 << std::endl;
    }
    using namespace std;
    
    int main(int argc, char** argv) 
    {
        Sample<float> s1;
        s1.print(cout);
        Sample<int>  s2;
     
        s2.print(cout);
        
        return 0;
    }
 
.. note:: 

   Note: ``template<>`` does not preceeded the definiton of ``std::ostream& Sample<int>::print(std::ostream& ostr) const``

Definition and declaration of explicit specializations
++++++++++++++++++++++++++++++++++++++++++++++++++++++

The definition of an explicitly specialized class is unrelated to the definition of the primary template. You do not have to define the primary template in order to define the specialization (nor do you have to define the specialization in order to define the primary
template). See the following example:

.. code-block:: cpp

    template<class T> class A;
    template<> class A<int>;
    
    template<> class A<int> { /* ... */ };
    
The primary template is not defined, but the explicit specialization is.
You can use the name of an explicit specialization that has been declared but not defined the same way as an incompletely defined class. The following example demonstrates this:
    
.. code-block:: cpp

    template<class T> class X { };
    template<>  class X<char>;
    X<char>* p;
    X<int> i;
    // X<char> j;
        
The compiler does not allow the declaration X<char> j because the explicit specialization of X<char> is not defined.
