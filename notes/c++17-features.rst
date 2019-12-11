Other C++17 Features
====================

See these excellent guides
--------------------------

* https://www.codingame.com/playgrounds/2205/7-features-of-c17-that-will-simplify-your-code/introduction
* https://github.com/AnthonyCalandra/modern-cpp-features

static_assert()
---------------

``static_assert()`` does compile time checking, so it is an efficient technique. 

Class Template Agument Deduction (CTAD)
---------------------------------------

In C++14 ``make_pair()`` ws a utility that eliminated the tedium of specfying the template argument manually:

.. code-block:: cpp

   pair<int, double> pr{1, 5.2}; // In C++14 the template arguments cannot be deduced from the pair constructor.
   
   pair pr = {1, 5.2};          // Compiler error in C++14

``std::make_pair`` takes advantage of template argument deduction for function templates.

.. code-block:: cpp

   auto p = make_pair(1,5.2);    // In C++14 we must use make_pair to avoid the tedium of manually specifying the template types.

In C++17 template argument deduction for class templates is supported. The constructor arguments will be used to deduce class templates arguments 

   using namespace std;
	
   pair p(1729, "taxicab");
   static_assert(std::is_same_v<decltype(p), std::pair<int, const char *>>);    

   pair pr = {1, 5.2};        // In C++17, we can write this because the template argument types are automatically deduced from the
                              // constructor parameters.

   array ar{1, 2, 3};         // instead of array<int, 3> ar{1, 2, 3};

   tuple tup{1, 10,2, 5L};    // instead of tuple<int, double, long> tup{1, 10.2, 5L};
   

   vector v1{1, 2, 3};       // deduce v1's element type from the initializer element type

   vector v2 = v1;           // deduce v2's element type from v1's element type

   template<class T> struct A { A(T,T) {} }; // declared elsewhere

   auto y = new A{1,2};      // allocated type is A<int>  
   A a{1, 2};

   template <typename T> struct point { // declared elsewahere
      T x;
      T y;
    };

    point pt{0L,0L};

Compile-Time if: ``if constexpr``
---------------------------------
