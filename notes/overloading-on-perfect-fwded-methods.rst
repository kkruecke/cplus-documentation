Elimanating Compile Errors Resulting from Overloading Methods Taking Forwarding References
==========================================================================================

.. todo:: Add the complete explanation, with examples, using the topic outline below.

The Two Use Problem Scenarios
-----------------------------

1. Overloading a template free function or class member template method that takes forwarding references.

see https://mortoray.com/2013/06/03/overriding-the-broken-universal-reference-t/

2. Overloading constructors that are template methods taking forwarding references 

SFINAE, which is the theoretical basis for enble_if<>, link: https://www.amazon.com/dp/B01D17IINE/?tag=aqualedlight-20&th=1

See https://mpark.github.io/programming/2014/06/07/beware-of-perfect-forwarding-constructors/

Solution Techiques for Each Scenario 
------------------------------------

1. tag dispatch technique
2. ``enable_if<T>`` template technique

Resources
---------

* http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2014/n4164.pdf
