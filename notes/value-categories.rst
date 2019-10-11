.. include:: <isopub.txt>
.. include:: <isonum.txt>

Value Categories
================

Very Helpful Articles
---------------------

* `Video: C++ lvalue rvalue xvalue glvalue prvalue <https://www.youtube.com/watch?v=yIOH95oIKbo>`_
* `Building Intuition on value categories <http://marcodiiga.github.io/building-intuition-on-value-categories>`_.
* `The deal with C++14 xvalues <https://blog.jeaye.com/2017/03/19/xvalues/>`_
* `lvalues, rvalues, glvalues, prvalues, xvalues, help! <https://blog.knatten.org/2018/03/09/lvalues-rvalues-glvalues-prvalues-xvalues-help/>`_
* `Stackoverflow Explanation <https://stackoverflow.com/questions/3601602/what-are-rvalues-lvalues-xvalues-glvalues-and-prvalues>`_ 
* `Bjarne Stroustrup Article Introducing Value Categories <http://www.stroustrup.com/terminology.pdf>`_
* `Open-std.org article <http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2010/n3055.pdf>`_

.. * `Lvalues, rvalues and references <https://akrzemi1.wordpress.com/2011/11/09/lvalues-rvalues-and-references>`_.

Introduction
------------

C++11 introduced the concept of an expression's **value category**, which the article `Value Categories <https://en.cppreference.com/w/cpp/language/value_category>`_ at en.cppreference.com explains

    Each C++ expression (an operator with its operands, a literal, a variable name, etc.) is characterized by two independent properties: a type and a value category.
    Each expression has some non-reference type, and each expression belongs to exactly one of the three primary value categories: prvalue, xvalue, and lvalue.

Here are some examples of expressions:

.. code-block:: cpp

    x = 42; // Assignment expression
    
    42; // An expression as well, with no side effects
    
    int a; // A declaration statement (not an expression)
    
    int b = fun(42); // A declaration statement with an expression initializer
                     // fun(42) is an expression

The three primary value categories--prvaue, xvalue and lvalue--are characterized by one or both of two properties: the 'has identity' property and the 'can be moved from' property. If an expression *has identity*, then it is possible to determine
whether the expression refers to the same entity as another expression, such as by comparing addresses of the objects or the function they identify(obtained directly or indirectly). If an expression can be *moved from*, then the move constructor,
the move assignment operator or any function that implements move semantics can bind to the expression.

Any expression that 'has identity' is termed an *glvalue(generalized lvalue)*. An expression that 'can be moved from' is termed an  *rvlaue*. The primary value categories are categorized according to the following taxonomy:

.. figure:: ../images/value-categories.jpg
   :alt: Value Categories
   :align: center
   :scale: 75 %

The big picture:

* A **glvalue** (generalized lvalue) has identity.
* An **rvalue** can be moved from.
* An **lvalue** has identity but cannot be moved from. lvalues (so-called, historically, because lvalues could appear on the left-hand side of an assignment expression) designate a function or an object.
* An **xvalue** (an eXpiring value) has identiy and can be moved from.
* A **prvalue** (pure rvalue) is an rvalue that is not an xvalue.

Point to note: The statement that held ture in C++03, that every expression is either an lvalue or an rvalue, still holds true in C++1, still holds true in C++11. 

What are Examples of lvalue expressions?
----------------------------------------

Examples of lvalues include:

1. the name of a variable, a function or a data member. Even if the variable's type is rvlaue reference, the expression consisting of its name is an lvalue expression.
2. A function call or an overloadws operator expression whose return type is an lvalue reference.
3. ``x = y``, ``x += y``, ``x %= y``, and all other built-in assignement and compound assigment expressions.
4. A cast expression that casts to an lvalue reference: ``static_cast<int&>(x)``.
5. Built-in pre-increment and post-incremant expressions.
6. ``*x``, build-in indirection operator expression.
7. ``x[i]``, built-in subscript expressions, except when x is an array rvalue.

STOPPED VIDEO at 5:00 minutes

.. todo:: Use lvalue examples from cppreference.com

.. todo:: Old stuff below

On page 165 of `The C++ Programming Language 4th Edition <https://smile.amazon.com/Programming-Language-hardcover-4th/dp/0321958322/ref=sr_1_fkmrnull_1?crid=47A4W3MV3W0Y&keywords=the+c%2B%2B+programming+language+hardcover+4th+edition&qid=1553447852&s=gateway&sprefix=the+c%2B%2B+prog%2Caps%2C206&sr=8-1-fkmrnull>`_,
Stroustrup explains what *lvalue* means:

    *lvalue* orignally meant an expression that can be on the left-hand side of an assignment. However, this is obviously not true of a *const* object. Note also, that object in this context refers to the low-level notion of "something
    in memory" and not to the notion of class object.

On page 166, he explains the term *rvalue* and elaborates on the differences between *lvalues* and *rvalues*:
 
    An *object* is a contiguous region of storage of storage; an *lvalue* is an expression that refers to an object....To complement the notion of an *lvalue*, we have the notion of an *rvalue* such as a temporary (e.g. the value returned from
    a function).

    There are two properties that matter for an object when it comes to addressing, copying and moving:

    * *Has Identity*: The program has the name of, address of, or reference to the object so that it is possible to determine if two objects are the same, whether the value has changed, etc.
    * *Movable*: the object may be moved from (i.e. we are allowed to move its value to another location and leave the object in a valid but unspecified state, rather than copying).

    It turns out that three of the four possibilities of these two properites are needed to precisely describe the C++ language rules (we have no need for object that do not have identity and cannot be moved). Using "**m**
    for movable" and "**i** for has identity", we can represent this classification of expressions graphically:

.. figure:: ../images/value-categories.jpg
   :alt: value categories
   :align: center 
   :scale: 100 %
   :figclass: custom-figure

   **Figure: value categories** 
..

    So, a classical lvalue is something that has identity and cannot be moved (because we could examine it after the move), and a classical rvalue is anything that we are allowed to move from. The other alternatives 
    are *prvalue* ("pure rvalue"), *glvalue* ("generalized lvalue"), and *xvalue* ("x" for "extraordinary"). For example:

.. code-block:: cpp

    void f(vector<stinrg>& vs)
    {
         vector<string>& v2 = std::move(vs);
    }

..

    Here ``std::move(vs)`` is an xvalue: it clearly has idenity (we can refer to it as ``vs``), but we have explicitly given permission for it to be moved from by calling ``std::move()``.
    
    For praictical programming, thinking in terms of rvalue and lvalue is usually sufficient. Note that every expression is either an lvalue or an rvalue, but not both.
