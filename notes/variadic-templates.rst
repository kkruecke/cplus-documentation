Variadic Templates
==================

Articles on implementing C++ Variadic Templates:

In addition to variadic function template examples, `Variadic templates in C++ <https://eli.thegreenplace.net/2014/variadic-templates-in-c/>`_ has an excellent example of how ``std::tuple`` can be implemented as a variadic data structure.
As the article explains:

    Variadic templates make it possible to define data structures that could have an arbitrary number of fields, and have this number configured per use. The prime example of this is a tuple class, and here I want to show how to construct one...

The code for the tuple implementation given in the article is at https://github.com/eliben/code-for-blog/blob/master/2014/variadic-tuple.cpp.

`C++11 - New features - Variadic templates <http://www.cplusplus.com/articles/EhvU7k9E/>`_

`Parameter pack(since C++11) <https://en.cppreference.com/w/cpp/language/parameter_pack>`_

`An introduction to C++'s variadic templates: a thread-safe multi-type map <https://jguegant.github.io/blogs/tech/thread-safe-multi-type-map.html>`_
