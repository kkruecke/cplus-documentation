.. include:: <isopub.txt>
.. include:: <isonum.txt>

Rvalue References and Forwarding References in C++
==================================================

Basic Concepts
--------------

#. `Default Constructable <http://en.cppreference.com/w/cpp/concept/DefaultConstructible>`_	specifies that an object of the type can be default constructed
#. `Destructable <http://en.cppreference.com/w/cpp/concept/Destructible>`_ specifies that an object of the type can be destroyed
#. `Copy Constructable <http://en.cppreference.com/w/cpp/concept/CopyConstructible>`_ 	specifies that an object of the type can be constructed from an lvalue
#. `Copy Assignable <http://en.cppreference.com/w/cpp/concept/CopyAssignable>`_ 	specifies that an object of the type can be assigned from  an lvalue
#. `Copy Insertable <http://en.cppreference.com/w/cpp/concept/CopyInsertable>`_
#. `Move Constructable <http://en.cppreference.com/w/cpp/concept/MoveConstructible>`_	specifies that an object of the type can be constructed from an rvalue
#. `Move Assignable <http://en.cppreference.com/w/cpp/concept/MoveAssignable>`_ 	specifies that an object of the type can be assigned from an rvalue

Container Element Concepts
--------------------------

See http://en.cppreference.com/w/cpp/concept

#. DefaultInsertable element can be default-constructed in uninitialized storage
#. CopyInsertable element can be copy-constructed in uninitialized storage
#. MoveInsertable element can be move-constructed in uninitialized storage
#. EmplaceConstructible element can be constructed in uninitialized storage
#. Erasable element can be destroyed using an allocator 

Move Semantics
--------------

This Vector class serves as a motivating example for move semantics. Move semantics allow you to overloaded a class\ |apos|\ s constructor and assignment operator with an rvalue reference (see :ref:`rvalue-reference`). Such
overloaded methods allow the compiler to always chose the a more effecient constructor or assignment operator when an rvalue is passed as input. Here is template class ``Vector``.
It has the usual copy constructor and assignment operator as well as an addend method called ``void push_back(const Vector<T>)`` that take an ``const Vector&``:

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

To understand move semantics we first need to understand rvalue references. An rvalue is a temporary object whose lifetime does not extend past the current line\ |apos|\ s semicolon\ |ndash|\ as
oppossed to an lvalue, which is an object whose address we can always take. See in-depth details see `A Brief Introduction to Rvalue References <http://www.artima.com/cppsource/rvalue.html>`_. 
Some example of rvalue and lvalues:

.. code-block:: cpp

    int i; // lvalue
    int j = 8; // lvalue assigned rvalue

    int f1(); // f1() returns an rvalue
    int& f2(); // f2() returns an lvalue
    int&& f3(); // f3() returns an rvalue

    class X { //...};

    X x1; // lvalue
    X f4(); // returns rvalue
    X& f5(); // returns lvalue

An rvalue variable is declared using ``&&``: 

.. code-block:: cpp

    int&& j = 8;
    int&& k = f3();
    int v = 9;
    int&& l = v; // error: cannot bind to rvlue reference l to lvalue v.

The rvalue reference j above is not really of any value. While we can change the value of a literal, using this trick
 
.. code-block:: cpp

    int&& j = 8;
    j = 9;
    cout << j;  // prints: 9

The temporay is deleted once j goes out of scope, and this technique has not real applicability. The real value of rvalues simply lie in the ability of the compiler to detect then. If the compiler see an rvalue, it thinks, "oh, this is an
rvalue, is there method that takes an rvalue reference"? It there is, it invokes it. 

Implications for constructors and assignment operators
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

When C++11 introduced rvalue references, it allowed constructors and assignment operators to be overloaed with rvalue references.  The compiler can will now branch at compiler time depending on whether the constructor (or assignment operator) is
being passed an lvalue or an rvalue. But how do you implement the constructor and assigment operator that take an rvalue reference? 

Implementation of move constructor and move assignment operator
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The move constructor and move assignment both read from and write to the rvalue reference parameter. They perform a shallow copy of its resourses, and then, in the example below, set the rvalue object\ |apos|\ s ``length`` to 0 and it\ |apos|\ s
pointer p is set to ``nullptr``. This prevents memory deallocation when the rvalue's destructor is called. 
 
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
         
         Vector(Vector<T>&& lhs); 
    
         Vector& operator=(const Vector<T>& lhs);
        
         Vector& operator=(Vector<T>&& lhs);
        
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
    
           p = std::move(lhs.p); 
    
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
           
Obviously the constructor and assignment operator overloaded to take an rvalue reference are more efficient that their copy constructor and copy assignment operator counterparts. For example 

.. code-block:: cpp

    Vector<int> v1{1, 5, 12};
    Vector<int> v2{v2}; // invokes copy constructor    
    Vector<int> v2{v{2, 6, 16}; // invokes move constructor Vector::Vector(Vector&&)    
    template<class T> void f(Vector<T>&& v);
    f(Vector<intT>{11, 19, 29}); // invokes move constructor Vector::Vector(Vector&&)    

Rvalue References and Derived classes
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Is an rvalue reference parameter itself an rvalue or an lvalue? The answer is, an rvalue reference that has a name is considered an lvalue. An rvalue reference parameter that has a name can have its address taken. It is therefore not a temporary.
So when it "has a name" the rvalue reference parameter itself is also an lvalue with the body of the method. This has implications for how move semantics must be implemented in derived classes:

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

Since ``d`` is an lvalue, the implementation of ``Derived(Derived&& d)`` requires casting it to an rvalue so that the Base move constructor is called and not the copy constructor.

Note, Since ``std::move()`` works correctly on both rvalues and lvalues, no harm is done when passing it an rvalue: it still returns an rvalue. The g++ version of ``std::move()`` is shown below. Its argument is of generic type ``T&&``. This looks
like an rvalue reference, but it works differently than an ordinary rvalue reference, say, ``std::string&&``, where the type is hard coded. `T&&`` binds to both lvalues and rvalues, and is known as a forwarding reference. When it binds to an lvalue, ``T`` resolves
to an lvalue reference, and when an rvalue is passed **T** resolves to the underlying nonreference type. We can see this by implementing a version of ``remove_reference`` and its partial template specializations that contains a static method
called ``describe()``, which ``move()`` invokes: 

.. code-block:: cpp

    template<typename T> constexpr typename std::remove_reference<T>::type&& move(T&& __t) noexcept
    { 
      return static_cast<typename std::remove_reference<T>::type&&>(__t); 
    }

    // remove_reference defined
    template<typename _Tp>
      struct remove_reference
      {
        static void describe()
        {
          cout << "In non-specialization remove_reference<_Tp> constructor" << endl;
        } 
        typedef _Tp   type; 
    };
      
    // remove_reference partial template specializations
    template<typename _Tp>
      struct remove_reference<_Tp&> { 
        static void describe()
        {
          cout << "In partial template specialization remove_reference<_Tp&> constructor" << endl;
        }
        typedef _Tp   type; 
    };
    
    template<typename _Tp>
      struct remove_reference<_Tp&&> { 
        static void describe()
        {  
         cout << "In partial template specialization remove_reference<_Tp&&> constructor" << endl;
        }
         typedef _Tp  type; 
    };
    
    template<typename T>
    constexpr typename remove_reference<T>::type&& move(T&& arg) 
    {
      remove_reference<T>::describe();
    
      return static_cast<typename remove_reference<T>::type&&>(arg);
    }

    string a{"test"};
  
    string&& rval = move(a); 
     
    string {move(string{"xyz"})};

This results in the output::

    In partial template specialization remove_reference<_Tp&> constructor
    In non-specialization remove_reference<_Tp> constructor

In the case of ``string {move(string{"xyz"})};``, **T** resolves to ``std::string``. This is what is instantiated step-by-step:

.. code-block:: cpp

    constexpr typename remove_reference<std::string>::type&& move(std::string&& arg) 
    {
      remove_reference<std::string>::describe();
    
      return static_cast<typename remove_reference<std::string>::type&&>(arg);
    }

which simplies to:

.. code-block:: cpp

    constexpr typename std::string&& move(std::string&& arg) 
    {
      remove_reference<std::string>::describe();
    
      return static_cast<typename std::string&&>(arg);
    }

which is a rvalue cast (to something that does not have a name). In the case of ``move(a)``, **T** resolves to ``std::string&``. Again, this is what is instantiated
step by step:

.. code-block:: cpp

    constexpr typename remove_reference<std::string&>::type&& move(std::string&  && arg) 
    {
      remove_reference<std::string&>::describe();
    
      return static_cast<typename remove_reference<std::string&>::type&&>(arg);
    }

If we applying the reference collapsing rules of C++11, this becomes:

.. code-block:: cpp

    constexpr typename remove_reference<std::string&>::type&& move(std::string& arg) 
    {
      remove_reference<std::string&>::describe();
    
      return static_cast<typename remove_reference<std::string&>::type&&>(arg);
    }

which simplies to

.. code-block:: cpp

    constexpr std::string&&  move(std::string& arg) 
    {
      remove_reference<std::string>::describe();
    
      return static_cast<typename std::string&&>(arg);
    }

And again as before, this casts arg to rvalue reference that does not have a name.

Move Conclusion:
~~~~~~~~~~~~~~~~

``move(T&&)`` is non-overloaded function template that casts its argument to an rvalue. It works both with lvalue and rvalue arguments. It uses the partial template specializations provided by ``remove_reference<T>`` to do this.

Helpful Articles on Rvalue References and Move Semantics
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

* Thomas Becker's article `C++ rvalue Reference Explained <http://thbecker.net/articles/rvalue_references/section_07.html>`_.
* `Move semantics and rvalue references in C++11 <https://www.cprogramming.com/c++11/rvalue-references-and-move-semantics-in-c++11.html>`_.
* `Perfect Forwarding by Stroutrup, Sutter and Dos Reis <http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2014/n4164.pdf>`_.
* `rvalue Reference and Move Semantics <http://www.bogotobogo.com/cplusplus/C11/5_C11_Move_Semantics_Rvalue_Reference.php>`_

Perfect Forwarding
------------------

This section discusses the use of forwarding references to implement perfect forwarding of parameters orechnique used to do in_place_construction_.

Forwarding References
~~~~~~~~~~~~~~~~~~~~~

A template function parameter of type ``T&&`` such as

.. code-block:: cpp

   template<typename T> void sample(T&& t);

is called a **forwarding reference**. Forwarding reference take advantage of the new **C++11** reference collapsing rules. In **C++11** unlike previous versions, you can have a reference to a reference. These reference collapsing rules apply to
references to references:

* T& & becomes T&
* T& && becomes T&
* T&& & becomes T&
* T&& && becomes T&&

The Purpose of Forwarding References
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Unlike an rvalue reference, a forwarding reference ``T&&`` can bind to both rvalues and lvalues. It can bind to both const and non-const objects. It can bind to mutable and volitale. When a lvalue, say, of type X is passed to a
template function argument of generic type ``T&&``, then ``T`` becomes ``X&``, and ``T&&`` becomes ``X& &&``, which after applying the reference collapsing rules is simply ``X&``. On the other hand, when an rvalue of type X is passed, ``T``
becomes ``X``, and ``T&&`` becomes ``X&&``.

So in general, an lvalue of type X binds as ``X&`` and an rvalue binds as ``X&&``. We can see this in the code below:

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

This will result in output of::

    In partial template specialization of struct state_type<T&>
    In non-specialization of struct state_type<T>

For the lvalue v in ``sample(v);``, ``ARG`` resolves to ``vector<int>&``, and the instantiation of sample() is

.. code-block:: cpp 

    void sample(vector<int>& && arg)
    {
       state_type<vector<int>&>::describe();
    }
    
Applying reference collapsing rules for references this becomes

.. code-block:: cpp 

    void sample(vector<int>& arg)
    {
       state_type<vector<int>&>::describe();
    }
 
So we see arg binds as an lvalue reference. In the case of ``sample(vector<int>{5, 6, 7, 8});``, ``ARG`` resolves to ``vector<int>``, and the instantiation of sample looks like
this: 

.. code-block:: cpp 

    void sample(vector<int>&& arg)
    {
       state_type<vector<int>>::describe();
    }

In this case arg binds as a rvalue reference. We can use these binding rules for function templates as the first step in writing a template function that perfect forwards
its parameters. 

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

Note the output of the code below, where first an lvalue then an rvlaue is passed to ``factory<T>(ARG&& arg)`` and then an rvalue: 

.. code-block:: cpp

    string lvaluestr{"lvaluestr"};

    shared_ptr<A> ptr1 { factory<A>(lvaluestr) };

    shared_ptr<A> ptr2 { factory<A>(string{"rvaluestr"}) };

The output is:    

.. code-block:: cpp

    In partial template specialization of struct state_type<T&>
     A::A(std::string& lhs) invoked.
    In non-specialization of struct state_type<T>
     A::A(std::string& lhs) invoked.
 
``factory<T>(ARG&& arg)`` correctly forwarded the lvalue reference, but not the rvalue reference. Instead it got passed as lvalue references. Why? Why did ``shared_ptr<A> ptr2 { factory<A>(string{"rvaluestr"}) };``
fail in invoking ``A::A(A&&)``?

The reason is, ``arg`` is not an lvalue within the body of factory\ |ndash|\ even though the type of ``arg`` is rvalue reference! Remember than an rvalue 
reference, if it has a name, is an lvalue. So we need to remove the name with a cast:

.. code-block:: cpp
 
    template<class T, class ARG> std::shared_ptr<T> factory(ARG&& arg)
    {
       state_type<ARG>::describe();
    
       return std::shared_ptr<T>{ new T( static_cast<ARG&&>(arg) ) };  // Cast returns a nameless parameter.
    }

Now when ``lvaluestr`` is passed, ``ARG&&`` becomes ``string&``, and ``static_cast<string&>(arg)`` is still an lvalue. When an rvalue is passed, however, the lvalue ``arg`` is  cast
to a nameless rvalue. 

The standard library provides ``forward<T>(std::remove_reference<T>::type&)`` to do the static_cast, which looks like:

.. code-block:: cpp

    template<class S>
    S&& forward(typename remove_reference<S>::type& a) noexcept
    {
      return static_cast<S&&>(a);
    } 

If you use just ``S&`` instead of ``remove_reference<S>::type&`` in the defintion of ``std::forward``, perfect forwarding still works just fine. However, as Thomas Becker `explains <http://thbecker.net/articles/rvalue_references/section_08.html>`_: 
"it works fine only as long as we explicitly specify Arg as the template argument of std::forward. The purpose of the remove_reference in the definition of std::forward is to
force us to do so." 

.. code-block:: cpp

    template<typename _Tp>
      constexpr _Tp&&
    forward(typename std::remove_reference<_Tp>::type& __t) noexcept
    { return static_cast<_Tp&&>(__t); }

We now use forward in our factory() function: 

.. code-block:: cpp

    string lvaluestr{"lvaluestr"};

    shared_ptr<A> ptr1 { factory<A>(lvaluestr) };

    shared_ptr<A> ptr2 { factory<A>(string{"rvaluestr"}) };

    template<class T, class ARG> std::shared_ptr<T> factory(ARG&& arg)
    {
       state_type<ARG>::describe();

       return std::shared_ptr<T>{ new T( std::forward(arg) ) };  // forward returns a nameless parameter.
    }

When ``factory<A>(lvaluestr)`` is called, again, ``ARG`` resolves to ``string&`` and applying reference collapsing, we have this instantiation of factory: 

.. code-block:: cpp

    std::shared_ptr<A> factory(string& arg)
    {
       return std::shared_ptr<A>{ new A( std::forward(arg) ) }; 
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

In-Place Construction using Perfect Forwarding
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

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

See:

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

Helpful Links on Forwarding Refererences
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#. `Forwarding Reference by Herb Sutter, Bjarne Stroustup and Gabriel Dos Ries <http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2014/n4164.pdf>`_
#. https://arne-mertz.de/2015/10/new-c-features-templated-rvalue-references-and-stdforward/
#. `rvalue references explained <http://thbecker.net/articles/rvalue_references/section_07.html>`_
