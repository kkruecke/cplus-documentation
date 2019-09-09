.. include:: <isopub.txt>

.. role:: cpp(code)
   :language: cpp

Reference to Pointer
====================

The usefulness of a reference to a pointer
------------------------------------------

A reference to a pointer is a convenient syntax to use when you want to both use a pointer to change the point-to value and also change what the pointer points to. A reference to a pointer can be handly, say, when implementing a linked-list. 

Example
-------

A simple example of how a reference to a pointer can be used to change the pointers address.

.. code-block:: cpp

    int global = 99;
    
    void alter_pointer(int *&ptr)
    {
        ptr = &global;
    }
    
    int main(int argc, char** argv) 
    {
        int i = 10;
        int *p = &i;
        cout << "Value of p is: " << p << endl;

        alter_pointer(p);
        
        cout << "Value of p is: " << p << endl;
        return 0;
    }
 
The output is:

.. raw:: html

    <pre>
    Value of p is: 0x7fffffffe4b4
    Value of p is: 0x555555558010
    </pre>    
