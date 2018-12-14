Elimanating Compile Errors Resulting from Overloading Methods Taking Forwarding References
==========================================================================================

Comments. See also:
-------------------

* https://mortoray.com/2013/06/03/overriding-the-broken-universal-reference-t/
* http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2014/n4164.pdf

.. todo:: Finish write up. 

Problem 1: Overloading a Template Function That Takes Forwarding Reference(s).
------------------------------------------------------------------------------


In this example of an overloaded method, the expected promotion of short to int occurs.

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

.. code-block:: cpp

But when there is a template version of addd2log that take a forwarding reference as in this code. 

.. code-block:: cpp

    vector<string> log;
    
    unordered_map<int, string> umap = { {1, "one"}, {2, "two"}, {3, "three"}, {4, "four"} };
    
    const string& look_up_string(int i)
    {
        return umap[i];
    }
    
    template<typename T>
    void add2log(T&& value) 
    {
        cout << "template<typename T> void add2log(T&&) called" << endl;
        log.emplace_back(value);
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

The code no longer compilers. Why does the expected promotion of sint to an int no longer occur? The reason is, the compiler can exactly match 'template<typename T> void add2log(T&& value)'. 
The compiler then instantiates the function 'void add2log(short& value)', which eventually results in a call to the non-existant constructor string::string(short).

How can we then achieve the overloaded behave we really want?

Solution: tag dispatch
----------------------

The technique .....


Example:

Credit Scott Meyers.      


Problem 2: Overloading a Constructor That Takes Forwarding Reference(s).
------------------------------------------------------------------------


Solution: ``enable_if<T>``
--------------------------

Resources
---------

