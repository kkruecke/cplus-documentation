.. include:: <isopub.txt>
.. include:: <isonum.txt>

Lvalues and Rvalues
===================

On page 165 of `The C++ Programming Language 4th Edition <https://smile.amazon.com/Programming-Language-hardcover-4th/dp/0321958322/ref=sr_1_fkmrnull_1?crid=47A4W3MV3W0Y&keywords=the+c%2B%2B+programming+language+hardcover+4th+edition&qid=1553447852&s=gateway&sprefix=the+c%2B%2B+prog%2Caps%2C206&sr=8-1-fkmrnull>`_, Stroustrup explains the meaning of the term *lvalue*:

    *lvalue* orignally meant an expression that can be on the left-hand side of an assignment. However, this is obviously not true of a *const* object. Note also, that object in this context refers to the low-level notion of "something
    in memory" and not to the notion of class object.

On page 166, he introduces the term *rvalue* and elaborate on the differences between them:
 
    An *object* is a contiguous region of storage of storage; an *lvalue* is an expression that refers to an object....To complement the notion of an *lvalue*, we have the notion of an *rvalue* such as a temporary (e.g. the value returned from
    a function).

    There are two properties that for an object when it comes to addressing, copying and moving:

    * *Has Identity*: The program has the name of, address of, or reference to the object so that it is possible to determine if two objects are the same whether the value has changed, etc.
    * *Movable*: the object may be moved from (i.e. we allowed to move its value to another location and leave the object in a valid but unspecified state, rather than copying).

    It turns out that three of the four possibilities of these two properites are neede to precisely describe the C++ language rules (we have not need for object that do not have identity and cannot be moved). Using "**m**
    for movable" and "**i** for has identity", we can represent this classification of expressions graphically:

.. figure:: ../images/lvalue-rvalue.jpg
   :alt: value categories
   :align: center 
   :scale: 100 %
   :figclass: custom-figure

   **Figure:  lvalues and rvalue and value categories** 

..

    So, a classical lvalue is something that has identity and cannot be moved (because we could examine it after the move), and a classical rvalue is anything that we are allowed to move from. The other alternatives 
    are *prvalue* ("pure rvalue"), *glvalue* ("generalized lvalue"), and *xvalue* ("x" for "extraordinary"). For example:

.. code-block:: cpp

    void f(vector<stinrg>& vs)
    {
         vector<string>& v2 = std::move(vs);
    }

..

    Here ``std::move(vs)`` is an xvalue: it clearly has idenity (we can refer to it as ``vs``), but we have explicitly given permissio for it to be mmoved from by calling ``std::move()``.
    
    For praictical programming, thinking in terms of rvalue and lvalue is usally sufficient. Note that every expression is either an lvalue or an rvalue, but not both.

Here are links that discuss value categories.

* `cppreference: Value categories <https://en.cppreference.com/w/cpp/language/value_category>`_.
* `CppCon 2015: John Lakos “Value Semantics: It ain't about the syntax!, Part I" <https://www.youtube.com/watch?v=W3xI1HJUy7Q>`_
* `CppCon 2015: John Lakos “Value Semantics: It ain't about the syntax!, Part II"  <https://www.youtube.com/watch?v=0EvSxHxFknM>`_.
