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

The declaration of full template specialization must be preceeded by ``template<>``, as the example of ``template<> class Sample<int>;`` below illustrates:

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

    // Note: template<> does not preceed member definition
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
template). In this example:

.. code-block:: cpp

    template<class T> class A;
    template<> class A<int>;
    
    template<> class A<int> { /* ... */ };
    
the primary template A above is not defined, but the explicit specialization for type **int** is.

You can also use the name of an explicit specialization that has been declared but not defined in the same way as an incompletely defined class. The following example demonstrates this:
    
.. code-block:: cpp

    template<class T> class X { };
    template<>  class X<char>;
    X<char>* p;
    X<int> i;
    // X<char> j;
        
The compiler does not allow the declaration ``X<char> j`` because the explicit specialization ``X<char>`` is not defined(only declared).

You define members of an explicitly specialized template class as you would normal classes, without the ``template<>`` prefix. In addition, you can define the members of an explicit specialization inline; no special template syntax is used in this case. The following
example demonstrates a class template specialization:

.. code-block:: cpp

    template<class T> class A {
       public:
          void f(T);
    };
    
    template<> class A<int> {
       public:
          int g(int);
    };
    
    int A<int>::g(int arg) { return 0; }
    
    int main() {
       A<int> a;
       a.g(1234);
    }

.. note:: Explicit specialization ``A<int>`` contains member function ``g()``, **not in the primary template**. So full template specializations can exclude functions found in the primary template, and they can include extra methods not found in the primary template.

Explicit specialization of function templates
+++++++++++++++++++++++++++++++++++++++++++++

In a function template specialization, a template argument is optional if the compiler can deduce it from the type of the function arguments. The following example demonstrates this:

.. code-block:: cpp

    template<class T> class X { };
    template<class T> void f(X<T>);
    template<> void f(X<int>);
    
The explicit specialization ``template<> void f(X<int>)`` is equivalent to ``template<> void f<int>(X<int>)``.

You cannot specify default function arguments in a declaration or a definition for any of the following cases:

    Explicit specialization of a function template
    Explicit specialization of a member function template

As an example the compiler will not allow the following code:

.. code-block:: cpp

    template<class T> void f(T a) { };
    template<> void f<int>(int a = 5) { }; // error: default argument(s) not permitted in function template specialization
    
    template<class T> class X {
      void f(T a) { }
    };

    template<> void X<int>::f(int a = 10) { };// error: default argument(s) not allowed in member function of full
                                              // template specialization

Explicit specialization of members of class templates
-----------------------------------------------------

Each instantiated class template specialization has its own copy of any static members. You may explicitly specialize static members. The following example demonstrates this:

.. code-block:: cpp

    template<class T> class X {
    public:
       static T v;
       static void f(T);
    };

    // primary template static member definitions
    template<class T> T X<T>::v = 0; 

    template<class T> void X<T>::f(T arg) { v = arg; } 

    // Specializaions of X<char*>::v and X<float>::f(float arg) require
    // separate definitions of the static member 
    
    template<> std::string X<std::string>::v{"Hello"};
    template<> void X<float>::f(float arg) { v = arg * 2; }
    
    int main() 
    {
        X<int> x;
       cout << "x, which is X<int>, has X<int>::v = " << X<int>::v << '\n';
    
       X<string> a, b;
       cout << "a and b are X<std::string>, and X<std::string>::v = " << X<string>::v << '\n';
    
       X<float> c;
       cout << "c is X<float>, and X<float>::v = " << X<float>::v << '\n';
       c.f(10);
       cout << "After c.f(10), X<float>::v = " <<  X<float>::v << '\n';
    }

yields this output:

::

    x, which is X<int>, has X<int>::v = 0
    a and b are X<std::string>, and X<std::string>::v = Hello
    c is X<float>, and X<float>::v = 0
    After c.f(10), X<float>::v = 20
