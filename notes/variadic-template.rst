.. code-block:: cpp

    void print() 
    {  
        cout << "In void print()\n\t";
        cout << endl;  
    }  
      
    template <typename T> void print(const T& t)
    {  
        cout << "In template template <typename T> void print(const T& t)\n\t";
        cout << t << endl;  
    }  
      
    template <typename First, typename... Rest> void print(const First& first, const Rest&... rest)
    {  
        cout << "In template <typename First, typename... Rest> void print(const First& first, const Rest&... rest)\n\t";
    
        cout << first << ", " << endl << "\t";  
        
        print(rest...); // recursive call using pack expansion syntax  
    }  
        
    using namespace std;
    
    int main(int argc, char *argv[])
    {
       print(1); // calls second overload  
     
       // these call the third overload, the variadic template,   
       // which uses recursion as needed.  
       print(10, 20);  
    
       print(100, 200, 300);  
    
       print("first", 2, "third", 3.14159);  
    
       return 0;
    
    }

Variadic Function Templates and Recursion
-----------------------------------------

<https://msdn.microsoft.com/en-us/library/dn439779.aspx> explaines::

    Most implementations that incorporate variadic template functions use recursion of some form, but it's slightly different from traditional recursion. Traditional
    recursion involves a function calling itself by using the same signature. (It may be overloaded or templated, but the same signature is chosen each time.) Variadic
    recursion involves calling a variadic function template by using differing (almost always decreasing) numbers of arguments, and thereby stamping out a different signature
    every time. A "base case" is still required, but the nature of the recursion is different.
