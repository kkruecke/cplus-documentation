What were previously called “references” in C++03 are now called “lvalue references” in C++11. This was done to distinguishes them from “rvalue references”, which are new in C++11. lvalue references in C++11 behave just like references did in C++03. On the other hand, rvalue refernces
are new in C++11. They are used primarily in the parameter declarations of move constructors and move assignment operators. Move construction and move assignment significantly improve performance (by "stealing" the resources of the rvalue, when the compiler detects an rvalue). 

In C++11 the concept of what constitutes an rvalue was broadened. What were previously called rvalues in C++03 are not called **prvalues**, and another category of rvalue, xvalues or "expiring values" was introduced. Xvalues result when a lvalue is cast to an rvalue reference or when
a method returns an rvalue reference.
