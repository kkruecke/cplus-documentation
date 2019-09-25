.. include:: <isopub.txt>
.. include:: <isonum.txt>

.. _perfect-forwarding-label: 

Forwarding References and Perfect Forwarding
============================================

Important Article that Explains Forwarding References and Perfect Forwarding
----------------------------------------------------------------------------

* `Perfect Forwarding by Stroutrup, Sutter and Dos Reis <http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2014/n4164.pdf>`_.

Forwarding References
---------------------

The same notation used for rvalue references, the double ampersand *&&*, is also used to denote a forwarding reference. A **forwarding reference** is a template function parameter of type ``T&&`` such as

.. code-block:: cpp

   template<typename T> void f(T&& t);

While ``T&&`` looks just like an rvalue reference (:ref:`rvalue-reference`), when ``&&`` is used for a function template parameter, it is called a **forwarding refernence**, and unlike an rvalue reference, a forwarding reference can bind to
both rvalues and lvalues, to *const* and *non-const*, to *volatile*, to everything. Its purpose is to support *argument forwarding*, to allow you to pass the argument on unchanged to other function(s). An example of this
use of *argument forwarding* is the ``emplace`` method of many STL containers. ``emplace (Args&&... args)`` allows the new container element to be constructed using *placement new* avoiding unnecessary copy or move operations. 

Forwarding reference take advantage of the new **C++11** reference collapsing rules. In **C++11**, unlike previous versions, you can syntactically have a reference to a reference, and the following reference collapsing rules apply:

* T& & becomes T&
* T& && becomes T&
* T&& & becomes T&
* T&& && becomes T&&

Except in one case, of ``T&& &&``, the final result of reference collapsing is always ``T&``.

The Purpose of Forwarding References
------------------------------------

Unlike an rvalue reference, a forwarding reference ``T&&`` can bind to both rvalues and lvalues. It can bind to both *const* and *non-const* objects. It can bind to *mutable* and *volitale*. It can bind to everything. When a lvalue, say, of
type X is passed to a template function with a *forwarding reference* ``T&&``, then ``T`` becomes ``X&``, and therefore ``T&&`` becomes ``X& &&``, which after applying the reference collapsing rules becomes simply ``X&``. On the other hand, when an rvalue
of type X is passed, ``T`` becomes ``X``, and ``T&&`` is simply ``X&&``.

Thus an lvalue of type X binds as ``X&`` and an rvalue of type X binds as ``X&&``. We can see this in the code below:

.. code-block:: cpp
   
    #include <vector>
    template<class T> struct state_type {
     using type = T;
     static void describe()
     {
        cout << "In non-specialization of struct state_type<T>" << endl;
     } 
    };
    
    template<class T> struct state_type<T&> {
     using type = T;
     static void describe()
     {
       cout << "In partial template specialization of struct state_type<T&>" << endl;
     }
    };
    
    template<class T> struct state_type<T&&> {
     using type = T;
     static void describe()
     {
       cout << "In partial template specialization of struct state_type<T&&>" << endl;
     }
    };
    
    template<class ARG> void f(ARG&& arg)
    {
       state_type<ARG>::describe();
    }
     
    using namespace std;
    vector<int> v{1, 2, 3, 4};
    f(v);
    f(vector<int>{5, 6, 7, 8});
    f(move(v));

This will result in output of::

    In partial template specialization of struct state_type<T&>
    In non-specialization of struct state_type<T>
    In non-specialization of struct state_type<T>

For the lvalue v in ``f(v);``, ``ARG`` resolves to ``vector<int>&``, and the instantiation of ``f()`` is

.. code-block:: cpp 

    void f(vector<int>& && arg)
    {
       state_type<vector<int&>::describe();
    }
    
which, after applying reference collapsing rules for references, becomes

.. code-block:: cpp 

    void f(vector<int>& arg)
    {
       state_type<vector<int&>::describe();
    }
 
So we see *arg* binds as an lvalue reference. In the case of ``f(vector<int>{5, 6, 7, 8});``, ``ARG`` resolves to ``vector<int>``, and the instantiation of ``f`` looks like this: 

.. code-block:: cpp 

    void f(vector<int>&& arg)
    {
       state_type<vector<int>>::describe();
    }

In this case ``arg`` binds as a rvalue reference. We can use these binding rules for function templates as the first step in writing a template function, like a factory method, that perfectly forwards its parameters leaving the paramters type intact. 

Now take this factory method:

.. code-block:: cpp

    class A { // trivial example

       std::string str;

      public:
        A(const std::string& lhs) : str(lhs) // copy constructor
        {
          cout << " A::A(std::string& lhs) invoked." << endl;
        }
    
        A(std::string&& lhs) // move constructor
        {
          cout << " A::A(std::string&& lhs) invoked." << endl;
        }
    };

    template<class T, class ARG> std::shared_ptr<T> factory(ARG&& arg)
    {
       state_type<ARG>::describe();
    
       return std::shared_ptr<T>{std::make_new<T>(arg)};  // fails to invoke A(string&&) when arg is an rvalue.
    }

Note the output of the code below, where first an lvalue is passed to ``factory<T>(ARG&& arg)`` and then an rvalue: 

.. code-block:: cpp

    string lvaluestr{"lvaluestr"};

    shared_ptr<A> ptr1 { factory<A>(lvaluestr) };

    shared_ptr<A> ptr2 { factory<A>(string{"rvaluestr"}) };

The output is::    

    In partial template specialization of struct state_type<T&>
     A::A(std::string& lhs) invoked.
    In non-specialization of struct state_type<T>
     A::A(std::string& lhs) invoked.
 
``factory<T>(ARG&& arg)`` correctly forwarded the lvalue reference, but not the rvalue reference. Instead the rvalue reference got passed as lvalue references. Why did ``shared_ptr<A> ptr2 { factory<A>(string{"rvaluestr"}) };``
fail in invoking ``A::A(A&&)``?

The reason is, ``arg`` is not an rvalue within the body of factory\ |ndash|\ even though the type of ``arg`` is rvalue reference to *std::string*! Remember an rvalue reference parameter, since it has a name, is an lvalue. We need to remove the
name with a cast:

.. code-block:: cpp
 
    template<class T, class ARG> std::shared_ptr<T> factory(ARG&& arg)
    {
       state_type<ARG>::describe();
    
       return std::shared_ptr<T>{ new T( static_cast<ARG&&>(arg) ) };  // static_cast<ARG&&>(arg) returns a nameless parameter.
    }

Now when ``lvaluestr`` is passed, ``ARG`` becomes ``string&`` and so ``ARG&&`` becomes ``string& &&``, which, after applying the reference collapsing rules, becomes simply ``string&``, and ``static_cast<string&>(arg)`` is still an lvalue. When an
rvalue is passed, however, the lvalue ``arg`` parameter is cast to a nameless rvalue. 

The standard library provides ``forward<T>(std::remove_reference<T>::type&)`` to do this *static_cast*, and it looks like this:

.. code-block:: cpp

    template<class T>
    T&& forward(typename remove_reference<T>::type& a) noexcept
    {
      return static_cast<T&&>(a);
    } 

If you use just ``T&`` instead of ``remove_reference<T>::type&`` in the defintion of ``std::forward``, perfect forwarding still works just fine. However, as Thomas Becker `explains <http://thbecker.net/articles/rvalue_references/section_08.html>`_: 
"it works fine only as long as we explicitly specify ARG as the template argument of ``std::forward``, ``std::forward<ARG>``. The purpose of the ``remove_reference`` in the definition of std::forward is to force us to do so." If we don't explicitly
supply the template argument when invoking ``forward()``, this gcc compile error results:

.. code-block:: cpp

    template<class _Tp> void f(_Tp&& t)
    {
        cout << "t = " << forward(t);
    }
    f(10);

results in::

    /usr/include/c++/7/bits/move.h:73:5: note: candidate: template<class _Tp> constexpr _Tp&& std::forward(typename std::remove_reference<_From>::type&)
         forward(typename std::remove_reference<_Tp>::type& __t) noexcept
         ^~~~~~~
    /usr/include/c++/7/bits/move.h:73:5: note:   template argument deduction/substitution failed:
    main.cpp:74:30: note:   couldn't deduce template parameter ‘_Tp’
         cout << "t = " << forward(t);

Returning to our original example

.. code-block:: cpp

    template<typename _Tp>
      constexpr _Tp&&
    forward(typename std::remove_reference<_Tp>::type& __t) noexcept
    {
      return static_cast<_Tp&&>(__t); 
    }

we now use ``std::forward`` in our factory() function: 

.. code-block:: cpp

    string lvaluestr{"lvaluestr"};

    shared_ptr<A> ptr1 { factory<A>(lvaluestr) };

    shared_ptr<A> ptr2 { factory<A>(string{"rvaluestr"}) };

    template<class T, class ARG> std::shared_ptr<T> factory(ARG&& arg)
    {
       state_type<ARG>::describe();

       return std::shared_ptr<T>{ new T( std::forward<T>(arg) ) };  // forward returns a nameless parameter.
    }

The output now is::

    In partial template specialization of struct state_type<T&>
     A::A(std::string& lhs) invoked.
    In non-specialization of struct state_type<T>
     A::A(std::string&& lhs) invoked.

When ``factory<A>(lvaluestr)`` is called, again, ``ARG`` resolves to ``string&`` and applying reference collapsing, we have this instantiation of factory: 

.. code-block:: cpp

    std::shared_ptr<A> factory(string& arg)
    {
       return std::shared_ptr<A>{ new A( std::forward<T>(arg) ) }; 
    }

For the accompanying forward instantiation, the partial template specialization for lvalue references is applied and ``std::remove_reference<string&>::type&`` resolves to ``string&`` and so ``forward()`` gets instantiated as:

.. code-block:: cpp

   constexpr string& forward(string& __t) noexcept 
   {
      // static_cast<string& &&> collpases to static_cast<string&> below:
      return static_cast<string&>(__t); 
   }

So the complete instantiation of ``factory<A>(lvaluestr)`` is

.. code-block:: cpp

    std::shared_ptr<A> factory(string& arg)
    {
       return std::shared_ptr<A>{ new A( static_cast<string&>(arg) ) }; 
    }

which results in the ``A::A(std::string&)`` being invoked! 

When ``factory<A>(string{"rvaluestr})`` is called, again, ``ARG`` resolves to ``string``, and  we have this instantiations of factory:

.. code-block:: cpp

    std::shared_ptr<A> factory(string&& arg)
    {
       return std::shared_ptr<A>{ new A( std::forward(arg) ) }; 
    }

and the accompanying instantiation of forward:

.. code-block:: cpp
 
    // remove_reference<string>::type& resolved to string&
    constexpr string&& forward(string& __t) noexcept 
    {
       return static_cast<string&&>(__t); 
    }

So finally ``factory<A>(string{"rvaluestr})`` resolves to: 

.. code-block:: cpp

    std::shared_ptr<A> factory(string&& arg)
    {
       return std::shared_ptr<A>{ new A( static_cast<string&&>(arg) ) }; 
    }

which will cause the ``A::A(string&&)`` constructor will be invoked!

Application of Perfect Forwarding
---------------------------------

Below ``temple<calss T> class Vector`` has a new template member function ``emplace_back`` that takes variadic `forwarding references`_.

.. code-block:: cpp

    template<class T> Vector { 
      // snip...(as above) 
      public:
      // snip...(as above) 
        template<class... ARGS> void emplace_back(ARGS&& ... args);
    };

    template<class T> template<class... ARGS> void Vector<T>::emplace_back(ARGS&& ... args)
    {
       if (current == size) { // If new value won't fit...
          
          grow();           // ...grow the vector
       }

       new(p + current) T{std::forward<ARGS>(args)...}; // Use placement new to construct the object in existing memory.
       
       current++; 
    }

    class Employee {

         std::string name
         int age;
         int salary;
      public:
         Employee() {}
         Employee(const std::string& _name, int _age, int _salary) :\
                 name{_name}, age{_age}, salary{_salary} {}
         
         Employee(Employee&& e) : name{std::move(e.name)}, age{e.age}, salary{e.salary} {}
         // snip...
    }; 

    Vector<Employee> v;

    v.push_back(Employee{"John Doe", 15, 0});
    v.emplace_back("Bob Smith", 45, 80000);

``emplace_back()`` creates the new vector element in-place, in the vector's already-allocated memory, using the forwarded parameters. This eliminates the creation and moving of a temporary object into the vector.

Overloading involving both rvalues and forwarding references
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

What happens when a function template is overloaded with two versions: one taking an lvalue reference and the other a forwarding reference? For example:

.. code-block:: cpp

    template<typename T> void g(T& param) noexcept
    {
      cout << "In g(T& param)" << endl;
    }
    
    template<typename T> void g(T&& param) noexcept
    {
      cout << "In g(T&& param)" << endl;
    }
    
    int x = 20;
    g(11);
    g(x);

In this case, the lvalue reference version always wins, so the output would be::

    In g(T&& param)
    In g(T& param)
 
However, if we remove the overload on the lvalue reference
See these links for move above forwarding references:

.. code-block:: cpp

    template<typename T> void g(T&& param) noexcept
    {
      cout << "In g(T&& param)" << endl;
    }
    
    int x = 20;
    g(11);
    g(x);

Then the output is::

    In g(T&& param)
    In g(T&& param)

as one would expect.

auto&&
------

``auto&&`` used in range-base for loops

.. code-block:: cpp

    
    for(auto&& x : c) { // x is a forwarding reference
      //...
    }

is also a forwarding reference. It's purpose is support the abstraction of perfect forwarding of x to other functions within the *for loop*.

Further articles on forwarding references:
------------------------------------------

#. `Modern C++ Features -- in place construction <https://arne-mertz.de/2016/02/modern-c-features-in-place-construction/>`_
#. https://www.youtube.com/watch?v=ECoLo17nG5c
#. http://eli.thegreenplace.net/2014/perfect-forwarding-and-universal-references-in-c/

Conclusion
----------

When an lvalue is passed to ``std::forward<T>(x)``, it returns a nameless lvalue; however, when an rvalue is passed, a nameless rvalue is returned. ``std::forward<T>()`` thus perfectly fowards template function parameters that are specified as
forwarding parameters, resulting in the correct method always being invoked.

.. Say, we want to add a method that would  <See below for insert vs emplace differences>
   http://stackoverflow.com/questions/14788261/c-stdvector-emplace-vs-insert
   http://stackoverflow.com/questions/17172080/insert-vs-emplace-vs-operator-in-c-map  
