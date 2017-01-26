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
the current line's semicolon\ |ndash|\ as oppossed to an lvalue which is an object whose address we can take:

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

The move constructor and move assignment read from the rvalue parameter. They perform a shallow copy of its resourses, and then write to the rvalue effectively
stealing its resources. This prevents memory deallocation when the rvalue's destructor is called.

std\ |colon|\ |colon|\ move()
-----------------------------

``std::move()`` casts its argument to an rvalue. It works for both rvalues and lvalues.

Code below illustrates how remove_reference<T> and move() work
   
.. code-block:: cpp

    //This is the code that begins to illustrates what remove_reference does.
    
    // remove_reference defined
    template<typename _Tp>
      class Remove_reference
      {
        public:
        Remove_reference()
        {
          cout << "In non-specialization Remove_reference<_Tp> constructor" << endl; 
        }
        typedef _Tp   type; 
      };
    
      
    // remove_reference partial template specializations
    template<typename _Tp>
      class Remove_reference<_Tp&> { 
        public:
        Remove_reference()
        {
              cout << "In partial template specialization Remove_reference<_Tp&> constructor" << endl;
        }
        typedef _Tp   type; 
    };
    
    template<typename _Tp>
      class Remove_reference<_Tp&&> { 
        public:
         Remove_reference()
         {
             cout << "In partial template specialization Remove_reference<_Tp&&> constructor" << endl;
         } 
         typedef _Tp   type; 
    };
    
    // End Illustration Code
    
    template<typename T>
    constexpr typename Remove_reference<T>::type&& Move(T&& arg) 
    {
      Remove_reference<T> tmp;
    
      return static_cast<typename Remove_reference<T>::type&&>(arg);
    }
    
    string a{"test"};
  
    string&& rval = Move(a); 
     
    Move(a);
   
    Move(string{"xyz"});
 
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
