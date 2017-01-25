.. include:: <isopub.txt>

.. TODO: See
.. 1. ~/Document/C++11/C++Rocks.pdf pp 103-109, p. 688 of TC++PL, and
.. 2. Thomas Becker's article on Move Semantics and Perfect Forwarding: http://thbecker.net/articles/rvalue_references/section_07.html Perfect Forwarding article.
.. 3. Perfect Forwarding by Stroutrup, Sutter and Dos Reis: http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2014/n4164.pdf

.. TODO: See ~/cplus/4-perfect-forwarding.rst which has more thoughts.

Perfect Forwarding
==================

A template function parameter of type T&& such as

.. code-block:: cpp

   template<typename T> void sample(T&& t);

is called a **forwarding reference**. It behaves very differently than an rvalue reference, like the move constructor and assignment operator below:

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

The Purpose of Forwarding References
------------------------------------

Unlike an rvalue reference, a forwarding reference can bind to both an rvalue and an lvalue. It can bind to both const and non-const. It can bind to mutable and
volitale. Say, we want to add a method that would  <See below for insert vs emplace differences>

http://stackoverflow.com/questions/14788261/c-stdvector-emplace-vs-insert
http://stackoverflow.com/questions/17172080/insert-vs-emplace-vs-operator-in-c-map 

.. code-block:: cpp

   vector<int> x(10, 1); // vector<int> of length 10 initialized with ones.

   sample(x); 

   sample(vector<int>(10, 1)); 

``T`` resolves to ``vector<int>&``, and initial code generated is ``void sample(vector<int>& && t)``. When the reference collapsing rules of **C++11** are applied
to ``vector<int>& &&``, the result is ``vector<int>&``. So the actual code generated is ``void sample(vector<int>&)``.  If an rvalue is passed to sample:

.. code-block:: cpp

``T`` resolves to ``vector<int>&&``, and the code initially generated is ``void sample(vector<int>&& &&)``. Applying the reference collapsing rules the final code generated
is ``void sample(vector<int>&&)``.

Reference Collapsing Rules
--------------------------

In **C++11**, these rules apply to references to references:

* T& & becomes T&
* T& && becomes T&
* T&& & becomes T&
* T&& && becomes T&&

Helpful Links:
 http://thbecker.net/articles/rvalue_references/section_07.html
 http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2014/n4164.pdf
