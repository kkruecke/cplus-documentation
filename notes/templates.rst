.. include:: <isopub.txt>

Template and Specializations
============================

GNU g++ 7 defines the ``std::vector`` template like this:

.. code-block:: cpp

 template<typename _Tp, typename _Alloc = std::allocator<_Tp> >
    class vector : protected _Vector_base<_Tp, _Alloc>
    {
    //...
    };

It also declares a partial specialization of ``vector<T, Alloc>`` for type booleans. The syntax used is:
Q: What is the difference between a template specialization and a partial template specialization?

.. code-block:: cpp

     template<typename _Alloc>
       class vector<bool, _Alloc> : protected _Bvector_base<_Alloc>
       {
       //... 


     
