Consider this series of derived structs, where each struct in the hierarchy has the member variable 'tail':

.. code-block:: cpp

    struct bottom {};
    
    struct A : bottom {
    
        A(const string& s) : tail{s}
        {
        }
        string tail;
    };
    
    struct B : A {

	B(double d, const string& s) : A(s), tail{d}
	{
	}
	double tail;
    };
    
    struct C : B {
	C(int i, double d, const string& s) : tail{i}, B(d, s)
        {
        }
	int tail;
    };

Given an instance of C such as

.. code-block:: cpp
    
    auto i = 5;
    auto d = 10.5;
    auto string s{"hellow world!"}; 

    C c(i, d, s);

How can we access each tail member at each level of the hierarchy?

.. code-block:: cpp
    
    auto i = 5;
    auto d = 10.5;
    auto string s{"hellow world!"}; 

    C c(i, d, s);

    auto x1 = c.tail; // tail is the C::tail

    auto x2 = static_cast<B&>(c).tail; // tail is B::tail

    auto x3 = static_cast<A&>(c).tail; // tail is A::tail

struct elem_type_holder is also a recursive data structure. Rather than containing data members or functions, it contains only a single typedef. Its recursive definition defines the typedef as follows. When the integer template argument is 0, then
'type' is define to be the type of the first template argument. For example, given this tuple

    tuple<int, string, double> t3;

then we have for template argument k == 0, the nested 'type' is nested to be 'int':<-- KEY QUESTION TO ANSWER: Should this be 'tuple<double>' instead?!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

   THE KEY QUESTION TO ANSWER: Should the struct below be 'tuple<double>' or 'tuple<int, string, double>' instead?
   The answer is found in the definitions of the get template functions. To understand it, read Eli Bendersky's article https://eli.thegreenplace.net/2014/variadic-templates-in-c/.
  struct elem_type_holder<0, tuple<double>> { 
  
      using type = int; // Where T is the first class template type of int 
  };

For each integer n greater than 0, the nested 'type' typedef if defined by this pattern

For k == 1, we have

   struct elem_type_holder<1, tuple<string, double >> {

     using type = typename elem_type_holder<0, tuple<double>>::type;
   } 

For k == 2, we have

   struct elem_type_holder<2, tuple<int, string, double >> {

     using type = typename elem_type_holder<0, tuple<Ts...>>::type;
   }


