From Becker page 6ff.

template<typename T, typename Arg> 
shared_ptr<T> factory(Arg& arg)
{ 
  return shared_ptr<T>(new T(arg));
} 

The problem is that now, the factory function cannot be called on rvalues:
  
factory<X>(hoo()); // error if hoo returns by value
factory<X>(41); // error
  
This can be fixed by providing an overload which takes its argument by const reference:

template<typename T, typename Arg> 
shared_ptr<T> factory(const Arg& arg)
{ 
  return shared_ptr<T>(new T(arg));
} 
  
There are two problems with this approach. Firstly, if factory had not one, but several arguments, you would have to provide overloads for all
combinations of non-const and const reference for the various arguments. Thus, the solution scales extremely poorly to functions with several
arguments.
  
TODO: Illustrate this problem with an example in order to understand it.
  
Secondly, this kind of forwarding is less than perfect because it blocks out move semantics: the argument of the constructor of T in the body of
factory is an lvalue. Therefore, move semantics can never happen even if it would without the wrapping function. 

From /usr/include/c++/5/bits/move.h

/**
   *  @brief  Forward an lvalue.
   *  @return The parameter cast to the specified type.
   *
   *  This function is used to implement "perfect forwarding".
   */

  template<typename _Tp>
    constexpr _Tp&& forward(typename std::remove_reference<_Tp>::type& __t) noexcept
    { return static_cast<_Tp&&>(__t); }

  /**
   *  @brief  Forward an rvalue.
   *  @return The parameter cast to the specified type.
   *
   *  This function is used to implement "perfect forwarding".
   */
  template<typename _Tp>
    constexpr _Tp&&
    forward(typename std::remove_reference<_Tp>::type&& __t) noexcept
    {
      static_assert(!std::is_lvalue_reference<_Tp>::value, "template argument"
		    " substituting _Tp is an lvalue reference type");
      return static_cast<_Tp&&>(__t);
    }

From /usr/include/c++/type_traits

  template<typename>
    struct remove_reference; // forward reference

 // remove_reference defined
  template<typename _Tp>
    struct remove_reference
    { typedef _Tp   type; };

 // remove_reference partial template specializations
  template<typename _Tp>
    struct remove_reference<_Tp&>
    { typedef _Tp   type; };

  template<typename _Tp>
    struct remove_reference<_Tp&&>
    { typedef _Tp   type; };

