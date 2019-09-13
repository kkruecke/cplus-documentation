.. include:: <isopub.txt>
.. include:: <isonum.txt>

Rvalue References and Forwarding References in C++
==================================================

Helpful Articles on Rvalue References, Move Semantics and Forwarding References
-------------------------------------------------------------------------------

* `Thomas Becker's article C++ rvalue Reference Explained <http://thbecker.net/articles/rvalue_references/section_07.html>`_.
* `Rvalue Reference Declarator: && <https://msdn.microsoft.com/en-us/library/dd293668.aspx>`_
* `rvalue Reference and Move Semantics <http://www.bogotobogo.com/cplusplus/C11/5_C11_Move_Semantics_Rvalue_Reference.php>`_.
* `Perfect Forwarding by Stroutrup, Sutter and Dos Reis <http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2014/n4164.pdf>`_.

Move Semantics
--------------

The Vector class below is used to explain lvalues and rvlaues and move semantics. Move semantics allow you to overloaded a class\ |apos|\ s constructor and assignment operator with a new type of reference called an **rvalue reference** 
(see :ref:`rvalue-reference`). Doing so allows the compiler to always choose the more effecient move constructor and move assignment operator when an rvalue is encountered. Below is a template ``Vector`` class with the usual copy constructor and assignment operator as well
as ``void push_back(const T&)`` that take an ``const T&``:

.. code-block:: cpp

    #include <memory>
    #include <initializer_list>
    
    template<class T>  class Vector {
    
       std::unique_ptr<T[]> p;
    
       int size;
    
       int current;
    
       void grow(); 
    
       static const int growth_factor = 2;
       static const int default_sz = 2;
    
       public:
    
         Vector() : p(std::make_unique<T[]>(Vector::default_sz )), size{Vector::default_sz}, current{0}  
         {
         }
         
         Vector(std::initializer_list<T> lst) : p(std::make_unique<T[]>(Vector::default_sz )), size{Vector::default_sz}, current{0}  
         {
            for (auto& x : lst) {
                push_back(std::move(x));  
           }
         }
        
         Vector(const Vector<T>& lhs);
         
         Vector& operator=(const Vector<T>& lhs);
        
         void push_back(const T& t);
        
         void push_back(T&& t);
    
         T& operator[](int);
        
         const T& operator[](int) const;
    
         std::ostream& print(std::ostream& ostr) const
         {
            if (size != 0) {
                
            std::copy(p.get(), p.get() + current, std::ostream_iterator<T>(ostr, ", "));
            }
            return ostr;   
         }
         
         friend std::ostream& operator<<(std::ostream& ostr, const Vector<T>& vec)
         {
             return vec.print(ostr);
         }
         
         int count() const { return size; }
         
         void* operator new (std::size_t size, void* ptr) noexcept;
    };
    
    
    template<class T> inline Vector<T>::Vector(const Vector& lhs) : p{new T[lhs.size]}, size{lhs.size}, current{lhs.current}
    {
      std::copy(p.get(), lhs.p, lhs.p + lhs.size); 
      
    }
    
    template<class T> inline Vector<T>::Vector(Vector<T>&& lhs) : p(std::move(lhs.p)), size{lhs.size}, current{lhs.current}
    {
        lhs.size = 0;
    }
    
    template<class T> Vector<T>&  Vector<T>::operator=(const Vector& lhs)
    {
       if (this != &lhs) {
    
           p = std::make_unique<T[]>(new T[lhs.size]);
           
           size = lhs.size;
    
           copy(p, lhs.p, lhs.p + lhs.size);
       }     
    
       return *this;
    }
             
    template<class T> void Vector<T>::grow()
    {
      auto new_size = size * Vector<T>::growth_factor; 
    
      std::unique_ptr<T[]> ptr = std::make_unique<T[]>(new_size); 
      
      for (auto i = 0; i < size; ++i) {
          
          ptr[i] = std::move(p[i]); 
      }
    
      size = new_size;
      
      p = std::move(ptr);
    } 
    
    template<class T> void Vector<T>::push_back(const T& t)
    {
      if (current == size) {
          
         grow();
      }
    
      p[current++] = t;
    }
    
    template<class T> T& Vector<T>::operator[](int pos)
    {
      if (pos >= size || pos < 0) {
          
          throw(std::out_of_range("pos not in range."));   
        
      } else {
    
         return p[pos];
      }
    }
        
    template<class T> inline const T& Vector<T>::operator[](int pos) const
    {
       return static_cast<T *>(this)->operator[](pos);
    }
    
.. _rvalue-reference:
    
rvalue references and their role
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

As explained above, an lvalue is object that has a name (identity), whose address can be taken, but that cannot be moved from. Examples of lvalues:

.. code-block:: cpp

    int i; // lvalue
    int j = 8; // lvalue assigned rvalue

    int& f2(); // f2() returns an lvalue

If an object is not an lvalue, it is an rvalue. See `A Brief Introduction to Rvalue References <http://www.artima.com/cppsource/rvalue.html>`_. An rvalue is a temporary object whose lifetime does not extend past the current line\ |apos|\ s semicolon\. You cannot take the address of an
rvalue. Below are some examples of rvalue and lvalues:

.. code-block:: cpp

    int i; // lvalue
    int j = 8; // lvalue assigned rvalue

    int f1(); // f1() returns an rvalue. We can't do &f1().
    int& f2(); // f2() returns an lvalue
    int&& f3(); // f3() returns an rvalue.

    class X { //...};

    X x1; // lvalue declared
    X f4(); // returns rvalue
    X& f5(); // returns lvalue

An rvalue reference is declared using ``&&``: 

.. code-block:: cpp

    int&& j = 8;
    int&& f3(); // f3() returns an rvalue
    int&& k = f3();
    int v = 9;
    int&& l = v; // error: cannot bind rvlue reference l to lvalue v.

The rvalue reference j above is not really of any value. While we can change the value of a literal using this trick
 
.. code-block:: cpp

    int&& j = 8;
    j = 9;
    cout << j;  // prints: 9

the temporay gets deleted once j goes out of scope, and this technique has no wide applicability. When the compiler see an rvalue, it thinks, "oh, this is an rvalue, let me see if the class method being invoked takes an rvalue reference, so I can
invoke it." 

.. code-block:: cpp

    #include <iostream>
    #include <utility>
     
    void f(int& x) {
        std::cout << "lvalue reference overload f(" << x << ")\n";
    }
     
    void f(const int& x) {
        std::cout << "lvalue reference to const overload f(" << x << ")\n";
    }
     
    void f(int&& x) {
        std::cout << "rvalue reference overload f(" << x << ")\n";
    }
     
    int main() {
        int i = 1;
        const int ci = 2;
        f(i);  // calls f(int&)
        f(ci); // calls f(const int&)
        f(3);  // calls f(int&&)
               // would call f(const int&) if f(int&&) overload wasn't provided
        f(std::move(i)); // calls f(int&&)
     
        // rvalue reference variables are lvalues when used in expressions
        int&& x = 1;
        f(x);            // calls f(int& x)
        f(std::move(x)); // calls f(int&& x)
    }

Note: *rvalue* reference variables are *lvalues* when used in expressions. For example, parameter d in ``Derived::Derived(Derived&& d)`` below
 
.. code-block:: cpp

   class Base {
     // snip...
    public:
     Base(const Base& b);
     Base(Base&& b);
     //snip...  
   };

   class Derived {
     // snip...
    public:
     Derived(const Derived& d);
     Derived(Derived&& d);
     //snip...  
   };
   
   Derived::Derived(Derived&& d) : Base(std::move(d)), ... {} 

is a reference to an rvalue; however, *d* itself is an lvalue because it has a name and its address can be taken. Therefore to ensure that ``Base::Base(Base&&)`` is called, it must first be cast to an rvalue using ``std::move(d)``.

Overloading Constructors and Assignment Operators with rvalue references
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

When C++11 introduced rvalue references, it allowed constructors and assignment operators to be overloaed with rvalue references, and this allows the compiler to now branch at compiler time depending on whether the constructor or assignment operator is
being passed an lvalue or an rvalue. But how do you implement the constructor and assigment operator that take an rvalue reference? 

Implementation of move constructor and move assignment operator
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The move constructor and move assignment, both of which take rvalue references, both read from and write to the rvalue reference parameter. They perform a shallow copy of its resourses, and then, as in the example below, set the rvalue object\ |apos|\ s
``length`` to 0 and it\ |apos|\ s pointer p is set to ``nullptr`` to prevent the memory being deallocated when the rvalue's destructor is called. 
 
.. code-block:: cpp

    #include <memory>
    #include <initializer_list>
    
    template<class T>  class Vector {
    
       std::unique_ptr<T[]> p;
    
       int size;
    
       int current;
    
       void grow(); 
    
       static const int growth_factor = 2;
       static const int default_sz = 2;
    
       public:
    
         Vector() : p(std::make_unique<T[]>(Vector::default_sz )), current{0}  
         {
         }
    
         Vector(std::initializer_list<T> lst) 
         {
           for (auto& x : lst) {
                push_back(x);  
           }
         }
        
         Vector(const Vector<T>& lhs);
         
         Vector(Vector<T>&& lhs); // move constructor
    
         Vector& operator=(const Vector<T>& lhs);
        
         Vector& operator=(Vector<T>&& lhs); // move assignment operator
        
         void push_back(const T& t);
        
         void push_back(T&& t);
    
         template<class... ARGS> void emplace_back(ARGS&& ... args);
        
         T& operator[](int);
        
         const T& operator[](int) const;
    
         std::ostream& print(std::ostream& ostr) const
         {
            std::copy(p.get(), p.get() + current, std::ostream_iterator<T>(ostr, "\n"));
            return ostr;   
         }
         
         friend std::ostream& operator<<(std::ostream& ostr, const Vector<T>& vec)
         {
             return vec.print(ostr);
         }
    
         void* operator new (std::size_t size, void* ptr) noexcept;
    };
    
    
    template<class T> inline Vector<T>::Vector(const Vector& lhs) : p{new T[lhs.size]}, size{lhs.size}, current{lhs.current}
    {
      std::copy(p.get(), lhs.p, lhs.p + lhs.size); 
    }
    
    template<class T> inline Vector<T>::Vector(Vector<T>&& lhs) : p(std::move(lhs.p)), size{lhs.size}, current{lhs.current}
    {
        lhs.size = 0;
    }
    
    template<class T> Vector<T>&  Vector<T>::operator=(const Vector& lhs)
    {
       if (this != &lhs) {
    
           p = std::make_unique<T[]>(new T[lhs.size]);
           
           size = lhs.size;
    
           copy(p, lhs.p, lhs.p + lhs.size);
       }     
    
       return *this;
    }
                     
    template<class T> Vector<T>&  Vector<T>::operator=(Vector&& lhs)
    {
       if (this != &lhs)  {
    
           p = std::move(lhs.p); // std::move() casts an lvalue to an rvalue. 
    
           size = lhs.size;
           
           lhs.size = 0;
       }     
    
       return *this;
    }
    
    template<class T> void Vector<T>::grow()
    {
      auto new_size = size * Vector<T>::growth_factor; 
    
      std::unique_ptr<T[]> ptr = std::make_unique<T[]>(new_size); 
      
      for (auto i = 0; i < size; ++i) {
          
          ptr[i] = std::move(p[i]); 
      }
    
      size = new_size;
      
      p = std::move(ptr);
    
      ++current;
    } 
    
    template<class T> void Vector<T>::push_back(const T& t)
    {
      if (current == size) {
          
         grow();
      }
    
      p[current++] = t;
    }
    
    template<class T> T& Vector<T>::operator[](int pos)
    {
      if (pos < size && pos > 0) {
          
         return p[pos];
    
      } else {
    
        throw(std::out_of_range("pos not in range."));
      }
    }
    
    template<class T> const T&  Vector<T>::operator[](int pos) const
    {
      if (pos < size && pos > 0) {
          
         return const_cast<const T&>(p[pos]);
    
      } else {
    
        throw(std::out_of_range("pos not in range."));
      }
    }
    
    template<class T> void  Vector<T>::push_back(T&& t)
    {
      if (current == size) {
         grow();
      }
      p[current++] = std::move( t );
    }
    
    template<class T> template<class... ARGS> void Vector<T>::emplace_back(ARGS&& ... args)
    {
       if (size == current) {
           
           grow();
       }
       
       T *ptr = p.get();
    
       T *location = ptr + current;
       
       new(location) T{std::forward<ARGS>(args)...}; 
       
       current++; 
    }
           
Obviously the versions of the constructor and assignment operator overloaded to take an rvalue reference are faster that their copy constructor and copy assignment operator counterparts. Take for example 

.. code-block:: cpp

    Vector<int> v1{1, 5, 12};
    Vector<int> v2{v2}; // invokes copy constructor    
    Vector<int> v2{v{2, 6, 16}}; // invokes move constructor Vector::Vector(Vector&&)    
                                 // because an rvalue is passed 

    template<class T> void f(Vector<T>&& v); // forward declaration

    f(Vector<int>{11, 19, 29}); // invokes move constructor Vector::Vector(Vector&&)    

Rvalue References and Derived classes
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Is an rvalue reference parameter an rvalue or an lvalue? Since an rvalue reference parameter has a name, it is not a temporary, and therefore it is an lvalue (because an rvalue reference parameter that has a name can have its address taken). 
Since it "has a name" the rvalue reference parameter is an lvalue within the scope of its function. This implies move semantics in derived classes must be implemented in a certain way:

.. code-block:: cpp

    class Base {
        char *p;
        int length;
      public:
       //...snip
       Base(Base&& lhs) 
       {
         p = lhs.p;
         lhs.p = nullptr;
         length = p.length;
         p.length = 0;  
       } 
       //...snip 
    }; 

    Derived : public Base {
       public:
         Derived(Derived&& d) : Base(std::move(d)) {}  
    };

Since ``d`` is an lvalue, the implementation of ``Derived(Derived&& d)`` requires casting ``d`` to an rvalue in order the Base move constructor is invoked rather than the copy constructor.

Note, Since ``std::move()`` works correctly on both rvalues and lvalues, no harm is done when it is passedan rvalue: an rvalue is still returned. The g++ version of ``std::move()`` is shown below. It takes an argument of generic type ``T&&``. While this looks
like an rvalue reference, it works differently than an ordinary rvalue reference\ |mdash|\ say, for example, ``std::string&&``\ |mdash|\ where the parameter's type is specified.

``T&&`` binds to both lvalues and rvalues, and is known as a forwarding reference. When it binds to an lvalue, ``T`` resolves to an lvalue reference, and when an rvalue is passed **T** resolves to the underlying nonreference type. We can see this by implementing
a version of ``Remove_reference`` and its partial template specializations that contains a static method called ``describe()``, which ``move()`` calls: 

.. code-block:: cpp

    template<typename T> constexpr typename std::Remove_reference<T>::type&& move(T&& __t) noexcept 
    { 
      return static_cast<typename std::Remove_reference<T>::type&&>(__t); 
    }

    // Remove_reference defined
    template<typename _Tp>
      struct Remove_reference
      {
        static void describe()
        {
          cout << "In non-specialization Remove_reference<_Tp> constructor" << endl;
        } 
        typedef _Tp   type; 
    };
      
    // Remove_reference partial template specializations
    template<typename _Tp>
      struct Remove_reference<_Tp&> { 
        static void describe()
        {
          cout << "In partial template specialization Remove_reference<_Tp&> constructor" << endl;
        }
        typedef _Tp   type; 
    };
    
    template<typename _Tp>
      struct Remove_reference<_Tp&&> { 
        static void describe()
        {  
         cout << "In partial template specialization Remove_reference<_Tp&&> constructor" << endl;
        }
         typedef _Tp  type; 
    };
    
    template<typename T>
    constexpr typename Remove_reference<T>::type&& move(T&& arg) 
    {
      Remove_reference<T>::describe();
    
      return static_cast<typename Remove_reference<T>::type&&>(arg);
    }

    string a{"test"};
  
    string&& rval = move(a); 
     
    string {move(string{"xyz"})};

This results in the output::

    In partial template specialization Remove_reference<_Tp&> constructor
    In non-specialization Remove_reference<_Tp> constructor

In the case of ``string {move(string{"xyz"})};``, **T** resolves to ``std::string``. This is what is instantiated step-by-step:

.. code-block:: cpp

    constexpr typename Remove_reference<std::string>::type&& move(std::string&& arg) 
    {
      Remove_reference<std::string>::describe();
    
      return static_cast<typename Remove_reference<std::string>::type&&>(arg);
    }

which simplies to:

.. code-block:: cpp

    constexpr typename std::string&& move(std::string&& arg) 
    {
      Remove_reference<std::string>::describe();
    
      return static_cast<typename std::string&&>(arg);
    }

which is a rvalue cast (to something that does not have a name). In the case of ``move(a)``, **T** resolves to ``std::string&``. Again, this is what is instantiated
step by step:

.. code-block:: cpp

    constexpr typename Remove_reference<std::string&>::type&& move(std::string&  && arg) 
    {
      Remove_reference<std::string&>::describe();
    
      return static_cast<typename Remove_reference<std::string&>::type&&>(arg);
    }

Applying the reference collapsing rules of C++11, gives us 

.. code-block:: cpp

    constexpr typename Remove_reference<std::string&>::type&& move(std::string& arg) 
    {
      Remove_reference<std::string&>::describe();
    
      return static_cast<typename Remove_reference<std::string&>::type&&>(arg);
    }

which simplies to

.. code-block:: cpp

    constexpr std::string&&  move(std::string& arg) 
    {
      Remove_reference<std::string>::describe();
    
      return static_cast<typename std::string&&>(arg);
    }

Again as before, this casts arg to an rvalue reference that does not have a name.

std::move() Implementation
~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: cpp
 
     template<typename T>
        constexpr typename std::remove_reference<T>::type&&
        move(T&& __t) noexcept
        {
          return static_cast<typename std::remove_reference<T>::type&&>(__t); 
        }

Why is the return type of ``std::move()`` is ``constexpr typename std::remove_reference<_Tp>::type&&`` instead of ``T&&``? Recall that when an lvalue is passed to ``std::move()`` like below

.. code-block:: cpp
   
   using namespace std;

   class X { 
        //snip...
      public:
        X(const X&);
        X(X&&);
        //snip...
   };

   X x1;

   X x2 = move(x1);
  
that T binds as ``X&``, and the instantiation of ``move(x1)`` before reference collapsing is done looks like this

.. code-block:: cpp

    constexpr typename std::remove_reference<X&>::type&&
    move(X& && __t) noexcept
    {
      return static_cast<typename std::remove_reference<X&>::type&&>(__t); 
    }

and after applying reference collapsing, it looks like this

.. code-block:: cpp

    constexpr typename std::remove_reference<X&>::type&&
    move(X& __t) noexcept
    {
      return static_cast<typename std::remove_reference<X&>::type&&>(__t); 
    }

``remove_reference<X&>::type`` is simply ``X``. Thus ``move(x1)`` resolves to be:

.. code-block:: cpp

    constexpr X&&
    move(X& __t) noexcept
    {
      return static_cast<X&&>(__t); 
    }

Had ``move()`` been implemented as

.. code-block:: cpp

    template<typename T>
       constexpr T&&
       move(T&& __t) noexcept
       {
         return static_cast<T&&>(__t); 
       }

then it would have been instantiated as 

.. code-block:: cpp

    constexpr X& &&
    move(X& && __t) noexcept
    {
      return static_cast<X& &&>(__t); 
    }

and after applying reference collapsing, we would have

.. code-block:: cpp

    constexpr X& 
    move(X& __t) noexcept
    {
      return static_cast<X&>(__t); 
    }

as the instantiation of ``move(x1)``, and the return value of ``move(x1)`` would still be an lvalue.  

If ``move()`` is passed an rvalue, then the instantion of  

.. code-block:: cpp

    template<typename T>
       constexpr T&&
       move(T&& __t) noexcept
       {
         return static_cast<T&&>(__t); 
       }

would work fine. For example:

    X createX();

    X x = move(createX());

would instantiate
 
.. code-block:: cpp

    constexpr X&&
    move(X&& __t) noexcept
    {
      return static_cast<X&&>(__t); 
    }

and a nameless rvalue (known as a xvalue) would be returned. ``remove_reference<T>::value&&`` is needed to ensure an lvalue is converted to an rvalue (or more specifically a xvalue).

remove_reference_t
~~~~~~~~~~~~~~~~~~

C++14 introduced a shorthand or "synonym" for ``template<class T> typename remove_reference<T>::type``, namely ``template<class T> remove_reference_t``, which is defined as:

.. code-block:: cpp

   template<class T>
   using remove_reference_t = typename remove_reference<T>::type 

We can use it to simplify the C++11 implementation of ``std::move()``, changing

.. code-block:: cpp
 
     template<typename T> 
        constexpr typename std::remove_reference<T>::type&& // C++11 implementation
        move(T&& __t) noexcept
        {
          return static_cast<typename std::remove_reference<T>::type&&>(__t); 
        }

to
 
.. code-block:: cpp
 
     template<typename T>
        constexpr typename std::remove_reference_t<T>&& // C++14
        move(T&& __t) noexcept
        {
          return static_cast<typename std::remove_reference_t<T>&&>(__t); 
        }
   
Move Conclusion
~~~~~~~~~~~~~~~

``move(T&&)`` is non-overloaded function template that casts its argument to an rvalue. It works both with lvalue and rvalue arguments. It uses the partial template specializations provided by ``Remove_reference<T>`` to do this.

.. note:: The C++ standard library's **Remove_reference** is simply ``remove_reference`` (with a lowercase r). The version above used ``Remove_reference`` (with an uppercase R), so that it would not conflict with the actual ``std::remove_reference`` in the standard library,
   which automatically gets included when <iostream> is included in main.cpp and its methods are used, as they were in the example code above.

Value Categories
----------------

articles discussing this topic: 
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

* `Building Intuition on value categories <http://marcodiiga.github.io/building-intuition-on-value-categories>`_.
* `Value Categories in C++17 <https://medium.com/@barryrevzin/value-categories-in-c-17-f56ae54bccbe>`_.
* `Stackoverflow Explanation <https://stackoverflow.com/questions/3601602/what-are-rvalues-lvalues-xvalues-glvalues-and-prvalues>`_ 
* `Value Categories at en.cppreference.com <https://en.cppreference.com/w/cpp/language/value_category>`_. 

C++11 introduced the concept of (an expression's) **value category**. The articles `Value Categories <https://en.cppreference.com/w/cpp/language/value_category>`_ at en.cppreference.com explains::

    Each C++ expression (an operator with its operands, a literal, a variable name, etc.) is characterized by two independent properties: a type and a value category.
    Each expression has some non-reference type, and each expression belongs to exactly one of the three primary value categories: prvalue, xvalue, and lvalue...

.. todo:: Use the top two references above to illustrate with examples how 1.) type and value category differ and how, in C++11 and above, the value category 2.) also affects the choice of overloaded method chosen.

Perfect Forwarding
------------------

This section discusses the use of forwarding references to implement perfect forwarding of parameters. Perfect forwardings allows us to do in_place_construction_.

Forwarding References
~~~~~~~~~~~~~~~~~~~~~

A **forwarding reference** is a template function parameter of type ``T&&`` such as

.. code-block:: cpp

   template<typename T> void sample(T&& t);

It looks just like an rvalue reference, but when ``&&`` is used in function template as above, it is called a **forwarding refernence**. Unlike an rvalue reference, a forwarding reference ``T&&`` can bind to both rvalues and lvalues. Forwarding reference take
advantage of the new **C++11** reference collapsing rules. In **C++11** unlike previous versions, you can syntactically have a reference to a reference. In this case, the following reference collapsing rules apply:
references to references:

* T& & becomes T&
* T& && becomes T&
* T&& & becomes T&
* T&& && becomes T&&

Except in the case of ``T&& &&``, the final result of reference collapsing is always ``T&``.

The Purpose of Forwarding References
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Unlike an rvalue reference, a forwarding reference ``T&&`` can bind to both rvalues and lvalues. It can bind to both const and non-const objects. It can bind to mutable and volitale. In essence, it can bind to any type. When a lvalue, say, of type X is passed to a
template function argument of generic type ``T&&``, then ``T`` becomes ``X&``, and therefore ``T&&`` becomes ``X& &&``, which after applying the reference collapsing rules becomes simply ``X&``. On the other hand, when an rvalue of type X is passed, ``T``
becomes ``X``, and ``T&&`` is thus simply ``X&&``.

Thus an lvalue of type X binds as ``X&`` and an rvalue binds as ``X&&``. We can see this in the code below:

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
    
    template<class ARG> void sample(ARG&& arg)
    {
       state_type<ARG>::describe();
    }
     
    using namespace std;
    vector<int> v{1, 2, 3, 4};
    sample(v);
    sample(vector<int>{5, 6, 7, 8});
    sample(move(v));

This will result in output of::

    In partial template specialization of struct state_type<T&>
    In non-specialization of struct state_type<T>
    In non-specialization of struct state_type<T>

For the lvalue v in ``sample(v);``, ``ARG`` resolves to ``vector<int>&``, and the instantiation of sample() is

.. code-block:: cpp 

    void sample(vector<int>& && arg)
    {
       state_type<vector<int&>::describe();
    }
    
Applying reference collapsing rules for references this becomes

.. code-block:: cpp 

    void sample(vector<int>& arg)
    {
       state_type<vector<int&>::describe();
    }
 
So we see arg binds as an lvalue reference. In the case of ``sample(vector<int>{5, 6, 7, 8});``, ``ARG`` resolves to ``vector<int>``, and the instantiation of sample looks like
this: 

.. code-block:: cpp 

    void sample(vector<int>&& arg)
    {
       state_type<vector<int>>::describe();
    }

In this case arg binds as a rvalue reference. We can use these binding rules for function templates as the first step in writing a template function that perfectly forwards its parameters leaving the paramters type intact. 

Now take this factory method:

.. code-block:: cpp

    class A { // trivial example
       std::string str;
      public:
        A(std::string& lhs) : str(lhs)
        {
          cout << " A::A(std::string& lhs) invoked." << endl;
        }
    
        A(std::string&& lhs) 
        {
          cout << " A::A(std::string&& lhs) invoked." << endl;
        }
    };

    template<class T, class ARG> std::shared_ptr<T> factory(ARG&& arg)
    {
       state_type<ARG>::describe();
    
       return std::shared_ptr<T>{new T(arg)};  // fails to invoke A(string&&) when string is rvalue.
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
 
``factory<T>(ARG&& arg)`` correctly forwarded the lvalue reference, but not the rvalue reference. Instead it got passed as lvalue references. Why? Why did ``shared_ptr<A> ptr2 { factory<A>(string{"rvaluestr"}) };``
fail in invoking ``A::A(A&&)``?

The reason is, ``arg`` is not an lvalue within the body of factory\ |ndash|\ even though the type of ``arg`` is rvalue reference! Remember than an rvalue reference, if it has a name, is an lvalue. So we need to remove the name with a cast:

.. code-block:: cpp
 
    template<class T, class ARG> std::shared_ptr<T> factory(ARG&& arg)
    {
       state_type<ARG>::describe();
    
       return std::shared_ptr<T>{ new T( static_cast<ARG&&>(arg) ) };  // Cast returns a nameless parameter.
    }

Now when ``"lvaluestr"`` is passed, ``ARG`` becomes ``string&`` and so ``ARG&&`` becomes ``string&&&``, and after applying the reference collapsing rules is simply ``string&``, and ``static_cast<string&>(arg)`` is still an lvalue. When an rvalue is passed; however,
the lvalue ``arg`` is cast to a nameless rvalue. 

The standard library provides ``forward<T>(std::remove_reference<T>::type&)`` to do this static_cast, and ith looks like this:

.. code-block:: cpp

    template<class T>
    T&& forward(typename remove_reference<T>::type& a) noexcept
    {
      return static_cast<T&&>(a);
    } 

If you use just ``T&`` instead of ``remove_reference<T>::type&`` in the defintion of ``std::forward``, perfect forwarding still works just fine. However, as Thomas Becker `explains <http://thbecker.net/articles/rvalue_references/section_08.html>`_: 
"it works fine only as long as we explicitly specify Arg as the template argument of std::forward. The purpose of the remove_reference in the definition of std::forward is to force us to do so." If we don't explicitly supply the template argument when invoking
``forward()``, a compile error results; for example

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

Returning to our original example:

.. code-block:: cpp

    template<typename _Tp>
      constexpr _Tp&&
    forward(typename std::remove_reference<_Tp>::type& __t) noexcept
    {
      return static_cast<_Tp&&>(__t); 
    }

We now use forward in our factory() function: 

.. code-block:: cpp

    string lvaluestr{"lvaluestr"};

    shared_ptr<A> ptr1 { factory<A>(lvaluestr) };

    shared_ptr<A> ptr2 { factory<A>(string{"rvaluestr"}) };

    template<class T, class ARG> std::shared_ptr<T> factory(ARG&& arg)
    {
       state_type<ARG>::describe();

       return std::shared_ptr<T>{ new T( std::forward<T>(arg) ) };  // forward returns a nameless parameter.
    }

When ``factory<A>(lvaluestr)`` is called, again, ``ARG`` resolves to ``string&`` and applying reference collapsing, we have this instantiation of factory: 

.. code-block:: cpp

    std::shared_ptr<A> factory(string& arg)
    {
       return std::shared_ptr<A>{ new A( std::forward<T>(arg) ) }; 
    }

For the accompanying forward instantiation, the partial template specialization for lvalue references is applied and ``std::remove_reference<string&>::type& `` resolves to ``string&``
and so forward() is instantiated as:

.. code-block:: cpp

   constexpr string& forward(string& __t) noexcept 
   {
      // static_cast<string& &&> collpases to static_cast<string&> below:
      return static_cast<string&>(__t); 
   }

So the complete instantiation of ``factory<A>(lvaluestr)`` is:

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

.. _in_place_construction:

Application of Perfect Forwarding
---------------------------------

Below Vector now has a new template member function ``emplace_back`` that takes variadic `forwarding references`_.

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

``emplace_back()`` creates the new vector element in-place, in the vector itself, using the object's forwarded parameters, and thus eliminating the creation and moving of a temporary object into the vector.

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

Further articles on forwarding references:
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#. `Modern C++ Features -- in place construction <https://arne-mertz.de/2016/02/modern-c-features-in-place-construction/>`_
#. https://www.youtube.com/watch?v=ECoLo17nG5c
#. http://eli.thegreenplace.net/2014/perfect-forwarding-and-universal-references-in-c/

Conclusion
~~~~~~~~~~

We have seen when an lvalue is passed to ``std::forward``, a nameless lvalue is returned; however, when an rvalue is passed to forward, a nameless rvalue is returned. This perfectly forwards
the parameters, resulting in the correct method always being invoked and its paramters perfectly forwared.

.. Say, we want to add a method that would  <See below for insert vs emplace differences>
   http://stackoverflow.com/questions/14788261/c-stdvector-emplace-vs-insert
   http://stackoverflow.com/questions/17172080/insert-vs-emplace-vs-operator-in-c-map 
