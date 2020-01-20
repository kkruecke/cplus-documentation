
Thus C++11 has three main value categories: lvalue, xvalue (or eXpiring value), and prvalue (pure rvalues). Bjarne Stroutrup described their differences this way, on page 166, of **The C++ Programming Language, 4th Edition**:

    There are two properties that matter for for an object when it comes to addressing, copying and moving:
    
    * **Has Identity**: The program has the name of, address of, or reference to the object so that it is possible to determine if two objects are the same, whether the value has changed,
      etc.
    * **Movable**: the object may be moved from (i.e. we are allowed to move its value to another location and leave the object in a valid but unspecified state, rather than copying it).
    
    It turns out that three of the four possibilities of these two properites are needed to precisely describe the C++ language rules (we have no need for object that do not have
    identity and cannot be moved). Using "**m** for movable" and "**i** for has identity", we can represent this classification of expressions graphically:
    
.. figure:: ../images/value-categories-bs.jpg
   :alt: value categories
   :align: center 
   :scale: 100 %
   :figclass: custom-figure

   **Figure: value categories** 

    So, a classical lvalue is something that has identity and cannot be moved (because we could examine it after the move), and a classical rvalue is anything that we are allowed to move
    from. 

.. todo:: Inclue BS's code example on page 166 

..

Another good explanation of C++11 value categories is from `cppreference's article <https://en.cppreference.com/w/cpp/language/value_category>`_, which offers this explanation:
    
    With the introduction of move semantics in C++11, value categories were redefined to characterize two independent properties of expressions:
    
    * **has identity**: it's possible to determine whether the expression refers to the same entity as another expression, such as by comparing addresses of the objects or the functions they identify (obtained directly or indirectly)
    * **can be moved from**: move constructor, move assignment operator, or another function overload that implements move semantics can bind to the expression. 
    
    This lead to the following value categories for expressions:
    
    **have identity** and **cannot be moved from** are called lvalue expressions;
    **have identity** and **can be moved from** are called xvalue expressions;
    **do not have identity** and **can be moved from** are called prvalue ("pure rvalue") expressions;
    **do not have identity** and **cannot be moved from** are not used. 
    
    The expressions that have identity are called "glvalue expressions" (glvalue stands for "generalized lvalue"). Both lvalues and xvalues are glvalue expressions.
    
    The expressions that can be moved from are called "rvalue expressions". Both prvalues and xvalues are rvalue expressions.
