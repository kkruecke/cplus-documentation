.. include:: <isopub.txt>

.. role:: cpp(code)
   :language: cpp

Observing Deduced Types
=======================

Scott Meyer's presentation ` <>`_ on youtube and in his book "Effective Modern C++" shows a trick for forcing the compiler to display how it is deducing a type.

.. code-block:: cpp

    template<typename T> void f(T& param)  // template w/types of interest
    {
      TD<T> tType;                      // cause T to be shown
      TD<decltype(param)>  paramType;  // ditto for param's type     
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


If we change ``f`` to take a ``const &&`` and use the same variables as above:
 
.. code-block:: cpp

    template<typename T> void f2(const T& param)  // template w/types of interest
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
 
