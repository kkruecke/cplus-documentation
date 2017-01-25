.. include:: <isopub.txt>

Move Semantics
==============

Consider a Vector class like this with copy constructors and an assignment operator. Both class methods take a const reference to Vector.

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

        T& operator[](int i) { return p[i]; }

        T operator[](int i) const { return p[i]; }
    };

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

        push_back()

        T& operator[](int i) { return p[i]; }
        T operator[](int i) const { return p[i]; }
    };

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
