Variadic Templates
==================

.. todo:: Read these articles and synthesize their contents with examples that explain what variadic templates are and how they can be used.

Good articles on implementing C++ Variadic Templates
----------------------------------------------------

In addition to variadic function template examples, `Variadic templates in C++ <https://eli.thegreenplace.net/2014/variadic-templates-in-c/>`_ has an example of how ``std::tuple`` is implemented using a variadic data structure. The code for the tuple implementation
given in the article is at https://github.com/eliben/code-for-blog/blob/master/2014/variadic-tuple.cpp.

.. note:: Can the tuple implementation take advantage of C++17 iteration features. See my bookmarks for clues.

* `C++11 - New features - Variadic templates <http://www.cplusplus.com/articles/EhvU7k9E/>`_
* `Wkipedia Article on Variadic Template <https://en.wikipedia.org/wiki/Variadic_template>`_
* `An introduction to C++'s variadic templates: a thread-safe multi-type map <https://jguegant.github.io/blogs/tech/thread-safe-multi-type-map.html>`_

Variadic Class Template
-----------------------

`Parameter pack(since C++11) <https://en.cppreference.com/w/cpp/language/parameter_pack>`_ explains that: A variadic class template can be instantiated with a varying number of template arguments.

.. code-block:: cpp

    template<class ... Types> struct Tuple {};
    Tuple<> t0;           // Types contains no arguments
    Tuple<int> t1;        // Types contains one argument: int
    Tuple<int, float> t2; // Types contains two arguments: int and float
    Tuple<0> error;       // error: 0 is not a type

Defining Recursive Data Structures Using Variadic Class Templates
-----------------------------------------------------------------

.. todo:: Use the tuple implementation by the Eli Berskensky listed first below as a teaching example. Use __PRETTY_FUNCTION__  to layout of tupe<Ts ...>.

If Tuple is defined recursively as 

.. code-block:: cpp

    template <class... Ts> struct Tuple { // <-- This Tuple definition is needed. It will be the final base Tuple of all Tuple<Ts...>'s.
    
        Tuple()
        {
  	    std::cout << "In base Tuple constructor, which has NO member tail." << std::endl;
        }
    }; 
    
    template <class T, class... Ts> struct Tuple<T, Ts...> : Tuple<Ts...> { // public inheritance is the default for structs.
    
    
        Tuple(T t, Ts... ts) : Tuple<Ts...>(ts...), tail(t)
        {
            std::cout << "In constructor of " <<  __PRETTY_FUNCTION__ << " where tail = " << tail << std::endl;
        }
    
        T tail;
    };
    
the generated definition of ``tuple<double, int, const char*>`` will be

.. code-block:: cpp

    struct Tuple<double, int, const char *> : struct Tuple<int, const char *> : struct Tuple<const char *> : Tuple<> {

       double tail; // top level of hierachy
    };    
    
    struct Tuple<int, const char *> : struct Tuple<const char *> : Tuple<> {

       int tail; // next to top level
    };    

    struct Tuple<const char *> : struct Tuple {

       const char *int tail; // next to bottom level 
    };    

    struct Tuple {

        // bottom of hierachy
    };    

The construction of ``tuple<double, int, const char*> tuple(12.2, 43, "big")`` shows these four levels being constructed 

.. raw:: html
 
    <pre>
    In base Tuple constructor, which has NO member tail.
    In constructor of Tuple<T, Ts ...>::Tuple(T, Ts ...) [with T = const char*; Ts = {}] where tail = big
    In constructor of Tuple<T, Ts ...>::Tuple(T, Ts ...) [with T = int; Ts = {const char*}] where tail = 42
    In constructor of Tuple<T, Ts ...>::Tuple(T, Ts ...) [with T = double; Ts = {int, const char*}] where tail = 12.2
   </pre>

This gives a layout of

.. figure:: ../images/recursive-tuple-layout.jpg
   :alt: recursive tuple layout
   :align: left 
   :scale: 75 %
   :figclass: tuple-layout

   **Figure: layout of tuple inheritance hierarchy** 

If we use zero to designate the bottom of the hierachy, then the top level is at level three (of the four levels). 

Now that we can instantiate Tuples of varying types, how to we access the values of its recursive data structure? How do we retrieve or change, say, ``int`` value above or that ``const char *``? The access method is a variadic template function called ``get`` whose template argument is
of type ``int``. ``get<int>()`` works uses a another recursive data structure that parelells the structure of ``tuple<Ts ...>`` to determine the depth in the tuple hierachy, using an integer template parameter. 

* `Variadic Templates in C++ <https://eli.thegreenplace.net/2014/variadic-templates-in-c/>`_.
* `Variadic template data structures <https://riptutorial.com/cplusplus/example/19276/variadic-template-data-structures>`_
* `Tuple implementation via variadic templates <https://voidnish.wordpress.com/2013/07/13/tuple-implementation-via-variadic-templates/>`_ also discusses how to implement tuple using variadic templates.

Variadic Function Template
--------------------------
 
`Parameter pack(since C++11) <https://en.cppreference.com/w/cpp/language/parameter_pack>`_ explains that "A variadic function template can be called with any number of function arguments (the template arguments are deduced through template argument deduction)":

.. code-block:: cpp

    template<class ... Types> void f(Types ... args);
    f();       // OK: args contains no arguments
    f(1);      // OK: args contains one argument: int
    f(2, 1.0); // OK: args contains two arguments: int and double

Further Explanation
-------------------

"In a primary class template, the template parameter pack must be the final parameter in the template parameter list. In a function template, the template parameter pack may appear earlier in the list provided that all following parameters can
be deduced from the function arguments, or have default arguments:"

.. code-block:: cpp

    template<typename... Ts, typename U> struct Invalid; // Error: Ts.. not at the end
     
    template<typename ...Ts, typename U, typename=void>
    void valid(U, Ts...);     // OK: can deduce U
    // void valid(Ts..., U);  // Can't be used: Ts... is a non-deduced context in this position
     
    valid(1.0, 1, 2, 3);      // OK: deduces U as double, Ts as {int,int,int} 

C++17 Offers Limited Iteration Over a Parameter Pack
----------------------------------------------------

In C++ a variadic template function like ``sum`` below required two versions of ``sum`` to be implemented, one taking just one parameter type and the other taking at least two or more parameters types:

.. code-block:: cpp

    template<typename T>
    T sum(T v) 
    {
      return v;
    }
    
    template<typename T, typename... Args>
    T sum(T first, Args... args) 
    {
      return first + adder(args...);
    }
    
    long sum = adder(1, 2, 3, 8, 7);
    
    std::string s1 = "x", s2 = "aa", s3 = "bb", s4 = "yy";
    std::string ssum = adder(s1, s2, s3, s4);

C++17 offers a limited form of iteration over elements of a parameter pack, which allows us to implement ``adder()`` with only one template:        

.. code-block:: cpp

    template<Number... T>int sum(T... v)
    {  
        return (v + ... + 0);     // add all elements of v starting with 0
    }
