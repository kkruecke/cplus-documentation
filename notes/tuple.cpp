template <class... Ts> struct Tuple {

	Tuple()
	{
		std::cout << "In base Tuple constructor, which has NO member tail." << std::endl;
	}
}; // <-- This is needed. It will be the final base Tuple of all Tuple<Ts...>'s.

template <class T, class... Ts> struct Tuple<T, Ts...> : Tuple<Ts...> { // public inheritance is the default for structs.


  Tuple(T t, Ts... ts) : Tuple<Ts...>(ts...), tail(t)
  {
	   std::cout << "In constructor of " <<  __PRETTY_FUNCTION__ << " where tail = " << tail << std::endl;
  }

  T tail;
};

For ``tuple<int> t1``,

struct tuple<int> {
 int tail;
}; 

For

tuple<int, string> t2

struct tuple<int, string> : tuple<string> {

 tuple(int t1, string t2) : tuple<string>(t2), tail(t1) {}
 int tail;
};

// where the base tuple<string> is:

struct tuple<string> {
  tuple(string t1) : tail(t1) {}
  string tail;
};

For 

tuple<int, string, double> t3;

struct tuple<int, string, double> : tuple<string, double> {

 tuple(int t1, string t2, double t3) : tuple<string, double>(t2, t3), tail(t1) {}
 int tail;
}; 

// where the base tuple<string, double> is:

struct tuple<string, double> {
  tuple(string t1, double t2) : tuple<double>(t2), tail(t1) {}
  string tail;
}; 

// where struct tuple<double> is:

struct tuple<double> {
  tuple(double t1) : tail(t1) {}
  double tail;
};

//////////

template <size_t, class> struct elem_type_holder; // fwd declar.

/*

struct elem_type_holder is also a recursive data structure. But instead of data members or functions, it contains only a single typedef. Its recursive definition defines the typedef as follows. When the integer template argument is 0, then
'type' is define to be the type of the first template argument. Thus, given

    tuple<int, string, double> t3;

then we have for template argument k == 0, the nested 'type' is nested to be 'int':

  struct elem_type_holder<0, tuple<int, string, double>> { <-- KEY QUESTION TO ANSWER: Should this be 'tuple<double>' instead?!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
  
      using type = int; // Where T is the first class template type of int 
  };


Then for each integer n greater than 0, the nested 'type' typedef if defined by this pattern

For k == 1, we have

   struct elem_type_holder<1, tuple<string, double >> {

     using type = typename elem_type_holder<0, tuple<Ts...>>::type;
   } 

For k == 2, we have

   struct elem_type_holder<2, tuple<int, string, double >> {

     using type = typename elem_type_holder<0, tuple<Ts...>>::type;
   }



*/

template <class T, class... Ts> struct elem_type_holder<0, tuple<T, Ts...>> {
  
  using type = T; // Where T is the first class template type of tuple<T, Ts...> 
};

template <size_t k, class T, class... Ts> struct elem_type_holder<k, tuple<T, Ts...>> {
  /*
   * 'type' is the the 'type' typedef from elem_type_holder<k - 1, tuple<Ts...>>::type
   *
   *
   *
   *
   *
   */
  using type = typename elem_type_holder<k - 1, tuple<Ts...>>::type;
};

template <size_t k, class... Ts>
typename std::enable_if<
    k == 0, typename elem_type_holder<0, tuple<Ts...>>::type&>::type
get(tuple<Ts...>& t) {
  return t.tail;
}

template <size_t k, class T, class... Ts>
typename std::enable_if<
    k != 0, typename elem_type_holder<k, tuple<T, Ts...>>::type&>::type
get(tuple<T, Ts...>& t) {
  tuple<Ts...>& base = t;
  return get<k - 1>(base);
}
