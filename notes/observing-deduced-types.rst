.. include:: <isopub.txt>

.. role:: cpp(code)
   :language: cpp

Observing Deduced Types
=======================

Scott Meyers lecture on `Type Deduction and Why You Care <https://www.youtube.com/watch?v=wQxj20X-tIU>`_ (at around minute 43) explains a technique to force compilers to display the types it deduces when a function template is instantiated or when an auto variable
is defined. Here is the code 

.. code-block:: cpp

    template<typename T> TD;   // Declaration for TD. TD == "Type Displayer"

    template<typename T> void f(T& param)  // A template w/types of interest
    {
      TD<T> tType;                      // cause T to be shown
      TD<decltype(param)>  paramType;   // ditto for param's type     
    }

that forces two compile errors that will display both the deduced type for  ``T`` and ``param``.  Examples:

.. code-block:: cpp

   int x = 27;  // x is an int
   f(x);  // main.cpp line 67

Using g++ version 7.2.0, the output is::

    main.cpp: In instantiation of ‘void f(T&) [with T = int]’:
    main.cpp:67:6:   required from here
    main.cpp:53:9: error: ‘TD<int> tType’ has incomplete type
       TD<T> tType;               // cause T to be shown
             ^~~~~
    main.cpp:54:24: error: ‘TD<int&> paramType’ has incomplete type
       TD<decltype(param)>  paramType; // ditto for param's type
                            ^~~~~~~~~

.. code-block:: cpp

   int& rx = x;  // rx is a reference to an int
   f(rx); // main.cpp line 70

Using g++ version 7.2.0, the output is::

    main.cpp: In instantiation of ‘void f(T&) [with T = int]’:
    main.cpp:70:6:   required from here
    main.cpp:60:9: error: ‘TD<int> tType’ has incomplete type
       TD<T> tType;               // cause T to be shown
             ^~~~~
    main.cpp:61:24: error: ‘TD<int&> paramType’ has incomplete type
       TD<decltype(param)>  paramType; // ditto for param's type
                            ^~~~~~~~~
    
.. code-block:: cpp
 
   const int& crx = x;  // crx is a reference to a const int
   f(crx); // main.cpp line 74

Using g++ version 7.2.0, the output is::

    main.cpp: In instantiation of ‘void f(T&) [with T = const int]’:
    main.cpp:74:8:   required from here
    main.cpp:60:9: error: ‘TD<const int> tType’ has incomplete type
       TD<T> tType;               // cause T to be shown
             ^~~~~
    main.cpp:61:24: error: ‘TD<const int&> paramType’ has incomplete type
       TD<decltype(param)>  paramType; // ditto for param's type
                            ^~~~~~~~~

Now if ``f`` is changed to take a ``const &``, and we re-run the same examples, we get:
 
.. code-block:: cpp

    template<typename T> void f(const T& param)  // template w/types of interest
    {
      TD<T> tType;                    // cause T to be shown
      TD<decltype(param)>  paramType; // ditto for param's type     
    }

.. code-block:: cpp

   int x = 27;
   f(rx);   // line 67

the output is::

    main.cpp: In instantiation of ‘void f(const T&) [with T = int]’:
    main.cpp:67:6:   required from here
    main.cpp:60:9: error: ‘TD<int> tType’ has incomplete type
       TD<T> tType;               // cause T to be shown
             ^~~~~
    main.cpp:61:24: error: ‘TD<const int&> paramType’ has incomplete type
       TD<decltype(param)>  paramType; // ditto for param's type
                            ^~~~~~~~~
     
.. code-block:: cpp
 
   int& rx = x; // rx is a reference to an int
   f(rx);  // line 70

the output is::

    main.cpp: In instantiation of ‘void f(const T&) [with T = int]’:
    main.cpp:70:7:   required from here
    main.cpp:60:9: error: ‘TD<int> tType’ has incomplete type
       TD<T> tType;               // cause T to be shown
             ^~~~~
    main.cpp:61:24: error: ‘TD<const int&> paramType’ has incomplete type
       TD<decltype(param)>  paramType; // ditto for param's type
                            ^~~~~~~~~
      
.. code-block:: cpp
 
   const int& crx = x; // crx is a reference to a const int.
   f(crx); // line 74

the output is::

    main.cpp: In instantiation of ‘void f(const T&) [with T = int]’:
    main.cpp:74:8:   required from here
    main.cpp:60:9: error: ‘TD<int> tType’ has incomplete type
       TD<T> tType;               // cause T to be shown
             ^~~~~
    main.cpp:61:24: error: ‘TD<const int&> paramType’ has incomplete type
       TD<decltype(param)>  paramType; // ditto for param's type
                            ^~~~~~~~~
