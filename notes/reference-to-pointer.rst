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
        *ptr = 11; 
        ptr = &global;
    }
    
    int main(int argc, char** argv) 
    {
        int i = 10;
        int *p = &i;

        cout << "Value of i is: " << i << endl;
        cout << "Value of p is: " << p << endl;

        alter_pointer(p);
        
        cout << "Value of i is: " << i << endl;
        cout << "Value of p is: " << p << endl;
        return 0;
    }
 
The output is:

.. raw:: html

    <pre>
    Value of i is: 10
    Value of p is: 0x7ffd098edbe4
    Value of i is: 11
    Value of p is: 0x560d010f2010
    </pre>    

If we wished to alter the pointer without using a reference, we would have been forced to use a pointer to a pointer.

.. code-block:: cpp

    int global = 99;
    
    void alter_pointer(int **ptr)
    {
        **ptr = 11; 
        *ptr = &global;
    }

    int main(int argc, char** argv) 
    {
        int i = 10;
        int *p = &i;

        cout << "Value of i is: " << i << endl;
        cout << "Value of p is: " << p << endl;

        alter_pointer(&p);
        
        cout << "Value of i is: " << i << endl;
        cout << "Value of p is: " << p << endl;
        return 0;
    }
