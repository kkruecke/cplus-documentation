.. include:: <isopub.txt>

.. role:: cpp(code)
   :language: cpp

Observing Deduced Types
=======================

.. todo:: The ^~~~~~~~~~~~~~~~~ seems to be in the wrong place for the paramType. Just use blockquote instead of 'raw:: html'

Scott Meyer's presentation at  at **CPPCon 2014** on `Type Deduction and Why You Care <https://www.youtube.com/watch?v=wQxj20X-tIU>`_ demonstates (at around minute 43) how to force the compiler to display the types it has
deduced when 1.) a function template is instantiated or when 2.) an auto variable is defined. The code below forces two compile errors that display both the deduced type of ``T`` and ``param``:

.. code-block:: cpp

    template<typename T> TD;   // Declaration for TD. TD == "Type Displayer"

    template<typename T> void f(T& param)  // A template w/types of interest
    {
      TD<T> tType;                      // cause T to be shown
      TD<decltype(param)>  paramType;   // ditto for param's type     
    }

Examples:

.. code-block:: cpp

   int x = 27;  
   f(x);  // main.cpp line 69

Using g++ version 7.2.0, the output is:

.. raw:: html

    <pre>
    main.cpp: In instantiation of ‘void f(T&) [with T = int]’:
    main.cpp:69:6:   required from here
    main.cpp:53:9: error: ‘TD<int> tType’ has incomplete type
       TD<T> tType;               // cause T to be shown
             ^~~~~
    main.cpp:54:24: error: ‘TD<int&> paramType’ has incomplete type
      TD<decltype(param)>  paramType; // ditto for param's type
                            ^~~~~~~~~
   </pre>

.. code-block:: cpp

   int& rx = x;
   f(rx); // main.cpp line 73

Using g++ version 7.2.0, the output is:

.. raw:: html

    <pre>
    main.cpp: In instantiation of ‘void f(T&) [with T = int]’:
    main.cpp:73:7:   required from here
    main.cpp:53:9: error: ‘TD<int> tType’ has incomplete type
       TD<T> tType;               // cause T to be shown
             ^~~~~
    main.cpp:54:24: error: ‘TD<int&> paramType’ has incomplete type
       TD<decltype(param)>  paramType; // ditto for param's type
    </pre>

.. code-block:: cpp
 
   const int& crx = x;
   f(crx); // main.cpp line 75

Using g++ version 7.2.0, the output is:

.. raw:: html

    <pre>
    main.cpp: In instantiation of ‘void f(T&) [with T = const int]’:
    main.cpp:75:8:   required from here
    main.cpp:53:9: error: ‘TD<const int> tType’ has incomplete type
       TD<T> tType;               // cause T to be shown
             ^~~~~
    main.cpp:54:24: error: ‘TD<const int&> paramType’ has incomplete type
       TD<decltype(param)>  paramType; // ditto for param's type
                            ^~~~~~~~~
    </pre>

Now if ``f`` is changed to take a ``const &&``, and we re-run the same examples, we get:
 
.. code-block:: cpp

    template<typename T> void f(const T& param)  // template w/types of interest
    {
      TD<T> tType;                    // cause T to be shown
      TD<decltype(param)>  paramType; // ditto for param's type     
    }

   int x = 27;  
   f(x);

the output is:

.. todo:: complete
 
.. code-block:: cpp

   int& rx = x;
   f(rx);

the output is:
 
.. todo:: complete
 
.. code-block:: cpp
 
   const int& crx = x;
   f(crx);

the output is:

.. todo:: complete
