.. include:: <isopub.txt>

std::variant
============

One of the big downsides of ``unions`` is that you must explicitly invoke the destructor of its active member. Even if you define a destructor for the ``union``, it will not implicitly call the destructors
of the union's member element. It would not know, of course, which element has been most recently set and in need of being destructed. 

Below is a nested union that is used to hold to two types of pairs: one where ``pair<Key, Value>::first`` is const; the other, in which ``pair<Key, Value>::first`` is not const. To make it easier to use, it implements
a move constructor and move assignment constructor, as well as the usual constructors.

.. code-block:: cpp

    template<class Key, Value> class tree234 {

    union KeyValue { 
   
       std::pair<Key, Value>        _pair;  // ...this pair eliminates constantly having to do: const_cast<Key>(p.first) = some_noconst_key;
       std::pair<const Key, Value>  _constkey_pair; 

     public:    
       KeyValue() {} 
      ~KeyValue() 
       {
         _pair.first.~Key();  // Note: Anonymous unions require explicit destructor calls.
         _pair.second.~Value();
       } 
      
       KeyValue(Key key, const Value& value) : _pair{key, value} {}
       
       KeyValue(const KeyValue& lhs) : _pair{lhs._pair.first, lhs._pair.second} {}
       
       KeyValue(Key k, Value&& v) : _pair{k, std::move(v)} {} 
   
       KeyValue(KeyValue&& lhs) :  _pair{move(lhs._pair)} {}
   
       KeyValue& operator=(const KeyValue& lhs) noexcept;  
       KeyValue& operator=(KeyValue&& lhs) noexcept; 

       constexpr Key&  key()  { return _pair.first; }
       
       constexpr const Key& key() const { return _constkey_pair.first; }

       constexpr Value&  value()  { return _pair.second; }
       
       constexpr const Value& value() const { return _constkey_pair.second; }

       constexpr const std::pair<Key, Value>& pair() const { return _pair; }
       constexpr std::pair<Key, Value>& pair() { return _pair; }
               
       constexpr const std::pair<const Key, Value>& constkey_pair() const { return _constkey_pair; }

       constexpr std::pair<const Key, Value>& constkey_pair() { return _constkey_pair; }

       friend std::ostream& operator<<(std::ostream& ostr, const KeyValue& key_value)
       {
          ostr << "{" << key_value._pair.first << ',' <<  key_value._pair.second <<  "}, ";
          return ostr;
       }
    };
    class Node { // Tree node elements. The root is of a managed type std::unique_ptr<Node> 
                 // The Node's children are also of  managed type std::unique_ptr<Node> 
                 // Node contains define outside of tree23
        friend class tree23<Key, Value>;             

     public:   
        Node(Key key, const Value& value, Node *ptr2parent=nullptr);
       ~Node()
        { 
        }

        // We disallow copy construction and assignment...
        Node(const Node&) = delete; 
        Node& operator=(const Node&) = delete; 

        Node(Node&&); // ...but we allow move assignment and move construction.
        Node& operator=(Node&&) noexcept;

        // Constructor for just coping the keys and values. 
	Node(const std::array<KeyValue, 2>& lhs_keys_values, Node * const lhs_parent, int lhs_totalItems) noexcept; 

        constexpr Key& key(int i) { return keys_values[i].key(); }
        constexpr const Key& key(int i) const { return keys_values[i].key(); }

        constexpr Key& value(int i) { return keys_values[i].value(); }
        constexpr const Key& value(int i) const { return keys_values[i].value(); }

        constexpr std::pair<Key, Value>& pair(int i) { return keys_values[i].pair(); }
        constexpr const std::pair<const Key, Value>& pair(int i) const { return keys_values[i].pair(); }

        constexpr bool isLeaf() const noexcept { return (children[0] == nullptr && children[1] == nullptr) ? true : false; } 
        //...snip
          private:

           Node *parent;
           std::array<KeyValue, 2> keys_values;
           std::array<std::unique_ptr<Node>, 3> children;
        //...snip

      }; 

      std::unique_ptr<Node> root;  // root of tree
      // snip....
   }; // end tree234

When a ``tree234<std::string, std::string>`` goes out of scope, ``root`` is implicitly destructor, and thus ``std::unique_ptr<Node>`` calls delete on the raw ``Node`` pointer, which in turn results in calling the 
destructor of each element of type ``KeyValue`` in ``std::array<KeyValue, 3>`` (as well as invoking the destructor for ``std::array<std::unique_ptr<Node>, 3>``, the children, which results in a cascade of recursive
calls, first to the immediate children, then grandchildren, and so on).

However, when ``KeyValue::~KeyValue()`` is invoked, it does not invoke the destructor for the ``std::pair<Key, Value::first`` or ``std::pair<Key, Value::second``, which is why the destructor explicitly must do so:

.. code-block:: cpp

    KeyValue::~KeyValue() 
    {
      _pair.first.~Key();  // Note: Anonymous unions require explicit destructor calls.
      _pair.second.~Value();
     }
 
This is obviously error prone. ``std::variant`` does not have this draw back. For a compelte description of ``std::variant`` and how to use it see `Everything You Need to Know About std::variant from C++17 <https://www.bfilipek.com/2018/06/variant.html>`_. 
