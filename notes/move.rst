.. include:: <isopub.txt>
.. include:: <isonum.txt>

Move Semantics
==============

Here is a Vector class like this with copy constructors and a copy assignment operator. Both class methods take a const reference to Vector.

.. code-block:: cpp

    template<class T> class Vector {
        T *p;
        int   length;

        void copy_Vector(const Vector<T>& lhs)
        {
            length = lhs.length;
            p = new T[length];
            for(i = 0; i < length; ++i) p[i] = lhs.p[i];
        }

      public:
        Vector(int l, const T& init=T()) : length{l}
        {
          p = new T[length];

          for(auto i = 0; i < length; ++i) {
              p[i] = init;
          }   
        } 

        Vector(const Vector& lhs) // make a copy
        {
          copy_Vector(lhs);
        }      

        Vector& operator=(const Vector&lhs)
        {
           if(this != &lhs) {

              delete [] p;
              copy_Vector(lhs);
           }   
           return *this;
        }

        void push_back(const T& ele);

        T& operator[](int i) { return p[i]; }

        T operator[](int i) const { return p[i]; }

        unsigned int size() const { return length; }
        
    };

Move Semantics
--------------

C++11 introduced move constructors and move assignment operators, which are methods overloaded on rvalues. An rvalue is a temporary whose lifetime does not extend past
the current line's semicolon\ |ndash|\ as oppossed to an lvalue which is an object whose address we can take. See `A Brief Introduction to Rvalue References <http://www.artima.com/cppsource/rvalue.html>`_. 
Here are some examples:

.. code-block:: cpp

    int i; // lvalue
    int j = 8; // lvalue assigned rvale

    int f1(); // f1() returns an rvalue
    int& f2(); // f2() returns an lvalue

    class X { //...};

    X x1; // lvalue
    X f3(); // returns rvalue
    X& f4(); // returns lvalue

An rvalue variable is declared using ``&&``: 

.. code-block:: cpp

    int&& j = 8;
    int&& k = f3();

Implementation
--------------

The move constructor and move assignment both read from the rvalue parameter and write to it. They perform a shallow copy of its resourses, and then write to the rvalue
effectively stealing its resources. The rvalue's ``length`` is set to 0 and the pointer p is set to ``nullptr``. This prevents memory deallocation when the rvalue's
destructor is called. 
 
.. code-block:: cpp

    template<class T> class Vector {
        T *p;
        int   length;
        void move_Vector(Vector<T>&& lhs)
        { 
           p = lhs.p;
           lhs.p = nullptr;
           p.length = lhs.length;
           lhs.length = 0;
        } 
        void copy_Vector(const Vector<T>& lhs)
        {
            length = lhs.length;
            p = new T[length];
            for(i = 0; i < length; ++i) p[i] = lhs.p[i];
        }

      public:
        Vector(int l, const T& init=T()) : length{l}
        {
          p = new T[length];

          for(auto i = 0; i < length; ++i) {
              p[i] = init;
          }   
        } 

        Vector(const Vector& lhs) // make a copy
        {
          copy_Vector(lhs);
        }      

        Vector(Vector&& lhs) // do a shallow copy. Then alter lhs.
        {
          move_Vector(lhs); 
        } 

        Vector& operator=(const Vector&lhs)
        {
           if(this != &lhs) {

              delete [] p;
              copy_Vector(lhs);
           }   
           return *this;
        }
 
        Vector& operator=(Vector&& lhs)
        {
           delete [] p;    
           move_Vector(lhs);              
           return *this;
        }

        void push_back(const T& );

        T& operator[](int i) { return p[i]; }
        T operator[](int i) const { return p[i]; }
    };

Note concerning Derived classes
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Since an rvalue reference parameter is not a temporary and its address can be taken, an rvalue parameter is itself an lvalue. This has implications for how move semantics 
must be implemented in derived classes:

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

         Derived(Devied&& d) : Base(std::move(d)) {}  
    };

Since ``d`` is an lvalue, the implementation of ``Derived(Derived&& d)`` requires casting it to an rvalue so that the Base move constructor is called and not the copy
constructor.

How std\ |colon|\ |colon|\ move() works
---------------------------------------

``std::move()`` casts its argument to an rvalue. Since it works for both rvalues and lvalues, no harm is done when passing it an parameter that is already an rvalue.
The is the g++ version of ``std::move()`` looks like this.

.. code-block::cpp

    template<typename T> constexpr typename std::remove_reference<T>::type&& move(T&& __t) noexcept
    { 
      return static_cast<typename std::remove_reference<T>::type&&>(__t); 
    }

``T&&`` is a forwarding reference that will bind to both lvalues and rvalues. When it binds to an lvalue, ``T`` resolves to an lvalue reference, and when an rvalue is
passed T resolves to the underlying nonreference type. We can see this by implementing a version of remove_reference and its partial template specializations, which have
constructors: 

.. code-block:: cpp

    //This is the code that begins to illustrates what remove_reference does.
    
    // remove_reference defined
    template<typename _Tp>
      class remove_reference
      {
        public:
        remove_reference()
        {
          cout << "In non-specialization Remove_reference<_Tp> constructor" << endl; 
        }
        typedef _Tp   type; 
      };
    
      
    // remove_reference partial template specializations
    template<typename _Tp>
      class remove_reference<_Tp&> { 
        public:
        remove_reference()
        {
              cout << "In partial template specialization remove_reference<_Tp&> constructor" << endl;
        }
        typedef _Tp   type; 
    };
    
    template<typename _Tp>
      class remove_reference<_Tp&&> { 
        public:
         remove_reference()
         {
             cout << "In partial template specialization remove_reference<_Tp&&> constructor" << endl;
         } 
         typedef _Tp   type; 
    };
    
    template<typename T>
    constexpr typename Remove_reference<T>::type&& move(T&& arg) 
    {
      Remove_reference<T> tmp;
    
      return static_cast<typename Remove_reference<T>::type&&>(arg);
    }
    
    string a{"test"};
  
    string&& rval = Move(a); 
     
    Move(a);
   
    Move(string{"xyz"});

This results is the output:

<TODO> 
 
Conclusion:
-----------
Move() is non-overloaded function template that casts its argument to an rvalue. It works both with lvalue and rvalue arguments. It uses the partial template specializations
of remove_reference to do this.
In certain circumstances it would be nice if we could do...

This is the purpose of move semanatics...rely on rvlaue references.

How remove_reference works...

.. code-block:: cpp

    // remove_reference defined
    template<typename _Tp>
      class remove_reference
      {
        public:
        remove_reference()
        {
          cout << "In non-specialization remove_reference<_Tp> constructor" << endl; 
        }
        typedef _Tp   type; 
      };
      
    // remove_reference partial template specializations
    template<typename _Tp>
      class remove_reference<_Tp&>
      { 
        public:
        remove_reference()
        {
              cout << "In partial template specialization remove_reference<_Tp&> constructor" << endl;
        }
        typedef _Tp   type; 
    };
    
    template<typename _Tp>
      class remove_reference<_Tp&&>
      { 
        public:
         remove_reference()
         {
             cout << "In partial template specialization remove_reference<_Tp&&> constructor" << endl;
         } 
         
         typedef _Tp   type; 
      };
    
    template<typename _Tp>
     void 
      Move(_Tp&& __t) noexcept
    {  
       remove_reference<_Tp> remref;
                
       //typename remove_reference<_Tp>::type v;
        
       return;
    }
 
    string a{"test"};
      
    Move(a);
    
    Move(string{"xyz"});

Mention here what it prints out.    
