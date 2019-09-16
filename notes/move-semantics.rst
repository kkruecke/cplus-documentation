.. include:: <isopub.txt>
.. include:: <isonum.txt>

Move Semantics
==============

Overloading Constructors and Assignment Operators with rvalue references
------------------------------------------------------------------------

.. todo:: See also https://www.fluentcpp.com/2018/07/17/how-to-construct-c-objects-without-making-copies/

When C++11 introduced rvalue references, it allowed constructors and assignment operators to be overloaed with rvalue references, and this allows the compiler to now branch at compiler time depending on whether the constructor or assignment operator is
being passed an lvalue or an rvalue. But how do you implement the constructor and assigment operator that take an rvalue reference? 

The Vector class below is used to explain lvalues and rvlaues and move semantics. Move semantics allow you to overloaded a class\ |apos|\ s constructor and assignment operator with a new type of reference called an **rvalue reference** 
(see :ref:`rvalue-reference`). Doing so allows the compiler to always choose the more effecient move constructor and move assignment operator when an rvalue is encountered. Below is a template ``Vector`` class with the usual copy constructor and assignment operator as well
as ``void push_back(const T&)`` that take an ``const T&``:

.. code-block:: cpp
    
    #include <memory>
    #include <initializer_list>
    #include <iostream>
    #include <iterator>
    
    template<class T>  class Vector {
    
       std::unique_ptr<T[]> p;
    
       int size;
    
       int current;
    
       void grow();
    
       static const int growth_factor = 2;
       static const int default_sz = 2;
    
       public:
    
         Vector() : p(std::make_unique<T[]>(Vector::default_sz)), size{Vector::default_sz}, current{0}
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

.. todo:: probably need to re-comment about ravlue reference paramaters are lvalues

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

Therefore to ensure the *Derived* move constructor invokes ``Base::Base(Base&&)``, *d* must first be cast to an rvalue using ``std::move(d)``.
All this is explained in more detail below.
   
Implementation of move constructor and move assignment operator
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The move constructor and move assignment, both of which take rvalue references, both read from and write to the rvalue reference parameter. They perform a shallow copy of its resourses, and then, as in the example below, set the rvalue object\ |apos|\ s
``length`` to 0 and it\ |apos|\ s pointer p is set to ``nullptr`` to prevent the memory being deallocated when the rvalue's destructor is called. 
 
.. code-block:: cpp

    #include <memory>
    #include <initializer_list>
    #include <iostream>
    #include <iterator>
    
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
    Vector<int> v2{v1}; // invokes copy constructor    
    Vector<int> v3{v{2, 6, 16}}; // move constructor Vector::Vector(Vector&&) invoked    
                                 // because an rvalue is passed 

    template<class T> void f(Vector<T>&& v); // forward declaration

    f(Vector<int>{11, 19, 29}); // move constructor Vector::Vector(Vector&&) invoked    

*v2* above does not allocation any memory. Instead it "steals" the memory allocated by v1 by copying *v1*'s ``int *`` pointer and then setting *v1*'s pointer to *nullptr*. The same comments apply to *v3* which steals the memory allocated
by the rvalue vector passed to it.

Rvalue References and Derived classes
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Is an rvalue reference parameter an rvalue or an lvalue? Since an rvalue reference parameter has a name, it is not a temporary. It is therefore an lvalue. Since it "has a name" the rvalue reference parameter is an lvalue within the scope of its
function. This implies move semantics in derived classes must be implemented in a certain way:

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

Since ``d`` is an lvalue, the implementation of ``Derived(Derived&& d)`` requires casting ``d`` to an rvalue in order that the Base move constructor is invoked rather than the default copy constructor.

Note, Since ``std::move()`` works correctly on both rvalues and lvalues, no harm is done when it is passed an rvalue: an rvalue is still returned. The g++ version of ``std::move()`` is discussed below.
Before one can understand the implementation of *std::move()*, it is first necessary to understand forwarding references, which are discussed here :ref:`perfect-forwarding-label`.

..  todo:: Is the section/discussion below part of forwarding references? Forwarding referencs need to first be understood to understand std::remove_reference and std::move!!!!!

It takes an argument of generic type ``T&&``.  While this looks like an rvalue reference, it works differently than an ordinary rvalue reference\ |mdash|\ say, for example, ``std::string&&``\ |mdash|\ where the parameter's type is specified.

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

.. todo:: First discuss forwarding references before discussing std::move

std::move() Implementation
~~~~~~~~~~~~~~~~~~~~~~~~~~

Before one can understand the implementation of *std::move()*, it is first necessary to understand forwarding references, which are discussed here :ref:`perfect-forwarding-label`.

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

.. todo:: Decide what to do with the material below...


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



