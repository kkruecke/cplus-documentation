.. include:: <isopub.txt>
.. include:: <isonum.txt>

Managed Pointers
================

Prefer std::make_shared and std::make_unique to new
---------------------------------------------------

Prefer ``shared_ptr<T> ptr = make_shared<T>(parm1, parm2)`` to ``shared_ptr<T> ptr = new T(parm1, parm2)``. The same comment applies to ``std::make_unique<T>``. It is more efficient because only one memory allocation occurs; for example,
in the case of ``std::make_shared<T>`` the control block that contains the shared count and the weak count for the share_ptr is contained within the same chunk of memory allocated to hold T.

std::weak_ptr: Use of and Use Cases for
---------------------------------------

If two class each contain a shared_ptr to the other, the underlying heap-allocated object will never be deleted. To see this circular reference problem, consider two classes A and B, where A has a ``shared_ptr<B>`` member
and B has a ``shared_ptr<A>`` member:

.. code-block:: cpp

    class B;
    
    class  A {
    public:
       shared_ptr<B> b_ptr;    
       A()
       {
           cout << "A::A() Constructor" << endl;
       }
       ~A()
       {
           cout << "~A::A Destructor" << endl;
       }
    };
    
    class B {
        public:
        shared_ptr<A> a_ptr;
       B()
       {
           cout << "B::B() Constructor" << endl;
       }
       ~B()
       {
           cout << "~B::B Destructor" << endl;
       }
    };
    
    shared_ptr<A> ptr_2a = make_shared<A>();
    
    shared_ptr<B> ptr_2b = make_shared<B>();
    
    cout << "ptr_2a reference count = " << ptr_2a.use_count() << endl;
    
    cout << "ptr_2b reference count = " << ptr_2b.use_count() << endl;
        
    ptr_2a->b_ptr = ptr_2b;
    
    ptr_2b->a_ptr = ptr_2a;
    
    cout << "Setting:   ptr_2a->b_ptr = ptr_2b;" << endl;
    cout << "Setting:   ptr_2b->a_ptr = ptr_2a;" << endl;
                   
    cout << "ptr_2a reference count now = " << ptr_2a.use_count() << endl;
    
    cout << "ptr_2b reference count now = " << ptr_2b.use_count() << endl;
    
    cout << "ptr_2a->b_ptr reference count = " << ptr_2a->b_ptr.use_count() << endl;
    
    cout << "ptr_2b->a_ptr reference count = " << ptr_2b->a_ptr.use_count() << endl;

The output shows neither A's destructor nor B's destructor gets called::

    A::A() Constructor
    B::B() Constructor
    ptr_2a reference count = 1
    ptr_2b reference count = 1
    Setting:   ptr_2a->b_ptr = ptr_2b;
    Setting:   ptr_2b->a_ptr = ptr_2a;
    ptr_2a reference count now = 2
    ptr_2b reference count now = 2
    ptr_2a->b_ptr reference count = 2
    ptr_2b->a_ptr reference count = 2

Because ``std::weak_ptr<>`` does not participate in the reference count, it can be used to break the circular reference cycle: 

.. code-block:: cpp

    class B;
    
    class  A {
    public:
       shared_ptr<B> b_ptr;    
       A()
       {
           cout << "A::A() Constructor" << endl;
       }
       ~A()
       {
           cout << "~A::A Destructor" << endl;
       }
    };
    
    class B {
      public:
       weak_ptr<A> a_ptr;
       B()
       {
           cout << "B::B() Constructor" << endl;
       }
       ~B()
       {
           cout << "~B::B Destructor" << endl;
       }
    };
    
    shared_ptr<A> ptr_2a = make_shared<A>();
    
    shared_ptr<B> ptr_2b = make_shared<B>();
    
    cout << "ptr_2a reference count = " << ptr_2a.use_count() << endl;
    
    cout << "ptr_2b reference count = " << ptr_2b.use_count() << endl;
        
    ptr_2a->b_ptr = ptr_2b;
    
    ptr_2b->a_ptr = ptr_2a;
    
    cout << "Setting:   ptr_2a->b_ptr = ptr_2b;" << endl;
    cout << "Setting:   ptr_2b->a_ptr = ptr_2a;" << endl;
                   
    cout << "ptr_2a reference count now = " << ptr_2a.use_count() << endl;
    
    cout << "ptr_2b reference count now = " << ptr_2b.use_count() << endl;
    
    cout << "ptr_2a->b_ptr reference count = " << ptr_2a->b_ptr.use_count() << endl;
    
    cout << "ptr_2b->a_ptr reference count = " << ptr_2b->a_ptr.use_count() << endl;

Output::

    A::A() Constructor
    B::B() Constructor
    ptr_2a reference count = 1
    ptr_2b reference count = 1
    Setting:   ptr_2a->b_ptr = ptr_2b;
    Setting:   ptr_2b->a_ptr = ptr_2a;
    ptr_2a reference count now = 1
    ptr_2b reference count now = 2
    ptr_2a->b_ptr reference count = 2
    ptr_2b->a_ptr reference count = 1
    ~A::A Destructor
    ~B::B Destructor
    
``weak_ptr<>`` has no dereference of overloaded pointer access methods, no ``T& weak_ptr<T>::operator*()`` or ``T *weak_ptr<T>::operator->()``. Instead it has the method ``lock()`` that returns a ``shared_ptr<>``.
The example below that uses the ``lock()`` method and is taken from https://en.cppreference.com/w/cpp/memory/weak_ptr/lock:

.. code-block:: cpp

    #include <iostream>
    #include <memory>
     
    void observe(std::weak_ptr<int> weak) 
    {
        if (auto observe = weak.lock()) {
            std::cout << "\tobserve() able to lock weak_ptr<>, value=" << *observe << "\n";
        } else {
            std::cout << "\tobserve() unable to lock weak_ptr<>\n";
        }
    }
     
    int main()
    {
        std::weak_ptr<int> weak;
        std::cout << "weak_ptr<> not yet initialized\n";
        observe(weak);
     
        {
            auto shared = std::make_shared<int>(42);
            weak = shared;
            std::cout << "weak_ptr<> initialized with shared_ptr.\n";
            observe(weak);
        }
     
        std::cout << "shared_ptr<> has been destructed due to scope exit.\n";
        observe(weak);
    }
    
Output::

    weak_ptr<> not yet initialized
            observe() unable to lock weak_ptr<>
    weak_ptr<> initialized with shared_ptr.
            observe() able to lock weak_ptr<>, value=42 
    shared_ptr<> has been destructed due to scope exit.
            observe() unable to lock weak_ptr<>

Key Points
----------

* ``weak_ptr<>`` can only be initialized with a ``shared_ptr<>`` or a ``weak_ptr<>``.
* ``weak_ptr<>`` has no dereference of overloaded pointer access methods, no ``T& weak_ptr<T>::operator*()`` or ``T *weak_ptr<T>::operator->()``.   
* ``weak_ptr<>`` does not participate in in the reference count. 
* ``shared_ptr<Ty> lock() const`` must be used to access the referenced object.
* ``weak_ptr<>`` is useful in 1. preventing circular references and in 2. checking whether a ``shared_ptr<>`` dangles. 
* To test if the corresponding ``shared_ptr<>`` dangle use ``weak_ptr<>`` methods ``bool expired() const`` or ``shared_ptr<Ty> lock() const``. 

Articles About weak_ptr<>
-------------------------

* `C++11 Smart Pointer – Part 5: shared_ptr, Binary trees and the problem of Cyclic References <https://thispointer.com/shared_ptr-binary-trees-and-the-problem-of-cyclic-references/>`_.
* `std::weak_ptr on Modernes C++ <http://www.modernescpp.com/index.php/std-weak-ptr>`_. 
* `C++ 11 Library: Weak Pointers <https://www.youtube.com/watch?v=EWoMjuN5OH4>`_
* `C++ / C++11 Smart Pointers : Relationship between shared_ptr and weak_ptr <https://www.youtube.com/watch?v=A9S18kIuIws>`_.
* `When weak_ptr useful <https://stackoverflow.com/questions/12030650/when-is-stdweak-ptr-useful>`_
