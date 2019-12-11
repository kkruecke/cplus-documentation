Other C++17 Features
====================

static_assert()
---------------

``static_assert()`` does compile time checking, so it is an efficient technique. 

Automatic Deduction of template parameters from constructor arguments
---------------------------------------------------------------------

In C++14 ``make_pair()`` ws a utility that eliminated the tedium of specfying the template argument manually:

.. code-block:: cpp

   pair<int, double> pr{1, 5.2}; // In C++14 the template arguments cannot be deduced from the pair constructor.
   
   pair pr = {1, 5.2};          // Compiler error in C++14

   auto p = make_pair(1,5.2);    // In C++14 we must use make_pair to avoid the tedium of manually specifying the template types.

   pair pr = {1, 5.2};          // In C++17, we can write this because the template argument types are automatically deduced from the
                                // constructor parameters.

In C++17 we can now write

.. code-block:: cpp

    vector v1{1, 2, 3};     // deduce v1's element type from the initializer element type

    vector v2 = v1;      // deduce v2's element type from v1's element type  

See Filiped p. 54.
