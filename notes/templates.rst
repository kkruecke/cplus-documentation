.. include:: <isopub.txt>

Templates and Specializations
=============================

Partial Template Specialization Examples
----------------------------------------

This modified example, taken from `partial template specialization <https://en.cppreference.com/w/cpp/language/partial_specialization>`_, shows how a primary class template can be partially specialized:

.. code-block:: cpp

     // Primary template
     template<typename T1, typename T2, int I>
     class A {
        public:
        void describe()
        {
          cout << "Primary class template<typename T1, typename T2, int I>." << endl;
     
        } 
     };  

     // #1: Partial specialization in which the 2nd parameter is always a pointer to the type of the first generic parameter. 
     template<typename T, int I> class A<T, T*, I> { 
        public:
        void describe()
        {
          cout << "uses partial template specialization #1 'class A<T, T*, I>', in which the 2nd parameter is a pointer to 'type of the first parameter'." << endl;
     
        } 
     }; 

     // #2: Partial specialization in which the first parameter is always a pointer.
     template<typename T, typename T2, int I>
     class A<T*, T2, I> { 
        public:
        void describe()
        {
          cout << "uses partial template specialization #2 'class A<T*, T2, I>', in which the first parameter is a pointer." << endl;
        } 
     
     };

     // #3: Partial specialization in which first parameter is always int, the 2nd parameter is always a pointer
     //  and the third is the scalar 5
     template<typename T>
     class A<int, T*, 5> { 
        public:
        void describe()
        {
          cout << "uses partial template specialization #3 'class A<int, T*, 5>', in which the first parameter is an int, and the 2nd is a pointer and the third is the scalar 5." << endl;
        } 
     
     }; 

     // #4: Partial specialization in which the second parameter is always a pointer.
     template<typename X, typename T, int I>
     class A<X, T*, I> {  
        public:
        void describe()
        {
          cout << "uses partial template specialization #4 'class A<X, T*, I>', in which The second parameter is a pointer." << endl;
        } 
     }; 
     
The output from these partial template instantiations further illustrate when a partial template specializations occurs:

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

The output from the above is:

.. raw:: html

     <pre>
     A<int, int, 1> a1 uses primary template.
     A<int, int *, 1> a2 uses partial template specialization #1 'class A<T, T*, I>', in which the 2nd parameter is a pointer to 'type of the first parameter'.
     A<int, char *, 5> a3 uses partial template specialization #3 'class A<int, T*, 5>', in which the first parameter is an int, and the 2nd is a pointer and the third is the scalar 5
     A<int, char *, 1> a4 uses partial template specialization #4 'class A<X, T*, I>', in which The second parameter is a pointer.
     </pre>
     
For ``a1`` no specializations exist that match the template parameters, so the primary template is used. In the case of ``a2``, where the second parameter is ``int *`` and the first parameter is ``int``, partial specialization #1 is more specialized than the primary
template. To say  "A is more specialized than B" means "A accepts a subset of the types that B accepts". In the case of ``a3``, the third parameter of ``5`` and the second parameter of pointer type, make #3 the only template partial specialization that matches.
For ``a5`` above, no most-specialized template can be found since ``a5`` matches #2 and #4 equally.

Example of Partial Template Specializaition of std::vector 
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Another, real-world example of partial template specialization comes from the GNU implementation of the C++ standard library container ``std::vector``. GNU g++ defines the primary ``std::vector`` template container like:

.. code-block:: cpp

    template<typename _Tp, typename _Alloc = std::allocator<_Tp> >
      class vector : protected _Vector_base<_Tp, _Alloc>
      {
      //...
      };

and it declares a partial specialization of ``vector<T, Alloc>`` for ``bool`` like this:

.. code-block:: cpp

    template<typename _Alloc>
      class vector<bool, _Alloc> : protected _Bvector_base<_Alloc>
      {
      //... 
      };

Explicit or Full Template Specialization
----------------------------------------

A class template or a function template can be completely specialized for a particular type(s). In this case, the entire class or functoin has a custom reimplementation. The explict or full template specialization explanation examples below are from the IBM Knowledge Center article
explaining template `Explicit specialization <https://www.ibm.com/support/knowledgecenter/en/SSLTBW_2.3.0/com.ibm.zos.v2r3.cbclx01/explicit_specialization.htm>`_. 

Definition and declaration of explicit specializations
++++++++++++++++++++++++++++++++++++++++++++++++++++++

``template<>`` must be preceed a full template specialization, as the example of ``template<> class Sample<int>`` below illustrates.
 
.. note:: 

   Note: However ``template<>`` does not preceeded the definiton of **std::ostream& Sample<int>::print(std::ostream& ostr) const**. Instead the syntax is: **template<class T> std::ostream& Sample<T>::print(std::ostream& ostr)**.


.. code-block:: cpp

    template<class T> class Sample {
       T t;
     public: 
       Sample() {}
       std::ostream& print(std::ostream&) const;
    };

     // full template specialization for Sample<int>. There is an entirely separate implementation of Sample<int>. 
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

Definition and declaration of explicit specializations
++++++++++++++++++++++++++++++++++++++++++++++++++++++

The definition of an explicitly specialized class is unrelated to the definition of the primary template. You do not have to define the primary template in order to define the specialization (nor do you have to define the specialization in order to define the primary template). In this
example:

.. code-block:: cpp

    template<class T> class A; // Primary templae
    template<> class A<int>;   // Explicit specialization for A<int>
    
    template<> class A<int> { /* ... */ };
    
the primary template A above is not defined, but the explicit specialization for type **int** is.

You can also use the name of an explicit specialization that has been declared but not defined in the same way as an incompletely defined class. The following example demonstrates this:
    
.. code-block:: cpp

    template<class T> class X { };
    template<>  class X<char>; // Declared but not defined
    X<char>* p;
    X<int> i;
    // X<char> j; Error: X<char> not defined
        
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

Explicit Specialization of Nested Templates
+++++++++++++++++++++++++++++++++++++++++++

If you explicitly specialize a template nested within several enclosing class templates, you must prefix the declaration with ``template<>`` for every enclosing class template you specialize. You may leave some enclosing class templates unspecialized; however, you
cannot explicitly specialize a class template unless its enclosing class templates are also explicitly specialized. The following example demonstrates explicit specialization of nested member templates:

.. code-block:: cpp

    #include <iostream>
    #include <string>
    
    using namespace std;
    
    template<class T> class X {
    
       public:
    
         template<class U> class Y {
    
            public:
               template<class V> void f(U,V);
               void g(U);
         };
    };
    
    template<class T> template<class U> template<class V>
      void X<T>::Y<U>::f(U, V) 
      { 
        cout << "Primary method definition:\n\n\t'template<class T> template<class U> template<class V> void X<T>::Y<U>::f(U, V) {//...}' \n\n"; 
      }
    
    template<class T> template<class U>
      void X<T>::Y<U>::g(U) 
      {
        cout << "Primary method definition:\n\n\t'template<class T> template<class U> void X<T>::Y<U>::g(U)'\n\n"; 
      }
       
    template<> template<>
      void X<int>::Y<int>::g(int) 
      {
        cout << "Explicit specialization of void X<T>::Y<U>::g(U):\n\n\t'template<> void X<int>::Y<int>::g(int)'\n\n";
      }
    
    template<> template<> template<class V>
      void X<int>::Y<int>::f(int, V)
      {
        cout << "Explicit specialization of void X<T>::Y<U>::f(U, V):\n\n\t'template<> template<class V> void X<int>::Y<int>::f(int, V)'\n\n";
      }
    
    template<> template<> template<>
      void X<int>::Y<int>::f<int>(int, int)
      {
        cout << "Explicit specialization of void X<T>::Y<U>::f(U, V):\n\n\t'template<> template<> template<> void X<int>::Y<int>::f<int>(int, int)'\n\n"; 
      }
       
    // template<> template<class U> template<class V>
    //    void X<char>::Y<U>::f(U, V) { cout << "Template 6" << "\n\n"; }
    
    // template<class T> template<>
    //    void X<T>::Y<float>::g(float) { cout << "Template 7" << "\n\n"; }
    
    int main() 
    {
      X<int>::Y<int> a;
    
      X<char>::Y<char> b;
    
      a.f(1, 2);
    
      a.f(3, 'x');
    
      a.g(3);
    
      b.f('x', 'y');
    
      b.g('z');
    
      return 0; 
    }
    
See the output of the above program:

::
   
    Explicit specialization of void X<T>::Y<U>::f(U, V):
    
            'template<> template<> template<> void X<int>::Y<int>::f<int>(int, int)'
    
    Explicit specialization of void X<T>::Y<U>::f(U, V):
    
            'template<> template<class V> void X<int>::Y<int>::f(int, V)'
    
    Explicit specialization of void X<T>::Y<U>::g(U):
    
            'template<> void X<int>::Y<int>::g(int)'
    
    Primary method definition:
    
            'template<class T> template<class U> template<class V> void X<T>::Y<U>::f(U, V) {//...}' 
    
    Primary method definition:
    
            'template<class T> template<class U> void X<T>::Y<U>::g(U)'

.. todo:: correct text below adding 'Template #'.
    
The compiler would not allow the template specialization definition that would output "Template 6" because it is attempting to specialize a member (function f()) without specialization of its containing class (Y).
The compiler would not allow the template specialization definition that would output "Template 7" because the enclosing class of class Y (which is class X) is not explicitly specialized.

A friend declaration cannot declare an explicit specialization

Partial Template Specialization and Variadic Class Templates
------------------------------------------------------------

A variadic template can take an arbitrary number of template arguments of any type.  Variadic class templates often use partial template specializations, and examples of these can be found within the code blocks where :ref:`variadic-class-templates` are described and discussed.

Links to Articles on Template specialization and partial specialization
-----------------------------------------------------------------------

* https://en.cppreference.com/w/cpp/language/partial_specialization 
* https://en.cppreference.com/w/cpp/language/template_specialization
* https://www.ibm.com/support/knowledgecenter/en/SSLTBW_2.3.0/com.ibm.zos.v2r3.cbclx01/template_specialization.htm
