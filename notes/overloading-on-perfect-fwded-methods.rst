Elimanating Compile Errors Resulting from Overloading Methods Taking Forwarding References
==========================================================================================

References
----------

* `Very Helpful <https://mortoray.com/2013/06/03/overriding-the-broken-universal-reference-t/>`_.
* http://ericniebler.com/2013/08/07/universal-references-and-the-copy-constructo/
* http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2014/n4164.pdf

.. todo:: Finish write up. 

Problem 1: Overloading a Template Function That Takes Forwarding Reference(s).
------------------------------------------------------------------------------

We first start with the example of an overloaded method ``addtolog`` where the expected behavior, the promotion of short to int, occurs.

.. code-block:: cpp

    vector<string> log;
    
    unordered_map<int, string> umap = { {1, "one"}, {2, "two"}, {3, "three"}, {4, "four"} };
    
    const string& look_up_string(int i)
    {
        return umap[i];
    }
    
    void add2log(int i) 
    {
        cout << "void add2log( int i) called" << endl;
        log.emplace_back(look_up_string(i)); 
    }  
    
    void add2log(string& str) 
    {
        cout << "void add2log(string& str) called" << endl;
        log.emplace_back(str); 
    }  

    add2log(1);
    
    short sint = 1;
   
    add2log(sint); 
    
    string str{"abc"};
    
    add2log(str);
     
Ouput::

    void add2log( int i) called
    void add2log( int i) called
    void add2log(string &) called

``add2log(sint);`` results in the expected promotion of short to int in the call to ``void add2log(int)``; however, when there is a template version of ``addd2log`` that takes a forwarding reference, the
expected behavior does not occur: 

.. code-block:: cpp

    vector<string> log;
    
    unordered_map<int, string> umap = { {1, "one"}, {2, "two"}, {3, "three"}, {4, "four"} };
    
    const string& look_up_string(int i)
    {
        return umap[i];
    }
    
    template<class T>
    void add2log(T&& value) 
    {
        cout << "template<class T> void add2log(T&&) called" << endl;
        log.emplace_back(std::forward<T>(value));
    }

    void add2log(int i) 
    {
        cout << "void add2log( int i) called" << endl;
        log.emplace_back(look_up_string(i)); 
    }  
    
    add2log(1);
    
    short sint = 1;
   
    add2log(sint); // <--- Compiler errors. Why?
    
    string str{"abc"};
    
    add2log(str);

This code does not compile. The expected promotion of ``sint`` to an ``int`` no longer occurs because the short ``sint`` is an exact match for ``T&&`` in ``template<class T> void add2log(T&& value)``, 
and the compiler instantiates ``void add2log(short& value)``. Then during the execution of ``log.emplace_back(value)`` a call to the non-extant constructor ``string::string(short)`` causes the compiler error.

How can we achieve the overloaded behavior in the first example if template methods with forwarding references can't be overloaded without producing compile errors like the example above? 

Solution: tag dispatch
----------------------

Edaqa Mortoray's article `Overloading the broken universal reference ‘T&&’ <https://mortoray.com/2013/06/03/overriding-the-broken-universal-reference-t/>`_ explains: "there is no way to avoid redefinition errors with just one parameter, thus we need at least one
extra parameter to overload. It would of course be very inconvenient if the caller had to know anything about this...The solution involves introducing a tag parameter."

First, no overloads of the add2log() are not allowed. Instead various implementations corresponding to each overload are employed. These implementation methods also take the same, identical forwarding reference parameter, but they also have
have a extra parameter that serves as a tag. The type of this secondary tag parameter is determined at run-time. Thus this second parameter must be a template. ``std::decay<class T>``, from header ``<type_traits>`` is used to achieve this.

Original code from Edqua
    
.. code-block:: cpp

    #include <iostream>
    #include <string>
    #include <vector>	
    #include <unordered_map>
    #include <type_traits>
    
    using namespace std;
    
     
    template<typename T>
    struct class_tag {};
     
    template<typename T>
    void apply(T&& t) 
    {
       //Get the unqualified type, using std::decay<T>, tor the purpose of tagging.
       type_tag<typename std::decay<T>::type> tag;
    
       apply_impl( std::forward<T>(t), tag );
    }
     
    template<typename T, typename Tag>
    void apply_impl(T&& t, Tag ) 
    {
       std::cout << t << std::endl;
    }
     
    struct match_a { };
    template<typename T> 
    void apply_impl(T&& t, type_tag<match_a>) 
    {
       std::cout << "match_a" << std::endl;
    }
     
    struct match_b { };
    template<typename T>
    void apply_impl(T&& t, type_tag<match_b>) 
    {
        std::cout << "match_b" << std::endl;
    }
     
    template<typename T>
    void apply_impl(T&& t, class_tag<int*>) 
    {
       std::cout << "int*" << std::endl;
    }
    
    template<typename T>
    void apply_impl(T&& t, class_tag<int>) 
    {
       std::cout << "int" << std::endl;
    }
     
    int main() 
    {	
       apply( 12 );
       apply( "hello" );
       apply( match_a() );
       apply( match_b() );
     
       match_a a;
    
       apply(a);
    
       apply( static_cast<match_a const&>(a) );
    
       apply( static_cast<match_a const>(a) );
     
       int b[5];
    
       apply(b);
    
       apply(static_cast<int*>(b));
       
       apply(9);
            
       return 0;
    }
 
class_tag and ``std::decay`` does provide enough flexibility as a solution to our ``add2log`` example. Instead....we need different way that First, we change add2log() to be an inline method that simply calls the template method ``add2log_impl()``:
First note this technique changes ``template<class T> void add2log(T&& value)`` to be an inline function that simply invokes ``template<class T, class Tag>  add2log_impl(T&& t, Tag)``.


.. todo::  Finish commenting and incorporating code below into explanation.

.. code-block:: cpp

    template<class T> 
    void add2log(T&& value) 
    {
	add2log_impl( std::forward<T>(f), tag );         
    }

.. todo:: Reference to Scott Meyers book and Item #?.      


Problem 2: Overloading a Constructor That Takes Forwarding Reference(s).
------------------------------------------------------------------------


Solution: ``enable_if<T>``
--------------------------

