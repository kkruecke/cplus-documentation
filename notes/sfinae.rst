SFINAE and enable_if<>
======================

See:

* `Mike Shaharm: Template SFINAE & type-traits (is very, very clear) <https://shaharmike.com/cpp/sfinae/>`_
* `SFINAE and enable_if <https://eli.thegreenplace.net/2014/sfinae-and-enable_if/>`_
* `Modern C++, Item 27 by Scott Meyers`
* `Filipek: Notes on C++ SFINAE <https://www.bfilipek.com/2016/02/notes-on-c-sfinae.html>`_  
* `Filipek: SFINAE Followup <https://www.bfilipek.com/2016/02/sfinae-followup.html>`_
* `Wikipedia SFNIAE Article <https://en.wikipedia.org/wiki/Substitution_failure_is_not_an_error>`_
* `Microsoft enable_if Article has Several Examples <https://docs.microsoft.com/en-us/cpp/standard-library/enable-if-class?view=vs-2019>`_

Type alias, alias template
--------------------------

The keyword ``using`` is a another way of creating a **typedef**, an alias of a type.

.. code-block:: cpp

   template<typename T> class Vector {
       
     public:
       using value_type = T; // Same as: typedef T value_type; 
       //...
   };
   
   Vector<int> v;
   Vector<int>::value_type x = 10; // x is of type int.  
   
``using value_type = T;`` is equivalent to ``typedef T  value_type;``. There is also a form of **using** for use with templates. It can help to create, for example, more readable templates.

.. code-block:: cpp

    template<class T> struct Alloc { 
          //...
    };

    template<class T> using Vec = vector<T, Alloc<T>>; // type-id is vector<T, Alloc<T>>

    Vec<int> v; // Vec<int> is the same as vector<int, Alloc<int>>
       
``Vec`` is an alias template, and ``Vec<int>`` results in substituting **int** for T in ``template<typename T> vector<T, Alloc<T>>``. Thus, ``Vec<int>`` is just an alias for ``vector<T, Alloc<T>>``.  

.. todo:: Finish later.
