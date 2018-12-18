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

We first start with the example of an overloaded method ``addtolog`` where the desired behavior, the promotion of short to int, does occurs.

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

``add2log(sint);`` results in the expected promotion of the short to an int and the call to ``void add2log(int)``; however, when there is a template version of ``addd2log`` that take a forwarding reference, this
same expected behavior does not occur as shown below. 

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

Now the code no longer compilers. The expected promotion of ``sint`` to an ``int`` no longer occurs because ``sint`` is an exact match for ``template<class T> void add2log(T&& value)``, 
and the compiler therefore instantiates ``void add2log(short& value)``, which results in a call to the non-existant constructor ``string::string(short)`` during the execution of ``log.emplace_back(value)``.

How can we achieve the overloaded behave we really want if template methods with forwarding references can't really be overloaded without producing compile errors like those above?

Solution: tag dispatch
----------------------

To ensure the template function (taking a forwarding reference) does not genearte such compile errors, Edaqa Mortoray's `Overloading the broken universal reference ‘T&&’ <https://mortoray.com/2013/06/03/overriding-the-broken-universal-reference-t/>`_ explains:
"there is no way to avoid redefinition errors with just one parameter, thus we need at least one extra parameter to overload. It would of course be very inconvenient if the caller had to know anything about this.
The solution involves introducing a tag parameter."

First, no overloads of the add2log() are not allowed. Instead various implementations corresponding to each overload are employed. These implementation methods also take the same, identical forwarding reference parameter, but they also have
have a extra parameter that serves as a tag. The type of this secondary tag parameter is determined at run-time. Thus this second parameter must be a template. ``std::decay<class T>``, from header ``<type_traits>`` is used to achieve this.

An example makes this clearer. First, we change add2log() to be an inline method that simply calls the template method ``add2log_impl()``:

.. code-block:: cpp

    template<typename T> struct type_tag {};

    template<typename T>
    void add2log(T&& t) 
    {
       //Get the unqualified type, using std::decay<T>, tor the purpose of tagging.
       class_tag<typename std::decay<T>::type> type_tag;
    
       add2log_impl(std::forward<T>(t), tag);
    }

First note this technique changes ``template<class T> void add2log(T&& value)`` to be an inline function that simply invokes ``template<class T, class Tag>  add2log_impl(T&& t, Tag)``.

.. todo::  Finish commenting and incorporating code below into explanation.

.. code-block:: cpp

    template<class T> struct type_tag { };

    // Two different struct types used for example purposes:
    struct match_a { }; 
    struct match_b { };

    template<class T> 
    void add2log(T&& value) 
    {
        //get the unqualified type for the purpose of tagging
	type_tag<typename std::decay<T>::type> tag;

	add2log_impl( std::forward<T>(f), tag );         
    }

    template<class T, class Tag>
    void add2log_impl(T && f, Tag ) 
    {
        std::cout << f << std::endl;
    }
     
    template<class T> 
    void add2log_impl(T && f, type_tag<match_a>) 
    {
        std::cout << "match_a" << std::endl;
    }
     
    template<class T>
    void add2log_impl(T && f, type_tag<match_b>)
    {
       std::cout << "match_b" << std::endl;
    }
     
    template<class T>
    void add2log_impl(T && f, type_tag<int*>) 
    {
       std::cout << "int*" << std::endl;
    }
    
Example 2:
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
 
.. todo:: Reference to Scott Meyers book and Item #?.      


Problem 2: Overloading a Constructor That Takes Forwarding Reference(s).
------------------------------------------------------------------------


Solution: ``enable_if<T>``
--------------------------

