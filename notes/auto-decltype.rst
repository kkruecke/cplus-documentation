Use of auto, decltype(), and decltype(auto)
===========================================

How auto deduces types
-----------------------

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
----------------

To further clarify the use of ``auto&``. What happens when a ``auto& rc`` is assigned from a ``const int``?

.. code-block:: cpp

    const int c = 0;
    auto& rc = c;
    rc = 44; // Is rc an 'int &' or an 'const int&'?

Obviously, ``c`` cannot be changed because it is ``const``. Therefore ``rc`` must be  a ``const int&``. Note: This example still adheres to the two rules above (since c was not a reference).

Use of auto&&
-------------

``auto&&`` behaves like template functions parameters that are declared using ``&&``:

.. todo:: Provide a cross link to forwarding references here.

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

decltype(*name*) and decltype(*expression*) deduction rules
-----------------------------------------------------------

**decltype** means the 'declared type'. If you use decltype with a name, it will give you the declared type of that name:

.. code-block:: cpp

    int x = 10;
    decltype(x); //  decltype(x) = int

    const auto& rx = x;
    decltype(rx); //  decltype(x) = const int&

If you have an expression instead of a name, then ``decltype(expr)`` is either an lvalue or an rvalue. If it an lvalue, then decltype will add a reference to it. Below when we add parenthesis to ``x`` before passing it to **decltype**, we turn it into an expression;
it is not longer solely a name:

.. code-block:: cpp

    decltype((x));

and the result of ``decltype((x))`` is ``int &`` because ``(x)`` is an expression not a name, and thus decltype adds a reference to the type of the lvalue expression.

Template Functions Returning ``auto`` versus ``decltype(auto)``
----------------------------------------------------------------

Consider this function template that whose return type is declared ``auto`` 

.. code-block:: cpp

    template<class Container, class Index> auto get_value(Container& c, Index i)
    {
        return c[i];
    }
    
    vector<int> v{1, 2, 3 ,4, 5};
  
    cout << "get_value(1, 3) = is: " << get_value(v, 3) << endl;

This produces the expected output of::

    get_value(v, 3) is: 4

However, assigning to ``get_values(v, 3) = 10`` fails to compile. Why? Most containers with an index operator like ``std::vector<int>`` return an reference to an lvalue: in the case of ``vecotr<int>``, an ``int&`` is retunred. However, if the return type of the 
template ``get_value()`` is ``auto``, instead of returning ``in&``, ``int`` is returned. That is, the value of the return type is the same as the value of x below:

.. code-block:: cpp
    
    vector<int> v{1 ,2 ,3 ,4 5};
    auto y = v[3]; // y is of type 'int' not 'int&'


This is because the ``auto`` return type uses **template (not auto) type deduction rules** and not the normal auto type deduction rules for objects. But, again, when auto is used as a return type, it uses template type deduction rules. Therefore,
to return the desired ``int&`` return type above, the type identical to ``c[i]``, we must use ``decltype(auto)``, which will retun the same type as ``y`` and ``z`` below

.. code-block:: cpp

    vector<int> v{1, 2, 3, 4, 5};
    
    auto x = v[3];
    
    decltype(auto) y = v[3];
    
    decltype(v[3]) z = v[3];
    
    y = 10;
    
    cout << "v[3] = " << v[3] << ", x = " << x << ", y = " << y << ", and z = " << z << endl;
    
The output is::

    v[3] = 10, x = 4, y = 10, and z = 10

because the **decltype(auto)** means 'automatically deduce the return type using the decltype type deduction rules'. So we must reimplement ``get_values()`` as

.. code-block:: cpp

    template<class Container, class Index> decltype(auto) get_value(Container& c, Index i)
    {
        return c[i];
    }

    vector<int> v{1, 2, 3, 4, 5};
 
    get_value(v, 3) = 10;

    cout << "v[3] = " << v[3] << ", get_value(v, 3) = " << get_value(v, 3) << endl;

which produces:

    v[3] = 10, get_value(v, 3) = 10

.. note:: The required C++11 syntax for get_value() would have been:

.. code-block:: cpp

    template<class Container, class Index> auto get_value(Container& c, Index i) ->  decltype(c[i])
    {
        return c[i];
    }

In summary, we need to know the use case for your function: do you want template type deduction rules, then use ``auto`` for the return type; if you want the decltype type deduction, then use ``decltype(auto)``. It often boils down to whether you want
an lvalue reference return or an rvalue. In general, ``decltype(auto)`` will return the type of the actual expression or object being returned. So in general it is the first choice to always consider.
rules described above.

Finally, the same comments about template returns types apply to lambdas.
