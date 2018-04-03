.. include:: <isopub.txt>

.. role:: cpp(code)
   :language: cpp

Observing Deduced Types
=======================

Scott Meyer's presentation `Type Deduction and Why You Care <https://www.youtube.com/watch?v=wQxj20X-tIU>`_ at **CPPCon 2014** (starting around minute 43) shows a technique for forcing the compiler to display the type it has deduced. The code, shown  below, results
in two compile errors that display the deduced type of ``T`` and the deduced type of ``param``:

.. code-block:: cpp

    template<typename T> TD;   // Declaration for TD. TD == "Type Displayer"

    template<typename T> void f(T& param)  // template w/types of interest
    {
      TD<T> tType;                      // cause T to be shown
      TD<decltype(param)>  paramType;   // ditto for param's type     
    }

Examples:

.. code-block:: cpp

   int x = 27;  
   f(x);

Output is:
 
.. code-block:: cpp

   int& rx = x;
   f(rx);

.. code-block:: cpp
 
   const int& crx = x;
   f(crx);

Output is:


Now if change ``f`` to take a ``const &&`` and run the same examples, we get the results show below.
 
.. code-block:: cpp

    template<typename T> void f(const T& param)  // template w/types of interest
    {
      TD<T> tType;                    // cause T to be shown
      TD<decltype(param)>  paramType; // ditto for param's type     
    }

   int x = 27;  
   f(x);

Output is:
 
.. code-block:: cpp

   int& rx = x;
   f(rx);

Output is:
 
.. code-block:: cpp
 
   const int& crx = x;
   f(crx);

Output is:
 
