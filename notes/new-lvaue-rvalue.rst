.. include:: <isopub.txt>
.. include:: <isonum.txt>

.. _rvalue-reference:

Rvalue References and Lvalue References in C++
==============================================

* `A Brief Introduction to Rvalue References <http://www.artima.com/cppsource/rvalue.html>`_.
* `Core C++ 2019 : Dan Saks : Understanding Lvalues and Rvalues video of talk <https://www.youtube.com/watch?v=mK0r21-djk8>`_
* `Thomas Becker's article C++ rvalue Reference Explained <http://thbecker.net/articles/rvalue_references/section_07.html>`_.

.. todo: See https://www.internalpointers.com/post/c-rvalue-references-and-move-semantics-beginners

Introduction and Definitions
----------------------------

**lvalues** and **rvalues** are properties of expressions. They are attributes of subexpressions in a larger expression. Prior to C++, in C, an lvalue was a expression that referred to an object, a region of data storage that may have a value. An rvalue is any expression that is not a lavlue. For example, in the
code below

.. code-block:: cpp

    int n; // lvalue

    n = 1;

``n`` can be assigned because it is an lvalue expression; it is a location of memory that can hold a value. Thus, in C, an lvalue was an expression valid on the left-hand side of an assignment. Note, so say thant ``n`` in the assignment statement is an lvalue expression to describe a compile-time property not a
runtime property. An lvalue expression is an compile-type property of the expression.

A thorough introduction to lvalues and rvalues is Dan Saks lecture at Core C++ 2019 on `Understanding Lvalues and Rvalues <https://www.youtube.com/watch?v=mK0r21-djk8>`_. 

References and lvalues and rvalues
-----------------------------------

References are implemented using pointers. References act like a const pointer that's dereferenced automatically. And like a dereferecnes pointer ``*p``, which yields an lvalue, a reference also yields an lvalue.
The concepts of lvalues and rvalues help explain C++ reference types, and the real strength of references comes out in operator overloading. 

.. code-block:: cpp

    class complex {
        complex operator+(const complex& param);
        //....
    }; 

The binary + operator takes a "reference to const" parameter. Such a parameter will bind to a argument that is either const of non-const. If we had written ``complex operator+(complex& param)``, then param would only bind to non-cost lvalue arguments.
In general a "reference to const" of type T binds to both const and non-const lvalues expressions involving objects of type T, and to rvalues like, say x, if x is convertible to T. For example

.. code-block:: cpp

     int const &intref = 3; 
     int double& dref = intref;

Here 3 is obviously "convertable" to an int because it is an int, and so the compiler creates a temporary to hold 3 and then binds ``intref`` to the temporary. The same thinking applies to the second line: ``intref`` is an lvalue that is convertable to a double,
so the compiler creates a temporary to store the convert double value and then bind ``dref`` to it. 

Only const references can bind to rvalues, as ``intref`` does to the rvalue 3 above. If ``intref`` were a non-const reference to a int, then it could not bind to 3.

The reason C++ bothers to create the temporaries is so that pass by value and pass by reference look the same in all cases. For example, the pass-by-value function ``f`` and the pass-by-const-reference function ``fref`` below behave the same way.

.. code-block:: cpp

    long double x;
    void f(long double ld);
    //...
    f(x); // passes a copy of x
    f(1); // passes a copy of 1 converted to a double.
    void fref(const long double& ld);
    fref(x); // ld is const reference to the double x.  
    fref(1); // passes a reference to a temporary, a copy of 1, converted to a double.

rvalue references
-----------------

Very basic code examples for rvalues. 

.. code-block:: cpp

Note: Rvalue references are implemented by allocating storage for a temporary object that is an rvalue expression and then binding an rvalue reference to it.

The purpose of rvalue reference is the help support move efficient move constructors and move assignment operators.

value categories
----------------

What were called references in C++98 are in C++11 and above called lvalue references. Rvalue references where added in C++11, so more efficient resouce-stealing move constructors and move assignment operators could be supported. When the compiler detects a value that is about to die or expire, an rvalue, an rvalue reference can be bound to it and
the resources of the expiring object can be stolen.

.. todo:: reference the Stroutrup article about.

xvalue is new to C++11. It represents an eXpiring value. Literals are now termed prvalues. Both are grvalues. 
