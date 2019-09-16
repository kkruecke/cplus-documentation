.. include:: <isopub.txt>
.. include:: <isonum.txt>

Rvalue References and Lvalue References in C++
==============================================

Helpful Articles on Understanding Rvalue References, Move Semantics and Forwarding References
---------------------------------------------------------------------------------------------

* `Understanding lvalues and rvalues in C and C++ <https://eli.thegreenplace.net/2011/12/15/understanding-lvalues-and-rvalues-in-c-and-c>`_
* `Thomas Becker's article C++ rvalue Reference Explained <http://thbecker.net/articles/rvalue_references/section_07.html>`_.

The rvalues references and forwarding references introduced in C++11 provide the foundation for move semantics, a way of efficiently creating new object from the resouces of objects about to die. We will first look at rvlaue reference, followed by forwarding references,
and finally move semantics.

.. todo: See https://www.internalpointers.com/post/c-rvalue-references-and-move-semantics-beginners

.. _rvalue-reference:
        
Rvalue References and Their Role
--------------------------------
    
An lvalue is object that has a name (identity), whose address can be taken, but that cannot be moved from. Examples of lvalues:

.. code-block:: cpp

    int i; // lvalue
    int j = 8; // lvalue assigned rvalue

    int& f2(); // f2() returns an lvalue

If an object is not an lvalue, it is an rvalue. See `A Brief Introduction to Rvalue References <http://www.artima.com/cppsource/rvalue.html>`_. An rvalue is a temporary object whose lifetime does not extend past the current line\ |apos|\ s semicolon\. You cannot take the address of an
rvalue. Below are some examples of rvalue and lvalues:

.. code-block:: cpp

    int i; // lvalue
    int j = 8; // lvalue assigned rvalue

    int f1(); // f1() returns an rvalue. We can't do &f1().
    int& f2(); // f2() returns an lvalue
    int&& f3(); // f3() returns an rvalue.

    class X { //...};

    X x1; // lvalue declared
    X f4(); // returns rvalue
    X& f5(); // returns lvalue

An rvalue reference is declared using ``&&``: 

.. code-block:: cpp

    int&& j = 8;
    int&& f3(); // f3() returns an rvalue
    int&& k = f3();
    int v = 9;
    int&& l = v; // error: cannot bind rvlue reference l to lvalue v.

The rvalue reference j above is not really of any value. While we can change the value of a literal using this trick
 
.. code-block:: cpp

    int&& j = 8;
    j = 9;
    cout << j;  // prints: 9

the temporay gets deleted once j goes out of scope, and this technique has no wide applicability. When the compiler see an rvalue, it thinks, "oh, this is an rvalue, let me see if the class method being invoked takes an rvalue reference, so I can
invoke it." 

.. code-block:: cpp

    #include <iostream>
    #include <utility>
     
    void f(int& x) {
        std::cout << "lvalue reference overload f(" << x << ")\n";
    }
     
    void f(const int& x) {
        std::cout << "lvalue reference to const overload f(" << x << ")\n";
    }
     
    void f(int&& x) {
        std::cout << "rvalue reference overload f(" << x << ")\n";
    }
     
    int main() {
        int i = 1;
        const int ci = 2;
        f(i);  // calls f(int&)
        f(ci); // calls f(const int&)
        f(3);  // calls f(int&&)
               // would call f(const int&) if f(int&&) overload wasn't provided
        f(std::move(i)); // calls f(int&&)
     
        // rvalue reference variables are lvalues when used in expressions
        int&& x = 1;
        f(x);            // calls f(int& x)
        f(std::move(x)); // calls f(int&& x)
    }

Note: *rvalue* reference variables are *lvalues* when used in expressions. To see this, consider parameter x in ``void f(SomeClass&& x)`` below 

.. code-block:: cpp
   
   void f(SomeClass&& x)
   {
       ...
       SomeClass *p = &x; // note: we can take the address of x.
       ...
   }

   f(SomeClass()); 
   
The parameter x is a reference to an rvalue that refers to a temporary SomeClass instantiated at the point f is invoked. But within the body of the  function f, *x* is an lvalue because it has a name. x refers to a object about to die, 
but x itself is not about to die. It exists within the execution lifetime of body of f. This fact will have important implications later when move constructors and move assignment operators are introduced.
