.. include <isopub.txt>

.. |nbsp| unicode:: 0xA0 
   :trim:


Variadic Templates
==================

Good articles on implementing C++ Variadic Templates
----------------------------------------------------

* `C++11 - New features - Variadic templates <http://www.cplusplus.com/articles/EhvU7k9E/>`_
* `Wkipedia Article on Variadic Template <https://en.wikipedia.org/wiki/Variadic_template>`_
* `An introduction to C++'s variadic templates: a thread-safe multi-type map <https://jguegant.github.io/blogs/tech/thread-safe-multi-type-map.html>`_

Variadic Class Template
-----------------------

A variadic class template can be instantiated with a varying number of template arguments. A good use case for such a recursive data structures is a tuple 
that can hold a mixed set of varying types:

.. code-block:: cpp

    template<class... Types> struct tuple; // Forward declaration 

    // Partial template specialization for the case when no template arguments are supplied 
    template<class... Types> struct tuple<> {}; 

    // public inheritance is the default for structs.
    template <class T, class... Ts> struct tuple<T, Ts...> : tuple<Ts...> { 

        // Invoke immediate base struct template with remaining arguments sans T argument,
        // and construct taiil using t, of type T.
        tuple(T t, Rest... ts) : tuple<Ts...>(ts...), tail(t) {}
        T tail;
    };

    tuple<> t0;           // Types contains no arguments. Same as: tuple t0;
    tuple<int> t1;        // Types contains one argument: int
    tuple<int, float> t2; // Types contains two arguments: int and float
    tuple<0> error;       // error: 0 is not a type

Defining Recursive Data Structures Using Variadic Class Templates
-----------------------------------------------------------------

To better understand variadic class templates, first consider this series of derived structs, where each struct in the hierarchy has a sole data member *tail*

.. code-block:: cpp

    struct bottom {};
    
    struct A : bottom {
    
        A(const string& s) : tail{s}
        {
        }
        string tail;
    };
    
    struct B : A {

	B(double d, const string& s) : A(s), tail{d}
	{
	}
	double tail;
    };
    
    struct C : B {
	C(int i, double d, const string& s) : tail{i}, B(d, s)
        {
        }
	int tail;
    };

To access each tail member of a ``C`` instance, like that below, we would use ``static_cast<class T>()`` as below

.. code-block:: cpp
    
    auto i = 5;
    auto d = 10.5;
    auto string s{"hello world!"}; 

    C c(i, d, s);

    auto x1 = c.tail; // tail is C::tail

    auto x2 = static_cast<B&>(c).tail; // tail is B::tail

    auto x3 = static_cast<A&>(c).tail; // tail is A::tail

The preceeding code is just the sort of use case where variadic templates can be effectively applied. We begin by defining a recursive data structure hierarchy for ``struct tuple<class...Types>``

.. code-block:: cpp

    template<class... Ts> struct tuple; //forward reference

    // Template specializtion for empty list of template arguments, the base struct of the recursive tuple data structure.
    template<> struct tuple<> { 
    
        tuple()
        {
  	    std::cout << "In template<> tuple<>::tuple() constructor, which has NO member tail." << std::endl;
        }
    }; 
    
    // Recall that public inheritance is the default for structs.
    template<class T, class... Ts> struct tuple<T, Ts...> : tuple<Ts...> { 
    
        tuple(T t, Ts... ts) : tuple<Ts...>(ts...), tail(t)
        {
            std::cout << "  In constructor for " <<  __PRETTY_FUNCTION__ << " where tail = " << tail << std::endl;
        }
    
        T tail;
    };
    
An instantiation of, say, ``tuple<double, int, const char*>`` will recursively generate these template instantiations

.. code-block:: cpp

    struct tuple<> { // base of inheritance hierarchy
        tuple()
        {
            std::cout << "In template<> tuple<>::tuple() constructor, which has NO member tail." << std::endl;
        }
    }

    struct tuple<const char *> : tuple<> { // next to bottom level

        tuple(const char *t) : tail(t)
        {
            std::cout << "In constructor for " <<  __PRETTY_FUNCTION__ << " where tail = " << tail << std::endl;
        }

        const char *tail; 
    };

    struct tuple<int, const char *> : struct tuple<const char *> { // next to top level of hierachy
       tuple(int t) : tail(t)
       {
          std::cout << "In constructor for " <<  __PRETTY_FUNCTION__ << " where tail = " << tail << std::endl;
       }
       int tail; 
    };    
    
    struct tuple<double> : struct tuple<int, const char *> { // top of inheritance hierarchy
       tuple(double t) : tail(t)
       {
          std::cout << "In constructor for " <<  __PRETTY_FUNCTION__ << " where tail = " << tail << std::endl;
       }

       double tail; // top level 
    };    

And the instantiated hierarchy above can be seen from its constructor print outs that occurs when we instantiate a tuple like ``tuple<double, int, const char *>`` below: 

.. code-block:: cpp

    tuple<double, int, const char *> t(10, 10.5, "hello world!");

The output looks like this and show the four levels of the struct hierarchy instantiated: 

.. raw:: html
 
    <pre>
    In template<> tuple<>::tuple() constructor, which has NO member tail.
    In constructor for tuple<T, Ts ...>::tuple(T, Ts ...) [with T = const char*; Ts = {}] where tail = hello world!
    In constructor for tuple<T, Ts ...>::tuple(T, Ts ...) [with T = double; Ts = {const char*}] where tail = 10.5
    In constructor for tuple<T, Ts ...>::tuple(T, Ts ...) [with T = int; Ts = {double, const char*}] where tail = 5
   </pre>

Visually the layout of ``tuple<double, int, const char *>`` looks like this:

.. image:: ../images/recursive-tuple-layout.jpg
   :scale: 75 %

We can now instantiate tuples of varying types, but how do we access its elements? How do we retrieve or change, say, the ``int`` value above or that ``const char *``? It boils down to determing where the ``int tail;`` member is in the hierarchy. We know the ``int tail`` member is 
in the next to top level, which is also the third level from the bottom. To retrieve the corresponding ``int tail`` member, we use the variadic template function called ``get<size_t, tuple<Ts ...>``, which has a partial template specialization for ``get<0, tuple<class...Ts>()`` . This partial
template specialization, in turn, contains two critical type definitions:

1. ``using value_type = T&;``                     // Reference to tail's type.
2. ``using base_tuple_type = tuple<T, Rest...>;`` // The type of the tuple instance (of the base

To understand these two type definitions we have to understand another recursive data structure, also defined using variadic class templates, ``tuple_elelment``, whose whole purpose is to provide type information for the ``tuple`` hierachy. Unlike ``tuple``, which contains
a sole ``tail`` data member at each level of its recursive structure, ``tuple_element`` contains no data members. Instead it contains only the two *type definitions* mentioned above, defined by means of a using statement, and these two type definitions only occur, at the
bottom level of the ``tuple_element`` hierarchy. 

To better grasp what is going on, we add print statements to ``tuple_element``'s default constructors. Note: The default constructor is not actually needed, but was added to understand ``tuple_element``'s role:

.. code-block:: cpp

    // tuple_element forward declaration.
    template<std::size_t Index, class _tuple> struct tuple_element;
    
    // recursive data structure tuple_element definition
    template <std::size_t Index, class T, class... Rest>  struct tuple_element<Index, tuple<T, Rest...>> : 
         public tuple_element<Index - 1, tuple<Rest...> > {
    
        tuple_element()
        {
          std::cout << "  In tuple_element<" << Index << ", tuple<T, Rest...>>::tuple(), where there are not type definitions." << std::endl;
        }
    };
    
    // partial template specialization when first parameter is zero: tuple_element<0, tuple<T, Rest...>>.
    template<class T, class... Rest>  struct tuple_element<0, tuple<T, Rest...>>  {
    
      using value_type = T&;                 // Reference to tail's type.
      using base_tuple_type = tuple<T, Rest...>;  // The type of the tuple instance
    
      tuple_element()
      {
          std::cout << "In tuple_element<0, T, Rest...>>::tuple(), where there are these two type definitions:" << std::endl;
          std::cout << "\tusing value_type = T&" << std::endl;
          std::cout << "\tusing base_tuple_type = tuple<T, Rest>" << std::endl;
      }
    };
    
    /*
     * get reference to Index element of tuple
     */
    template<size_t Index, class... Type> inline 
                           typename tuple_element<Index, tuple<Type...>>::value_type get(tuple<Type...>& _tuple)
    {
      std::cout << "In get<" << Index << ">(some_tuple)" << "\n---------" << std::endl;
    
      // Cast _tuple to the base type of the corresponding tuple_element<Index,  tuple<Type...>> recursive struct type. 
      using base_tuple_type = typename tuple_element<Index, tuple<Type...>>::base_tuple_type;
    
      return static_cast<base_tuple_type&>(_tuple).tail;
    }
    
If we instantiate ``element_tupe<3, tuple<double, int, const char*>> te2``, we will see these constructor calls. 

.. raw:: html
 
    <pre>
    In tuple_element<0, T, Rest...>>::tuple(), where there are these two type definitions:
	    using value_type = T&
	    using base_tuple_type = tuple<T, Rest>
      In tuple_element<1, tuple<T, Rest...>>::tuple(), where there are not type definitions.
      In tuple_element<2, tuple<T, Rest...>>::tuple(), where there are not type definitions.
    </pre>

This reflects the actual instantiations of ``template<size_t, tuple<class...Rest>> struct element_tuple`` that occur when, say, ``element_tupe<3, tuple<double, int, const char*>> te2`` is declared: 

.. code-block:: cpp

    struct tuple_element<2, tuple<double, int, const char*>> : struct tuple_element<1, tuple<int, const char*>>   {};
    struct tuple_element<1, tuple<int, const char*>> : struct tuple_element<0, tuple<const char*>>   {};
    struct tuple_element<0, tuple<const char*>>  {
           using value_type = const char *;
           using base_tuple_type = tuple<const char *>;
    }; 
 
As mentioned, only the base struct of the hierarchy has the two type definitions, which is what we see in the output above. Next, let's examine the ouput from ``get<int>(some_instance)``

.. code-block:: cpp

    tuple<int, double, const char *> tup1(5, 10.5, "hello world!");

    get<2>(tup1);

which looks like this

.. raw:: html
 
    <pre>
    In template<> tuple<>::tuple() constructor, which has NO member tail.
      In constructor for tuple<T, Ts ...>::tuple(T, Ts ...) [with T = const char*; Ts = {}] where tail = hello world!
      In constructor for tuple<T, Ts ...>::tuple(T, Ts ...) [with T = double; Ts = {const char*}] where tail = 10.5
      In constructor for tuple<T, Ts ...>::tuple(T, Ts ...) [with T = int; Ts = {double, const char*}] where tail = 5
    In get<2>(some_tuple) doing this cast: static_cast<base_tuple_type&>(_tuple).tail
    </pre>

we see ``get<size_t, ...>`` works by casting its input argument to the base type of the inheritance hierarchy ...FINISH BY WORKING THROUGH THE example code and showing how the tuple_element hierarch actually works. Draw it out on a sheet of paper first to aid a clear
explanation.

.. todo:: Show a better way to inmplement `tupple using C++17 <https://medium.com/@mortificador/implementing-std-tuple-in-c-17-3cc5c6da7277>`_.

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
