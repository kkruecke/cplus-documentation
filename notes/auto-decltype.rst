When auto sets the type of a declared variable from its initializing expression, it proceeds as follows:

1. First, if an auto vairable is assigned from a reference, the reference is ignored in determing the type of the auto variable. For example,

.. code-block:: cpp

    int x = 10;
    int& rx = x;
    auto y = crx; // The type of y is 'int'

2. Next, if, after the step above has been performed, any const and/or volatile qualifier is also ignored. For example:

.. code-block:: cpp

    int x = 10;
    const int& rx = x;
    *crx = 11; // error: *crx is read only
    auto y = crx; // The type of y is 'int' 

The type of ``y`` above is ``int``. Both the reference and const are ignored. To make ``y`` a reference you simply use ``auto&`` or ``const auto&&``:

.. code-block:: cpp

    int x = 10;
    const int& rx = x;
    auto& y = crx; // The type of y is 'int&' 
    const auto& a = crx; // The type of a is 'const int&' 

Further Examples
^^^^^^^^^^^^^^^^

To further clarify the use of ``auto&``. What happens when a ``auto& rc`` is assigned from a ``const int``?

.. code-block:: cpp

    const int c = 0;
    auto& rc = c;
    rc = 44; // Is rc an 'int &' or an 'const int&'?

Obviously, ``c`` cannot be changed because it is ``const``. Therefore ``rc`` must be  a ``const int&``. Note: This example still adheres to the two rules above (since c was not a reference).

Use of auto&&
^^^^^^^^^^^^^

``auto&&`` behaves like template functions parameters that are declared using ``&&``:

.. todo:: Provide a cross link to forward references.

.. code-block:: cpp

   template<typename T> T f(T&& t)
   {
     T ret{t};
     return ret;
   } 

   class Example {
           std::vector<int> v;
      public:
          Example() : v{0, 1, 2, 3} {}
          Example(const Example& lhs) : v{lhs} 
          {
             cout << "Example copy ctor called." << std::endl;
          }

          Example(Example&& lhs) : v{std::move(lhs}
          {             
             cout << "Example move ctor called." << std::endl;
          }   
          const std::vector<int>&  get_vector() const { return v;}
   };

   Example example1{}; // lvalue

   T t1{  f(example1) };
   T t2{ f(Example{} );  // rvalue
