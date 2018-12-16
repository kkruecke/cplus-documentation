Elimanating Compile Errors Resulting from Overloading Methods Taking Forwarding References
==========================================================================================

Comments. See also:
-------------------

* https://mortoray.com/2013/06/03/overriding-the-broken-universal-reference-t/
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

Now the code no longer compilers. The expected promotion of ``sint`` to an ``int`` no longer occurs because ``sint`` is an exact match for ``template<typename T> void add2log(T&& value)``, 
and the compiler therefore instantiates ``void add2log(short& value)``, which results in a call to the non-existant constructor ``string::string(short)`` during the execution of ``log.emplace_back(value)``.

How can we achieve the overloaded behave we really want if template methods with forwarding references can't really be overloaded without producing compile errors like those above?

Solution: tag dispatch
----------------------

This technique involves providing an inline method that implements ``template<typename T> void add2log(T&& value)`` that takes the same forwarding reference as ``add2log`` along with an extra or **tag**  parameter that is used
to select at compile-time the correct implementation method. This example shows exactly how this is done.

.. todo:: Finish code below:

.. code-block:: cpp

    template<typename T>
    void add2log(T&& value) 
    {
          
    }

Example:

.. todo:: Reference to Scott Meyers book and Item #?.      


Problem 2: Overloading a Constructor That Takes Forwarding Reference(s).
------------------------------------------------------------------------


Solution: ``enable_if<T>``
--------------------------

