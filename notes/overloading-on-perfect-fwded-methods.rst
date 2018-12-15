Elimanating Compile Errors Resulting from Overloading Methods Taking Forwarding References
==========================================================================================

Comments. See also:
-------------------

* https://mortoray.com/2013/06/03/overriding-the-broken-universal-reference-t/
* http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2014/n4164.pdf

.. todo:: Finish write up. 

Problem 1: Overloading a Template Function That Takes Forwarding Reference(s).
------------------------------------------------------------------------------

To highlight the problem. We first consider a example of an overloaded method in which the desired behavior, the promotion of short to int, occurs function call resolution.

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

``add2log(sint);`` results in the expected promotion of the short to an int and the call to ``void add2log(int)``. However, when there is a template version of addd2log that take a forwarding reference, this
expected behavior does not occur as in this example. 

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

Now the code no longer compilers. The expected promotion of ``sint`` to an ``int`` no longer occurs because ``sint`` is an exact match for ``template<typename T> void add2log(T&& value)``. 
The compiler then instantiates ``void add2log(short& value)``, which eventually results in a call to the non-existant constructor ``string::string(short)``.

How can we achieve the overloaded behave we really want if template methods with forwarding references can't really be overloaded without producing compile errors like those above?

Solution: tag dispatch
----------------------

The technique .....


Example:

.. todo:: Reference to Scott Meyers book and Item #?.      


Problem 2: Overloading a Constructor That Takes Forwarding Reference(s).
------------------------------------------------------------------------


Solution: ``enable_if<T>``
--------------------------

