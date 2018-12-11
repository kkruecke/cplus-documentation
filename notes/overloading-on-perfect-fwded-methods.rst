Elimanating Compile Errors Resulting from Overloading Methods Taking Forwarding References
==========================================================================================

.. todo:: Add the complete explanation, with examples, using the topic outline below.

The Two Use Cases Illustrating the Two Problem Scenarios
--------------------------------------------------------

1. Overloading a template free function or class member template method that takes forwarding references.
2. Overloading constructors that are template methods taking forwarding references 

The Two Techniques for Solving These Two Problem Scenarios 
----------------------------------------------------------

1. tag dispatch technique
2. enable_if<T> template technique

Resources
---------

* http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2014/n4164.pdf
