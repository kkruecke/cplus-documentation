.. include:: <isopub.txt>

.. To customize the appearance of inline code, do ``unique_ptr<Node23>`` instead of ``unique_ptr<Node23>``, or simply customize the 'cpp' class in 
.. `_static/css/kurt.css` 

.. role:: cpp(code)
   :language: cpp

Implementing a 2 3 Tree in C++14
================================

The complete source code is at https://github.com/kkruecke/23tree-in-cpp

2 3 Tree Discussions
--------------------

The following sources discuss 2 3 Trees and their algorithms: 

1. `Balanced Trees <http://algs4.cs.princeton.edu/33balanced/>`_ from online book `Algorithms 4th Edition <http://algs4.cs.princeton.edu/home>`_.
2. `Data Structures Balanced Trees <https://www.cse.unr.edu/~mgunes/cs302/Chapter19-BalancedSearchTrees.ppt>`_ from `Univ. of Nevada CS 302 Data Structures <https://www.cse.unr.edu/~mgunes/cs302/>`_. 
3. `Balanced Search Trees <http://www.cs.sfu.ca/CourseCentral/225/ahadjkho/lecture-notes/balanced_trees.pdf>`_ from `Simon Frazer Univ. Data Structure and Programming <http://www2.cs.sfu.ca/CourseCentral/225/ahadjkho/>`_.
4. `2 3 Trees <http://ee.usc.edu/~redekopp/cs104/slides/L19_BalancedBST_23.pdf>`_ from University of Southern California's CSCI 104.
5. `Deletion in 2 3 trees <http://www-bcf.usc.edu/~dkempe/CS104/11-19.pdf>`_ from `USC Data Structure and Object Oriented Design <http://bits.usc.edu/cs104/>`_.
6. `Virgina Tech 2 3 Tree slides <http://courses.cs.vt.edu/cs2606/Fall07/Notes/T05B.2-3Trees.pdf>`_ from `Virgina Tech Data Structures and ObjectOriented Development <http://courses.cs.vt.edu/cs2606/>`_.

Implementation Overview
-----------------------

Nested Union tree23<Key, Value>::KeyValue
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The key and value are stored in a KeyValue object that is a union of two ``std::pair``'s. KeyValue has a move assignement and move constructor to improve the efficiency of the tree insertion
algorithm. Using a unions allows us to write to the ``std::pair<Key, Value>`` member without the need for constantly doing ``const_cast<Key&>(key) = k``, but also allow
the  ``tree23<Key,Value>``'s iterators to return ``pair<const Key, Value>`` references, again without the need to use explicit ``const_cast<>`` throughout the code.

.. code-block:: cpp 

    union KeyValue {
      friend class tree23<Key, Value>;
 
      std::pair<Key, Value>        p1; // write to this one...
      std::pair<const Key, Value>  p2;  // but return this one.
      
      KeyValue() {} 
      KeyValue(Key key, const Value& value) : p1{key, value} {}
      
      KeyValue(const KeyValue& lhs) : p1{lhs.p1.first, lhs.p1.second} {}
      
      KeyValue(Key k, Value&& v) : p1{k, std::move(v)} {} 

      KeyValue(KeyValue&& lhs) : p1{lhs.p1.first, std::move(lhs.p1.second)} {}

      KeyValue& operator=(const KeyValue& lhs);  
      KeyValue& operator=(KeyValue&& lhs); 
      
     friend std::ostream& operator<<(std::ostream& ostr, const KeyValue& key_value)
     {
         ostr << "{" << key_value.p1.first << ',' <<  key_value.p1.second <<  "}, ";
	 return ostr;
     }
    };   

    template<class Key, class Value> inline typename tree23<Key, Value>::KeyValue& tree23<Key, Value>::KeyValue::operator=(typename tree23<Key, Value>::KeyValue&& lhs)
    {
      if (this == &lhs) {
          return *this;
      }
    
      p1.first = lhs.p1.first;
      p1.second = std::move(lhs.p1.second);
    
      return *this; 
    }
     
    template<class Key, class Value> inline  typename tree23<Key, Value>::KeyValue& tree23<Key, Value>::KeyValue::operator=(const typename tree23<Key, Value>::KeyValue& lhs)
    {
      if (this == &lhs) {
          return *this;
      }
    
      p1.first = lhs.p1.first;
      p1.second = lhs.p1.second;
    
      return *this; 
    }

Node23 nested class
^^^^^^^^^^^^^^^^^^^^

2 3 tree nodes are of type ``unique_ptr<Node23>``, where Node23 is the nested class shown below that contains two **std::arrays**: ``std::array<KeyValue, 2> keys_values``
and ``std::array<std::unique_ptr<Node23>, 3> children[]``.  When a Node23 object represents a 2-node, the left subtree of smaller keys is rooted at ``children[0]`` and
the right subtree of larger keys is rooted at ``children[1]``. When a Node23 represent a 3-node, ``children[0]`` is the left subtree, ``children[1]`` the middle subtree
that contains keys greater than ``keys_values[0].key`` but less than ``keys_values[2].key``, and ``children[2]`` is the right subtree containing all keys greater than
`keys_values[2].key``.

.. code-block:: cpp 

    /*
    * The tree is consists of heap-allocated Node23 nodes, each managed by std::unique_ptr<Node23>'s.
    */ 
    class Node23 {

        friend class tree23<Key, Value>;             

     public:   
        Node23(Key key, const Value& value, Node23 *ptr2parent=nullptr);
        Node23(Node4&);
        // We disallow copy construction and assignment...
        Node23(const Node23&) = delete; 
        Node23& operator=(const Node23&) = delete; 

        Node23(Node23&&); // ...but we allow move assignment and move construction.
        Node23& operator=(Node23&&) noexcept;

        // Constructor for just coping the keys and values. 
	Node23(const std::array<KeyValue, 2>& lhs_keys_values, Node23 * const lhs_parent, int lhs_totalItems) noexcept; 

        constexpr bool isLeaf() const noexcept { return (children[0] == nullptr && children[1] == nullptr) ? true : false; } 
        constexpr bool isEmpty() const noexcept { return (totalItems == 0) ? true : false; } 

        constexpr bool isThreeNode() const noexcept { return (totalItems == Node23::ThreeNodeItems) ? true : false; }
        constexpr bool isTwoNode() const noexcept { return (totalItems == Node23::TwoNodeItems) ? true : false; }
        
        constexpr int getTotalItems() const noexcept { return totalItems; }
        constexpr int getChildCount() const noexcept { return totalItems + 1; }
        constexpr std::unique_ptr<Node23>& getNonNullChild() noexcept;

	std::ostream& test_parent_ptr(std::ostream& ostr, const Node23 *root) const noexcept;

        bool siblingHasTwoItems(int child_index, int& sibling_index) const noexcept;

        std::ostream& test_keys_ordering(std::ostream& ostr) const noexcept;

        std::ostream& test_remove_invariant(std::ostream& ostr) const noexcept; 

        std::ostream& test_2node_invariant(std::ostream& ostr, const Node23 *root) const noexcept;

        std::ostream& test_3node_invariant(std::ostream& ostr, const Node23 *root) const noexcept;

        std::ostream& debug_print(std::ostream& ostr, bool show_addresses=false) const;

        std::ostream& print(std::ostream& ostr) const noexcept;
   
        friend std::ostream& operator<<(std::ostream& ostr, const Node23& node23)
        { 
	   return node23.print(ostr);
        }

        private:

           Node23 *parent;

           static const int TwoNodeItems = 1;
           static const int TwoNodeChildren = 2;
           static const int ThreeNodeItems = 2;
           static const int ThreeNodeChildren = 3;
           static const int NotFoundIndex = -1;
               
           std::array<KeyValue, 2> keys_values;

           std::array<std::unique_ptr<Node23>, 3> children;

           void move_keys_values(std::array<std::unique_ptr<KeyValue>, 2>&& lhs);

           void move_children(std::array<std::unique_ptr<Node23>, 3>&& lhs);

           void removeLeafKey(Key key) noexcept;
       
           int totalItems; // set to either Node23::TwoNodeItems or Node23::ThreeNodeItems

           void connectChild(int childIndex, std::unique_ptr<Node23> child)  noexcept;
           void connectChild(std::unique_ptr<Node23>& dest, std::unique_ptr<Node23> src)  noexcept;
          
           void convertTo2Node(Node4&& node4) noexcept; 

           void convertTo3Node(Key key, const Value& value, std::unique_ptr<Node23> pnode23) noexcept; 

           bool NodeDescentSearch(Key value, int& index, Node23 *next) noexcept;          // called during find()  
           bool NodeDescentSearch(Key value, int& index, int& next_child_index) noexcept; // called during insert()

           void insertKeyInLeaf(Key key, const Value& value);
           void insertKeyInLeaf(Key key, Value&& new_value);
    }; 
 

**Note:** Method ``isLeaf()`` checks that both ``children[0]`` and ``children[1]`` are ``nullptr`` since checking only ``children[0]`` is insufficient during ``remove(Key key)``
when a node might have only one subtree that, for example, is rooted at ``children[1]``, while the subtree at ``children[0]`` is ``nullptr``.
  
Node4 nested class
^^^^^^^^^^^^^^^^^^

The nested Node4 class is used during insertion only. Its two constructors automatically sort the keys of its input parameters. When the input parameters are an internal 3-node, 
this particular constructor is used: 

.. code-block:: cpp

    template<class Key, class Value> tree23<Key, Value>::Node4::Node4(Node23 *p3node, Key new_key, const Value& new_value) noexcept : parent{p3node->parent} 

The constructor takes ownership of both ``p3node``'s children and ``heap_2node``. ``child_index`` is used to determine the indecies of each adopted child,
where ``child_index`` is the index of the prior, lower-level 3-node that was processed in the immediately-prior call to ``split()``.

Methods
-------

test\_invariant
^^^^^^^^^^^^^^^

The ``test_invariant()`` methods test both the ordering of the tree as well as the parent pointer in each node. Any invariant violations result in a message following
the display of the node's keys. It calls several ``test_xxx_invariant()`` subroutines of ``Node23`` each of which tests various invariant properties.
 
find(Key key)
^^^^^^^^^^^^^

An iterative algorithm rather than a recursive algorithm is used to search for a key.

.. code-block:: cpp

    template<class Key, class Value> bool tree23<Key, Value>::find(Key key) const noexcept
    {
      if (isEmpty()) {
          return false;
      }
    
      Node23 *current = root.get();
    
      while (current != nullptr) {
          
           auto totalItems = current->totalItems;   
           auto i = 0;
    
           for(; i < totalItems; ++i) {
    
               if (key < current->keys_values[i].p1.first) {
    
                    current = current->children[i].get();
                    break;
    
               } else if (key == current->keys_values[i].p1.first) {
    
                    return true;
           } 
           }
    
           if (i == totalItems) {
    
               current = current->children[totalItems].get(); // key > largest key
           } 
      }
    
      return false;
    }

Iteration of 2 3 Tree
^^^^^^^^^^^^^^^^^^^^^

By Recursion
~~~~~~~~~~~~

Recursive algorithms, like the in-order traversal code below, can be used to traverse the tree in pre order, in order and post order. In the in order traversal method
below a template method that take a functor class instance that overloads the function call operator.

.. code-block:: cpp

    template<class Key, class Value> template<typename Functor> inline void tree23<Key, Value>::inOrderTraverse(Functor f) const noexcept
    {
       DoInOrderTraverse(f, root);
    }
    
    template<class Key, class Value> template<typename Functor> void tree23<Key, Value>::DoInOrderTraverse(Functor f, const std::unique_ptr<Node23>& current) const noexcept
    {
       if (current == nullptr) {
    
          return;
       }
    
       switch (current->getTotalItems()) {
    
          case 1: // two node
                DoInOrderTraverse(f, current->children[0]);
     
                f(current->keys_values[0].p2);
    
                DoInOrderTraverse(f, current->children[1]);
                break;
    
          case 2: // three node
                DoInOrderTraverse(f, current->children[0]);
    
                f(current->keys_values[0].p2);
    
                DoInOrderTraverse(f, current->children[1]);
     
                f(current->keys_values[1].p2);
    
                DoInOrderTraverse(f, current->children[2]);
                break;
       }
    }

There is also a level-order traversal template method that also takes a functor as parameter. In this case, the functor's function call operator must take two arguments:
a ``const Node23&`` and an ``int`` that indicates the current level of the tree.

.. code-block:: cpp
 
    template<class Key, class Value> template<typename Functor> void tree23<Key, Value>::levelOrderTraverse(Functor f) const noexcept
    {
       std::queue< std::pair<const Node23*, int> > queue; 
    
       Node23 *proot = root.get();
    
       if (proot == nullptr) return;
          
       auto initial_level = 1; // initial, top level is 1, the root.
       
       // 1. pair.first  is: const tree<Key, Value>::Node23*
       // 2. pair.second is: current level of tree.
       queue.push(std::make_pair(proot, initial_level));
    
       while (!queue.empty()) {
    
            std::pair<const Node23 *, int> pair_ = queue.front();
    
            const tree23<Key, Value>::Node23 *current = pair_.first;
    
            int current_tree_level = pair_.second;
    
            f(*current, current_tree_level);  
            
            if (current != nullptr && !current->isLeaf()) {
    
                if (current->totalItems == 0) { // This can happen only during remove() when an internal 2-node can become empty temporarily...
    
                       //...when only and only one of the empty 2-node's children will be nullptr. 
                       queue.push( std::make_pair( (current->children[0] == nullptr) ? nullptr : current->children[0].get(), current_tree_level + 1) ); 
                       queue.push( std::make_pair( (current->children[1] == nullptr) ? nullptr : current->children[1].get(), current_tree_level + 1) ); 
    
	        } else {
                
                    for(auto i = 0; i < current->getChildCount(); ++i) {
        
                       queue.push(std::make_pair(current->children[i].get(), current_tree_level + 1));  
                    }
	        }
            }
    
            queue.pop(); 
       }
    }

By External Iterators
~~~~~~~~~~~~~~~~~~~~~
     
Since the predecessor and successor of any key (except the tree's min and maximum) can always be found, an external bidirectional iterator can be supplied. The iterator
maintains a pointer to the current node, the current index into ``keys_values``, and the current iterator state, where state can be ``beg``, ``end``, or ``in_interval``. 
``end`` is a logical sate representing one-past the last element, ``beg`` represents the first key/value pair, and ``in_interval`` is the state of not being at ``end`` or beg, a sort of
the in-between state.

tree23's ``begin()`` calls a constructor that sets position to ``beg``, and it calls ``seekToSmallest()`` to set ``current`` and ``key_index`` to the first key. ``end()``
likewise calls a constructor that sets position to ``end``, and it calls ``seekToLargest()`` to set ``current`` and ``key_index`` to the last key.

If the ``position`` is ``beg``, ``decrement()`` does a no-op, and none of the member varibles changes. If the ``position`` is ``end`` and ``increment()`` is called, it, too,
does a no-op, and none of the member varibles changes. 

.. code-block:: cpp                       

    enum class iterator_position {beg, in_interval, end}; 

    class iterator : public std::iterator<std::bidirectional_iterator_tag, typename tree23<Key, Value>::value_type> { 
                                                 
       friend class tree23<Key, Value>;   

      private:
         tree23<Key, Value>& tree; 

         const typename tree23<Key, Value>::Node23 *current;

         int key_index;  // The parent's child index such that: current == current->parent->children[child_index]

         iterator_position position;

         void initialize(iterator_position pos); // reuseable constructor code. 

         int getChildIndex(const typename tree23<Key, Value>::Node23 *p) const noexcept;

         std::pair<const Node23 *, int> getSuccessor(const Node23 *current, int key_index) const noexcept;

         std::pair<const Node23 *, int> getInternalNodeSuccessor(const typename tree23<Key, Value>::Node23 *pnode, int index_of_key) const noexcept;

         std::pair<const typename tree23<Key, Value>::Node23 *, int>  getLeafNodeSuccessor(const typename tree23<Key, Value>::Node23 *, int) const noexcept;

         std::pair<const Node23 *, int> getPredecessor(const Node23 *current, int key_index) const noexcept;

         std::pair<const Node23 *, int> getInternalNodePredecessor(const typename tree23<Key, Value>::Node23 *pnode, int index) const noexcept;

         std::pair<const Node23 *, int>  getLeafNodePredecessor(const typename tree23<Key, Value>::Node23 *p, int index) const noexcept;

         std::pair<const typename tree23<Key, Value>::Node23 *, int> findLeftChildAncestor() noexcept;

         // called by 
         void seekToSmallest() noexcept;    
         void seekToLargest() noexcept;    

         iterator& increment() noexcept; 

         iterator& decrement() noexcept;

      public:

         explicit iterator(tree23<Key, Value>&); 

         iterator(tree23<Key, Value>& lhs, tree23<Key, Value>::iterator_position);  

         iterator(const iterator& lhs); // What does explicit do?

         iterator(iterator&& lhs); 
 
         bool operator==(const iterator& lhs) const;
         constexpr bool operator!=(const iterator& lhs) const { return !operator==(lhs); }

         constexpr std::pair<const Key, Value>& dereference() noexcept { return const_cast<std::pair<const Key, Value>&>(current->keys_values[key_index].p2); } 

         constexpr const std::pair<const Key, Value>& dereference() const noexcept { return const_cast<const std::pair<const Key, Value>&>( current->keys_values[key_index].p2);} 

         iterator& operator++() noexcept; 
         iterator operator++(int) noexcept;

         iterator& operator--() noexcept;
         iterator operator--(int) noexcept;
         
         std::pair<const Key, Value>& operator*() noexcept { return dereference(); } 

         const std::pair<const Key, Value>& operator*() const noexcept { return dereference(); }
         
         typename tree23<Key, Value>::KeyValue *operator->() noexcept;
    };

    class const_iterator : public std::iterator<std::bidirectional_iterator_tag, const value_type> {
      private:

        iterator iter; 
      public:
         
         explicit const_iterator(const tree23<Key, Value>& lhs);

         const_iterator(const tree23<Key, Value>& lhs, iterator_position pos); 

         const_iterator(const const_iterator& lhs);
         const_iterator(const_iterator&& lhs); 
         const_iterator(const iterator& lhs);

         bool operator==(const const_iterator& lhs) const;
         bool operator!=(const const_iterator& lhs) const;
         
         const_iterator& operator++() noexcept;
         const_iterator operator++(int) noexcept;
         const_iterator& operator--() noexcept;
         const_iterator operator--(int) noexcept;

         const std::pair<const Key,Value>&  operator*() const noexcept 
         {
           return iter.dereference(); 
         } 

         const std::pair<const Key, Value> *operator->() const noexcept { return &this->operator*(); } 
    };

Reverse iterators are also supported by the rbegin() and rend() methods. The source for the methods above is below

.. code-block:: cpp

    // Called by begin()
    template<class Key, class Value> inline tree23<Key, Value>::iterator::iterator(tree23<Key, Value>& lhs_tree) : tree{lhs_tree},\
                                                                current{lhs_tree.root.get()}, key_index{0}
    {
      initialize(iterator_position::beg);
    }
    
    template<class Key, class Value> void tree23<Key, Value>::iterator::initialize(tree23<Key, Value>::iterator_position pos)
    {
      position = pos;
    
      // If the tree is empty, there is nothing over which to iterate...
       if (tree.root.get() == nullptr) {
             
          current = nullptr;
          key_index = 0;
          position = iterator_position::end;
    
      } else if (position == iterator_position::end) {
    
          seekToLargest();  // Go to the largest node, and thus allow decrement() to be called on a non-empty tree.
    
       } else if (position == iterator_position::beg) {
    
          seekToSmallest(); // Go to the smallest node, and thus allow increment() to be called
    
       } else { // any other position value is invalid
    
          throw std::logic_error("iterator constructor called with wrong position paramater");
       }
    }
    
    template<class Key, class Value> inline tree23<Key, Value>::iterator::iterator(const iterator& lhs) : tree{lhs.tree}, current{lhs.current}, \
             key_index{lhs.key_index}, position{lhs.position} 
    {
      initialize(position);
    }
    
    // non const tree23<Key, Value>& passed to ctor. Called only by end()
    template<class Key, class Value> inline tree23<Key, Value>::iterator::iterator(tree23<Key, Value>& lhs_tree, \
                                     typename tree23<Key, Value>::iterator_position pos) : tree{lhs_tree}, position{pos} 
    {
      initialize(position);
    }
    
    template<class Key, class Value> inline typename tree23<Key, Value>::iterator tree23<Key, Value>::begin() noexcept
    {
      return iterator{*this, iterator_position::beg};
    }
    
    template<class Key, class Value> inline typename tree23<Key, Value>::const_iterator tree23<Key, Value>::begin() const noexcept
    {
      return const_iterator{*this, iterator_position::beg};
    }
    
    /*
     end() calls the iterator constructor that sets current to nullptr and key_index to 0. 
     */
    
    template<class Key, class Value> inline typename tree23<Key, Value>::iterator tree23<Key, Value>::end() noexcept
    {
       return iterator(const_cast<tree23<Key, Value>&>(*this), iterator_position::end);
    }
    
    template<class Key, class Value> inline typename tree23<Key, Value>::const_iterator tree23<Key, Value>::end() const noexcept
    {
       return const_iterator(const_cast<tree23<Key, Value>&>(*this), tree23<Key, Value>::iterator_position::end);
    }
    
    template<class Key, class Value> inline typename tree23<Key, Value>::reverse_iterator tree23<Key, Value>::rbegin() noexcept
    {
       return reverse_iterator{ end() }; 
    }
    
    template<class Key, class Value> inline typename tree23<Key, Value>::const_reverse_iterator tree23<Key, Value>::rbegin() const noexcept
    {
        return const_reverse_iterator{ end() }; 
    }
    
    template<class Key, class Value> inline typename tree23<Key, Value>::reverse_iterator tree23<Key, Value>::rend() noexcept
    {
        return reverse_iterator{ begin() }; 
    }
    
    template<class Key, class Value> inline typename tree23<Key, Value>::const_reverse_iterator tree23<Key, Value>::rend() const noexcept
    {
        return const_reverse_iterator{ begin() }; 
    }
    /*
     Moves to first, smallest node in tree.
     Sets:
     1. current to smallest node
     2. key_index to 0
     3. position is set to value passed 
     */
    template<class Key, class Value> void tree23<Key, Value>::iterator::seekToSmallest() noexcept
    {
      if (position != iterator_position::beg) {
    
          throw std::logic_error("iterator constructor called with wrong position paramater");
      }
    
      for (const Node23 *cursor = tree.root.get(); cursor != nullptr; cursor = cursor->children[0].get()) {
               current = cursor;
      }
    
      key_index = 0;
    }
    
    template<class Key, class Value> inline void tree23<Key, Value>::iterator::seekToLargest() noexcept
    {
      if (position != iterator_position::end) {
    
          throw std::logic_error("iterator constructor called with wrong position paramater");
      }
    
      for (const Node23 *cursor = tree.root.get(); cursor != nullptr; cursor = cursor->children[cursor->totalItems].get()) {
               current = cursor;
      }
    
      key_index = (current->isThreeNode()) ? 1 : 0;
    }
    
    template<class Key, class Value> inline tree23<Key, Value>::iterator::iterator(iterator&& lhs) : \
                 tree{lhs.tree}, current{std::move(lhs.current)}, key_index{std::move(lhs.key_index)}, position{std::move(lhs.position)} 
    {
       lhs.current = nullptr; // set to end
       lhs.key_index = 0;
       lhs.position = iterator_position::end;
    }
    /*
    
     */
    template<class Key, class Value> bool tree23<Key, Value>::iterator::operator==(const iterator& lhs) const
    {
     if (&lhs.tree == &tree) {
    
        // If we are not in_interval...
        if (lhs.position == iterator_position::end && position == iterator_position::end) { // check whethert both iterators are at the end...
    
            return true;
    
        } else if (lhs.position == iterator_position::beg && position == iterator_position::beg) { // ...or at beg. 
    
            return true;
    
        } else if (lhs.position == position && lhs.current == current && lhs.key_index == key_index) { // else check whether position, current and key_index
                                                                                                       // are all equal.
            return true;
       }
     }
     
     return false;
    }
    
    /*
     int getChildIndex(Node23 *pnode)
    
     Requires: pnode is a node in the tree for which we want child_index such that
    
          current->parent->children[child_index] == current
    
     Returns: child_index as shown above. 
     */
    
    template<class Key, class Value> int tree23<Key, Value>::iterator::getChildIndex(const typename tree23<Key, Value>::Node23 *p) const noexcept
    {
      // Determine child_index such that current == current->parent->children[child_index]
      int child_index = 0;
    
      for (; child_index <= current->parent->totalItems; ++child_index) {
    
           if (current == current->parent->children[child_index].get())
                  break;
      }
    
      return child_index;
    }
    
    /*
    Two cases have to be considered: when current is an internal node and when current is a leaf node.
    
    If current is a leaf node, we check if it is a 3-node. If it is, and if key_index is 0, the predecessor is the second
    key of current. However, if key_index is 0, then we ascend the parent nodes as long as the parent is the right-most
    child (of its parent). If we reach the root, there is no predecessor.
    
    Else upon reaching a parent (before the root) that is a middle or left-most child (of its parent), we find the smallest key in the parent's "right" subtree.
     */
    template<class Key, class Value> std::pair<const typename tree23<Key, Value>::Node23 *, int> tree23<Key, Value>::iterator::getPredecessor(const typename  tree23<Key, Value>::Node23 *current, int key_index) const noexcept
    {
      if (current->isLeaf()) { // If leaf node
    
         if (current == tree.root.get()) { // root is leaf      
    
             if (tree.root->isThreeNode() && key_index == 1) {
    
                 return std::make_pair(current, 0);
             }
                      
             return std::make_pair(nullptr, 0);
                
         } else {
    
            return getLeafNodePredecessor(current, key_index);
         }
    
      } else { // else internal node
    
          return getInternalNodePredecessor(current, key_index);
      }
    }
    
    template<class Key, class Value> std::pair<const typename tree23<Key, Value>::Node23 *, int> tree23<Key, Value>::iterator::getInternalNodePredecessor(\
         const typename tree23<Key, Value>::Node23 *pnode, int index_of_key) const noexcept	    
    {
     // Get next left child node of pnode based on key_index.
     const Node23 *leftChild;
    
     if (pnode->isThreeNode()) {
    
          if (index_of_key == 0) {
    
            leftChild = pnode->children[0].get();
    
          } else { // it is 1     
    
            leftChild = pnode->children[1].get(); // middle child 
          }
    
     } else { // It is a 2-node
    
            leftChild = pnode->children[0].get();
     }
    
     for (const Node23 *cursor = leftChild; cursor != nullptr; cursor = cursor->children[cursor->totalItems].get()) {
    
        pnode = cursor;
     }
    
     return std::make_pair(pnode, pnode->totalItems - 1); 
    }
    /* 
    Finding the predecessor of a given node 
    ---------------------------------------
    
      Pseudo code and illustration is at From http://ee.usc.edu/~redekopp/cs104/slides/L19_BalancedBST_23.pdf slides #7 and #8
    
      If left child exists, predecessor is the right most node of the left subtree
    
      Else we walk up the ancestor chain until you traverse the first right child pointer (find the first node that is a right child of its 
      parent...that parent is the predecessor)
    
      If you get to the root w/o finding a node that is a right child, there is no predecessor
    */
    
    template<class Key, class Value> std::pair<const typename tree23<Key, Value>::Node23 *, int> tree23<Key, Value>::iterator::getLeafNodePredecessor(const typename tree23<Key, Value>::Node23 *pnode, int index) const noexcept
    {
      // Handle trivial case: if the leaf node is a 3-node and key_index points to the second key, simply set key_index to 0. 
      if (pnode->isThreeNode() && index == 1) {
    
          return std::make_pair(current, 0); 
      }
    
      // Determine child_index such that pnode == pnode->parent->children[child_index]
      int child_index = getChildIndex(pnode);
    
      int pred_key_index;
    
       /*
       Handle easy cases first:
       1. If child_index is 2, then the predecessor -- when pnode is either a 2-node of 3-node -- is pnode->parent and the pred_key_index is the parent's right most key.
       2. If child_index is 1 and parent is a 3-node, the predecessor is pnode->parent and pred_key_index is 0.
       */
      switch (child_index) {
    
          case 2:
                  /*
                     We know that is pnode is a 3-node, then key_index is 0 because if it was 1, this was already handled at the start of this method.
                     If pnode is a 2-node, then obviously, key_index is also 0. So in either case, pnode is the right most child of its parent, so the 
                     predecessor will be the right most key of the parent.
                   */
             pnode = pnode->parent;
    
             pred_key_index = pnode->isThreeNode() ? 1 : 0;               
             break;
    
       case 1: /* 
                pnode is either the right child of a 2-node or the middle child of a 3-node. If pnode is a 3-node, key_index equals 0 because if it was a 1, it was
                already handled at the start of this method.  The possibilities look like this
    
                 If parent is a 2-node, there are two possible cases: 
    
                  (a)   [20]    (b)   [20]
                        /  \          /  \  
                      [x]  [30]    [x]  [30, 32]
    
                 In (a), key_index is 0. In (b), key_index is also 0.   
    
                 If parent is a 3-node, there are two possible cases: 
    
                  (c)   [20,  40]    (d)   [20,   40]
                        /   |   \         /    |     \ 
                      [x]  [30] [y]     [x] [30, 32]  [y] 
                 
                  In (c) above, key_index is 0. In (d), key_index is also 0. 
    
                  So in all four cases, the predecessor is the first key of the parent, which is 20.
                  */ 
    
                pnode = pnode->parent;
                pred_key_index = 0;
                break;
    
        case 0: 
       /* 
        The possibilites for this case are: 
    
           (a)   [20]    (b)   [20]       (c)   [20,   40]     (d)   [20,   40]        
                 / \           /  \            /    |    \          /    |    \        
              [15]  [x]   [15, 19] [x]    [15, 19] [x]   [y]     [15]   [x]   [y] 
    
        In (a), pnode is [15]. In (b), pnode is [15, 19] and key_index is 0. In (c), pnode is [15] and key_index is 0. In (d), pnode is also [15] and key_index of 0.
    
        In all four cases, to find the next largest node the logic is identical.
    
        We walk up the ancestor chain until we traverse the first right child pointer, that is, we find the first descendent node that is a "right" child of its parent.
        That parent is the predecessor. If we get to the root without finding a node that is a right child, there is no predecessor.
    
        Note: In a 2 3 tree, a "right" child pointer will be either the second child of a 2-node or the second, the middle, or the third child of a 3-node. "right" child
        pointer means a pointer to a subtree with larger keys. In a 2 3 tree, the middle child pointer of a 3-node parent is a "right child pointer" of the 1st key
        because all the keys of the subtree whose root is the second (or middle) child pointer are greater than 1st key of the subtree's parent. 
    
        So when we walk up the ancestor chain as long as the parent is the first child, which this loop does:
    
           const Node23 *__parent = pnode->parent;
           const Node23 *prior_node  = pnode;
    
           while (pnode == __parent->children[0].get()) { // As long as the parent is the first child of its parent. 
    
                  // pnode is still the right most child but now its parent is the root, therefore there is no predecessor. 
                  if (__parent == tree.root.get()) {
                      
                      return std::make_pair(nullptr, 0);  // Because pnode is still the right most child of its parent it has no predecessor.
                                                          // To indicate this we set current to nullptr and key_index to 0.
                  }
    
                  prior_node = pnode;
                  pnode = __parent; 
                __parent = pnode->parent; 
           }
               
           prior_node_node = pnode; 
           pnode = __parent;
    
        The loops stops when we find a child pointer that is not the first child pointer of its parent. If we get to the root without finding a non-first child pointer,
        there is no predecessor. For example, in the tree portion shown below
    
                  [5,   10]  
                  /   |   \                              
              ...    ...  [27,       70]  
                           /       |     \
                          /        |      \   
                       [20]       [45]    [80, 170]
                       /   \      /  \     /  |  \
                    [15]  [25]  [30] [60]  <-- pnode points to leaf node [20]. 
                    / \   / \   / \  / \   
                   0   0 0   0 0   0 0  0  ... 
         
        if [15] is the pnode leaf node, the predecessor of [15] is the second key of the 3-node [5, 10] because when we walk up the parent chain from [15], the first
        right child pointer we encounter is the parent of [27, 70], which is [5, 10]. So [10] is the next smallest key. In this example
    
                  [5,   10]  
                  /   |   \                              
              ...    ...  [27,       70]  
                           /       |     \
                          /        |      \   
                       [20]       [45]     [80, 170]
                      /   \       /  \      /  |  \
                    [15]  [25]  [30] [60]  <-- pnode points to leaf node [20]. 
                    / \   / \   / \  / \   
                   0   0 0   0 0   0 0  0  ... 
         
          if [30] is the pnode leaf node, the predecessor of [30] is the first key of the 3-node [27, 70] because when we walk up the parent chain from [30], the first
          non-first child pointer we encounter is the parent of [45], which is [27, 70]. So the key at index 0, which is [27], is the next smallest key.
    
          Therefore, if our loop above terminates without encountering the root, we must determine the child index of prior_node in pnode. If pnode is a 2-node, it is 
          trivial: the child index is one. If pnode is a three node, the child index is either one or two:
    
          int child_index = 1; // assume pnode is a 2-node.
    
          if (pnode->isThreeNode()) { // if it is a 3-nodee, compare prior_node to children[1]
    
              child_index = prior_node == pnode->children[1].get() ? 1 : 2;
          }
      
          Now that we know the child_index such that
    
                pnode->children[child_index] == prior_node;
          
          Determine which key is the predecessor. If child_index is one, the middle child, then the predecessor is pnode->keys_values[0]. If child_index is two, then
          the predecessor is pnode->keys_values[1]. Thus, the predecessor is the key at child_index - 1, or:
    
          int predecessor = pnode->keys_values[child_index - 1]; 
          return std::make_pair(pnode, predecessor);
          */
            {
              const Node23 *__parent = pnode->parent;
              const Node23 *prior_node = pnode;
              
              // Ascend the parent pointer as long as pnode is the left most child of its parent.
              while(pnode == __parent->children[0].get() )  {
              
                  // pnode is still the left most child but now its parent is the root. 
                  // Question: Can the root ever be the predecessor? Is this test correct or not? See todo.md.
                  if (__parent == tree.root.get()) {
                      
                        
                      return std::make_pair(nullptr, 0);  // Because pnode is still the right most child of its parent it has no predecessor.
                                                          // To indicate this we set current to nullptr and key_index to 0.
                  }
              
                  prior_node = pnode;
                  pnode = __parent;
                  __parent = __parent->parent;
              }
              
              prior_node = pnode; 
              pnode = __parent;
    
              int child_index; 
    
              child_index = prior_node == pnode->children[1].get() ? 1 : 2; // If pnode is a 2-node, then prior_node will always be pnode->children[1].get().
                                                                            // If pnode is a 3-node, then we must compare prior_node to  pnode->children[1].get().
                                                                            // If prior_node is not the second child, it must be the third child.
    
              // Using child_index, we know the key of the next smallest key will be child_index -1.
    
              return std::make_pair(pnode, child_index - 1);
          }
          break;
     
        default:
           break;
    
      } // end switch
    
      return std::make_pair(pnode, pred_key_index); 
    }
    
    /*
    
    Finding the successor of a given node 
    -------------------------------------
    
    Requires:
    
    1. If position is beg, current and key_index MUST point to first key in tree. 
    
    2. If position is end,  current and key_index MUST point to last key in tree.
      
    3. If position is in_interval, current and key_index do not point to either the first key in the tree or last key in tree. If the tree has only one node,
    the state can only be in_interval if the first node is a 3-node.
    
    Returns:
    
    pair<const Node23 *, int>, where the Node23 pointer is the node with the next key and value in in-order sequence. key_index is the index into
    Node23::keys_values[].  If the last key has already been visited, the pointer returned will be nullptr.
    
    Questions: Will position ever be end or beg, or do the callers increment() and decrement() ensure that it is never end or beg?
    
    pseudo code for getting successor from: http://ee.usc.edu/~redekopp/cs104/slides/L19_BalancedBST_23.pdf
    
    If left child exists, predecessor is the right most node of the left subtree. Internal node's of a 2 3 tree always have a right branch because 2 3 trees are
    balanced.
    
    Else walk up the ancestor chain until you traverse the first right child pointer (find 
    the first node who is a right child of his parent...that parent is the predecessor)
    
    If you get to the root w/o finding a node who is a right child, there is no predecessor
    
    May set:
    1. current
    2. key_index
    3. position
     */
    template<class Key, class Value> std::pair<const typename tree23<Key, Value>::Node23 *, int> tree23<Key, Value>::iterator::getSuccessor(const Node23 *current,\
                                                                                                               int index_of_key) const noexcept
    {
      if (current->isLeaf()) { // If leaf node
         
         if (current == tree.root.get()) { // root is leaf      
    
             if (tree.root->isThreeNode() && key_index == 0) {
    
                 return std::make_pair(current, 1);
             }
                      
             return std::make_pair(nullptr, 0);
     
         } else {
    
            return getLeafNodeSuccessor(current, index_of_key);
         }
    
      } else { // else internal node
    
          return getInternalNodeSuccessor(current, index_of_key);
      }
    }
    
    /* 
       Requires:
       1. pnode is an internal node not a leaf node.
       2. If pnode is a 3-node, key_index is 1 not 0.
    
       Returns:
       pointer to successor node.
    
    Note: When a 2 3 tree node is a 3-node, it has two "right" chidren from the point of view of its first key and two "left" children from the point of view of its
    second key.
     */
    template<class Key, class Value> std::pair<const typename tree23<Key, Value>::Node23 *, int> tree23<Key, Value>::iterator::getInternalNodeSuccessor(const typename tree23<Key, Value>::Node23 *pnode, \
     int index_of_key) const noexcept	    
    {
     // Get next right child node of pnode based on key_index.
     const Node23 *rightChild;
    
     if (pnode->isThreeNode()) {
    
          if (index_of_key == 0) {
    
            rightChild = pnode->children[1].get();
    
          } else { // it is 1     
    
            rightChild = pnode->children[2].get(); 
          }
    
     } else { // It is a 2-node
    
            rightChild = pnode->children[1].get();
     }
    
     // Question: Does it take into account that fact that a node may have already been visited in order?
     // Get the smallest node in the subtree rooted at the rightChild, i.e., its left most node...
     for (const Node23 *cursor = rightChild; cursor != nullptr; cursor = cursor->children[0].get()) {
    
        pnode = cursor;
     }
    
     return std::make_pair(const_cast<Node23 *>(pnode), 0);
    }
    /*
     Requires:
     1. pnode is a leaf node, either a 2 or 3-node
     2. If pnode is 3-node, then key_index, the key index into pnode->keys_values[].p1.first must be 1, the second key. It can never be 0, the first key.
    
     */
    template<class Key, class Value> std::pair<const typename tree23<Key, Value>::Node23 *, int> tree23<Key, Value>::iterator::getLeafNodeSuccessor(const \
     typename tree23<Key, Value>::Node23 *pnode, int index_of_key) const noexcept
    {
      // If the leaf node is a 3-node and key_index points to the first key, this is trivial: we simply set key_index to 1. 
      if (pnode->isThreeNode() && index_of_key == 0) {
    
          return std::make_pair(current, 1); 
      }
    
      // Determine child_index such that pnode == pnode->parent->children[child_index]
      int child_index = getChildIndex(pnode); // BUG: This blows up if pnode == root.get())
    
      int suc_key_index;
    
      /*
       Handle easy cases first:
       1. If child_index is 0, then the successor -- when pnode is either a 2-node of 3-node -- is pnode->parent and the suc_key_index is 0.
       2. If child_index is 1 and parent is a 3-node, the successor is pnode->parent and suc_key_index is 1.
       */
      switch (child_index) {
    
          case 0: /*
                 pnode is either the left most child of either a 2-node or 3-node parent. If pnode is a 3-node, its key_index equals 1 (because if it is was 0,
                 this was already handled at the beginning of this method. 
                 The possibilities are:
    
                (a)   [20]       (b) [20]    (c)   [20, 40]       (d) [20,  40]    
                      / \            /  \          /   |  \            /   |  \ 
                   [15]  [x]    [15, 18] [x]    [15]  [ ]  [ ]   [15, 18] [ ] [ ]   Note: if leaf is a 3-node, key_index is 1.
    
              In all four scenarios above, we advance to the first key of the parent. 
                   */
             pnode = pnode->parent;
             suc_key_index = 0;
             break;
     
          case 1: /* 
                pnode is either the right child of a 2-node or the middle child of a 3-node. If pnode is a 3-node, key_index equals 1 because if it was a 0, it was
                already handled at the start of this method.  The possibilities look like this
    
                 If parent is a 2-node, there are two possible cases: 
    
                  (a)   [20]    (b)   [20]
                        / \           /  \  
                      [x]  [30]    [x]  [30, 32]
    
                 In (a), key_index is 0. In (b), key_index is 1.   
    
                 If parent is a 3-node, there are two possible cases: 
    
                  (c)   [20,  40]    (d)   [20,   40]
                        /   |   \         /    |    \ 
                      [x]  [30] [ ]     [x]  [30,32] [ ] 
                 
                  In (c) above, key_index is 0. In (d), key_index is 1. 
                   
                  */ 
             if (pnode->parent->isThreeNode()) { // This is the trivial case, we advance to the 2nd key of the parent 3-node. 
    
                pnode = pnode->parent;
                suc_key_index = 1;
                break;
             } 
    
             /* If the parent is a 2-node, we fall through to 'case 2' */
        case 2: 
       /* 
        The possibilites for this case are: 
    
           (a)   [20]    (b)   [20]       (c)   [20,   40]     (d)   [20,   40]        
                 / \           /  \            /    |    \          /    |    \        
               [x]  [30]    [x]  [30, 32]    [ ]   [ ]   [50]     [ ]   [ ]   [50, 60] 
    
        In (a), pnode is [30]. In (b), pnode is [30, 32] and key_index is 1. In (c), pnode is [50]. In (d), pnode is [50, 60] and key_index of 1.
    
        In all four cases, the logic is identical. We walk up the ancestor chain until we traverse the first left child pointer, that is, we find the first node that is
        a left child of its parent. That parent is the successor. If we get to the root without finding a node that is a left child, there is no successor.
    
        Note: In a 2 3 tree, a "left child pointer" isn't always the first child. A "left child pointer" simply means a pointer to a subtree with smaller values than
        the parent. In a 2 3 tree, the middle child pointer of a 3-node parent is a "left child pointer" of the 2nd key because all the values of the subtree rooted at
        the middle child are less than the 2nd key of the middle child's parent 3-node. 
    
        So when we walk up the ancestor chain, we stop when we find a child pointer that is not the right most child pointer of its parent. If we get to the root without
        finding a non-right most child pointer, there is no successor. For example, in the tree portion shown below
    
                      [17,       60]   <-- 3-node
                      /       |     \
                     /        |      \
                  [10]       [35]     [70, 100]
                 /   \       /  \      /  |  \
               [5]  [15]   [20] [50]  <-- pnode points to leaf node [50]. 
               / \   / \   / \  / \   
              0   0 0   0 0   0 0  0  ... 
    
          In the tree above, if [50] is the pnode leaf node, the successor of [50] is the second key of the 3-node [17, 60]. When we walk up the parent chain from [50],
          the first left child pointer we encounter is the middle child of the 3-node [17, 60], which is the "left" child of 60. So [60] is the next largest key.
    
          The same logic applies to all four possilbe cases (a) through (d). For example, for case (b), illustrate in the tree below
     
                      [17,            60]   <-- 3-node
                      /       |         \
                     /        |          \
                  [10]       [35]        [70, 100]
                 /   \       /  \        /  |  \
               [5]  [15]   [20] [50, 55]             <-- pnode points to key 55 in leaf node [50, 55]. 
               / \   / \   / \  / \   
              0   0 0   0 0   0 0  0  ... 
        
          again, 60 is the successor by applying the same reasoning.
    
          */
            {
               const Node23 *prior_node = pnode;
               const Node23 *__parent = pnode->parent;
               
               // Ascend the parent pointers as long as pnode is the right most child of its parent.
               while(pnode == __parent->children[__parent->totalItems].get() )  {
               
                   // pnode is still the right most child but now its parent is the root, therefore there is no successor. 
                   if (__parent == tree.root.get()) {
                      
                       return std::make_pair(nullptr, 0);  // Because pnode is still the right most child of its parent it has no successor.
                                                           // To indicate this we set current to nullptr and key_index to 0.
                   }
               
                   prior_node = pnode;
                   pnode = __parent;
                   __parent = __parent->parent;
               }
               
               prior_node = pnode; 
               pnode = __parent;
               
               // If pnode is a 3-node, determine if we ascended from the first child, children[0], or the middle child, children[1], and set suc_key_index accordingly. 
               if (pnode->isThreeNode()) {
    
                  suc_key_index = (prior_node == pnode->children[0].get()) ? 0 : 1; 
    
               } else { // pnode is a 2-node
    
                  suc_key_index = 0;
               }
             }
             break;
    
        default:
           break;
    
      } // end switch
    
      return std::make_pair(pnode, suc_key_index); 
    }
    
    template<class Key, class Value> inline typename tree23<Key, Value>::iterator& tree23<Key, Value>::iterator::increment() noexcept	    
    {
      if (tree.isEmpty()) {
    
         return *this;  // If tree is empty, do nothing.
      }
    
      switch (position) {
    
         case iterator_position::end:
    
               // no-op for increment. current and key_index still point to largest key/value in tree
               break;
          
         case iterator_position::beg:
         case iterator_position::in_interval:
         {
               std::pair<const Node23 *, int> pair = getSuccessor(current, key_index);
    
               if (pair.first == nullptr) { // nullptr implies there is no successor. Therefore current and key_index already pointed to last key/value in tree.
    
                    // There current doesn't change, nor key_index, but the state becomes 'end', one-past last key. 
                    position = iterator_position::end;
    
               } else if (current == pair.first) {
    
                    key_index = pair.second; // current has no change, but key_index has.
      
               } else {  // curent has changed, so we adjust current and key_index. To ensure position is no longer 'beg', we unconditionally set position to 'in_interval'.
    
                   current = pair.first;
                   key_index = pair.second;
                   position = iterator_position::in_interval; 
               }
         }
               break;
         default:
               break;
       }
    
       return *this;
    }
    
    template<class Key, class Value> typename tree23<Key, Value>::iterator& tree23<Key, Value>::iterator::decrement() noexcept	    
    {
      if (tree.isEmpty()) {
    
         return *this; 
      }
    
      switch (position) {
    
       case iterator_position::beg:
         // no op. Since current and key_index still point to smallest key and its value., we don't change them. 
         break;
    
       case iterator_position::in_interval: // 'in_interval' means current and key_index range from the second key/value in tree and its last key/value.
                                            // 'in_interval' corresponds to the inclusive half interval [second key, last key), while 'beg' refers only to
                                            //  first key/value.  
        {      
           std::pair<const Node23 *,int> pair = getPredecessor(current, key_index); // returns current and key_index of predecessor
    
           if (pair.first == nullptr) { // If nullptr, there is no successor: current and key_index already point to the first key/value in the tree. 
    
                // Therefore current doesn't change, nor key_index, but the state becomes 'beg'. 
                position = iterator_position::beg;
    
           } else if (current == pair.first) { 
    
                key_index = pair.second;  // current hasn't change, key_index may have, so we set it.
    
           } else { // Current changed, so we update both current and key_index
    
               current = pair.first;
               key_index = pair.second;
           }
        }
        break;
    
       case iterator_position::end:
    
            // current and key_index already point to last key/value, so we merely change the position state to indicate they are 'in_interval'.
            position = iterator_position::in_interval;
            break;
    
       default:
            break;
     }
    
     return *this;
    }
        
Insertion
^^^^^^^^^

To best understand the algorithm, it helps to follow the Insertion slides examples at `Data Structures Balanced Trees <https://www.cse.unr.edu/~mgunes/cs302/Chapter19-BalancedSearchTrees.ppt>`_.  
The insertion algorithm is based on the pseudo code in slides 25 and 26, along with the 4-node technique discussed in `Balanced Trees <http://algs4.cs.princeton.edu/33balanced/>`_.

Insertion begins at the leaf node where the insertion search terminates. As the algorithm descends the tree to the leaf node, the index of each child
branch taken is pushed onto a ``stack<int>``.  If the leaf is a 2-node, we simply insert the new key and its associated value into the leaf, and we are done. However, if
the leaf where the insertion is to begin is a 3-node, as is the case in slide #17 of https://www.cse.unr.edu/~mgunes/cs302/Chapter19-BalancedSearchTrees.ppt, when 38 is
inserted. 

To handle this case, we need to split the 3-node.

split method
~~~~~~~~~~~~

`split(Node23 *p3node, Key new_key, const Value& new_value, std::stack<int>& child_indecies, std::unique_ptr<Node23> heap_2node)`` is passed four parameters: 

1. a 3-node leaf pointer (which is always a leaf node when invoked by ``insert()``)
2. the new key and value
3. the stack of child indecies of the child branches taken descending the tree to the leaf node
4. and an rvalue ``unique_ptr<Node23>`` whose underlying pointer is nullptr. 
   
Neither the stack nor the unique_ptr<Node23> are used when the first parameter is a leaf node. 

`split()`` first creates a 4-node, whose constructor automatically sorts the keys of ``p3node`` and ``new_key``. It sets all four children to nullptr:

.. code-block:: cpp

    template<class Key, class Value> void tree23<Key, Value>::split(Node23 *p3node, Key new_key, const Value& new_value, std::stack<int>& child_indecies, \
        std::unique_ptr<Node23> heap_2node)  noexcept
    {
      // get the actual parent              
      Node23 *parent = p3node->parent;
      
      // Create 4-node on stack that will aid in splitting the 3-node that receives new_key (and new_value).
      Node4 node4;
    
      int child_index;
     
      if (p3node->isLeaf()) { 
    
          node4 = Node4{p3node, new_key, new_value}; // We construct a 4-node from the 3-node leaf.
      } else { 
        //...omitted. See below  
      }
         //...omitted. See below
     } 

Next the 4-node is "split" into two 2-nodes: one that contains the smallest key in ``node4`` and that adopts node4's two left most childre; the other will
contains node4's largest key and adopts node4's two right most children. The smaller 2-node is simply pnode downsized from a 3-node to a 2-node.  
The larger 2-node is allocated on the heap:

.. code-block:: cpp

    pnode->convertTo2Node(std::move(node4)); // takes an rvalue: Node4&&

    std::unique_ptr<Node23> larger_2node{std::make_unique<Node23>(node4)}; 
                                                                          
Next, split attempts to "push" or insert the middle key (and its asoociated value) of node4 in the parent node. There are cases it considers:

1. when pnode is the root, ``CreateNewRoot()`` is called to add a new root node above pnode 

.. code-block:: cpp

      if (pnode == root.get()) {
    
           // We pass node4.keys_values[1].key and node4.keys_values[1].value as the key and value for the new root.
           // pnode == root.get(), and p3node is now a 2-node. larger_2node is the 2-node holding node4.keys_values[2].key.
            
           CreateNewRoot(node4.keys_values[1].p1.first, node4.keys_values[1].p1.second, std::move(root), std::move(larger_2node)); 
      } 

2. when pnode->parent is a 2-node, it calls ``convertTo3Node()`` to rebalance the tree:

.. code-block:: cpp

      else if (parent->isTwoNode()) { // Since p3node is not the root, its parent is an internal node. If it, too, is a a 2-node,
    
          // we convert it to a 3-node by inserting the middle value of node4 into the parent, and passing it the larger 2-node, which it will adopt.
          parent->convertTo3Node(node4.keys_values[1].p1.first, node4.keys_values[1].p1.second, std::move(larger_2node));
      }

3. if the parent is a 3-node, we recurse. The recursion terminates when either of the two above cases is encountered, as will eventually always be the
   case.

.. code-block:: cpp

      else { // parent is a 3-node, so we recurse.
    
         // parent now has three items, so we can't insert the middle item. We recurse to split it.
         split(parent, node4.keys_values[1].p1.first, new_value, child_indecies, std::move(larger_2node)); 
      } 
    
      return;
    } // end of split()

See the source code comments for details on the subroutines ``convertTo3Node()`` and ``CreateNewRoot()`` as well as slides #16 through #26 at `Data Structures Balanced Trees <https://www.cse.unr.edu/~mgunes/cs302/Chapter19-BalancedSearchTrees.ppt>`_
and the insertion examples at `Balanced Trees <http://algs4.cs.princeton.edu/33balanced/>`_, where the use the 4-node is explained. 

Deletion
^^^^^^^^

The deletion algorithm is based on the examples in slides # through # and the pseudo code in slide #. 
.. http://algs4.cs.princeton.edu/33balanced/
.. http://www-bcf.usc.edu/~dkempe/CS104/11-19.pdf

Pseudocode and Code
~~~~~~~~~~~~~~~~~~~

`void tree23<Key, Value>::remove(Key key)`` pseudocode: 

    remove first calls ``findRemovalStartNode(Key key, std::stack<int>& child_indecies, int& found_index) const noexcept)``, which returns:
    
    1. ``Node23 *`` of node with key, or ``nullptr`` if key not found.
    2. index of key
    3. a ``stack<int>`` of the child indecies we descended from the root.

The source code is

.. code-block:: cpp

    template<class Key, class Value> void tree23<Key, Value>::remove(Key key)
    {
      if (isEmpty()) {
    
         return;
      }
    
      std::stack<int> descent_indecies; 
    
      int found_index = Node23::NotFoundIndex;
      
      Node23 *premove_start = findRemovalStartNode(key, descent_indecies, found_index);
    
      if (premove_start == nullptr) return;
    
      Node23 *pLeaf;
     
      if (!premove_start->isLeaf()) { // If it is an internal node...
    
          // ...get its in order successor, which will be keys_values[0].key of a leaf node.
          pLeaf = remove_getSuccessor(premove_start, found_index, descent_indecies); 
              
          /*  
           * Swap the internal key( and its associated value) with its in order successor key and value. The in order successor is always in
           * keys_values[0].key.
           */
          std::swap(premove_start->keys_values[found_index], pLeaf->keys_values[0]); 
            
      } else { // ...premove_start is a leaf, and the key is in premove_start->keys[found_index]
          
          pLeaf = premove_start;
      } 
      
      pLeaf->removeLeafKey(key); // remove key from leaf         
      
      // We now have reduced the problem to removing the key (and its value) from a leaf node, pLeaf. 
      if (pLeaf->isEmpty()) { 
          
          fixTree(pLeaf, descent_indecies);  
      }
    
      return;
    }

    template<class Key, class Value> inline typename tree23<Key, Value>::Node23 *tree23<Key, Value>::findRemovalStartNode(Key key,\
                                                                                       std::stack<int>& child_indecies, int& found_index) const noexcept
    {
        
      found_index = Node23::NotFoundIndex;
      Node23 *premove_start = root.get();
      
      while(1) { // Search for key until found, or we reach a leaf and it is not found when we simply return.
    
        int child_index; 
        
        bool found = premove_start->NodeDescentSearch(key, found_index, child_index); 
    
        if (found) {
    
            break;  
    
        } else if (premove_start->isLeaf()) {
    
            return nullptr;
    
        } else {  // continue looking...
            
           child_indecies.push(child_index); // ...remembering which child node branch we took.
    
           premove_start = premove_start->children[child_index].get();
        }
      }  
    
      return premove_start;
    }

Continuing the pseudo code:
    
    If key does not exist, we return. If key is in an internal node, swap the key with the key's in order successor's, which will always be in a 
    leaf, enabling deletion to start at a leaf node. Next,
        
    delete swapped key from leaf node 
    
    if leaf is now empty
     ``fixTree(pLeaf, stack_of_child_indecies)``  
    
    **fixTree** pseudocode:
    
    Note: initially pLeaf is an empty leaf node. Later, during recursive calls, pnode is an empty internal 2-node with only one non-nullptr child.  
    
fixTree pseudo code:
    
    fixTree is called when a node has become empty, and the tree needs to be rebalanced. It is initially called when a leaf node becomes empty. It first attempts
    to barrow a key from a 3-node sibling. silbingHasTwoItems() is called to determine if any 3-node sibling exists. If one does, it calls barrowSiblingKey(), which
    will remove a key/value from the sibling and then shifts it left or right so that the tree is re-balanced, and the empty node is filled with the 'barrowe'
    key/value.  
    
    If no adjacent sibling is a 3-node, a key/value from the parent is brought down and merged with a sibling of pnode. Any non-empty children of pnode are moved to
    the sibling. Upon return, pnode is deleted from the tree by calling ``unique_ptr<Node23>::reset()``.  
    
    If the parent of pnode has now become empty (because merge2Nodes was called), a recursive call to fixTree is made.

.. code-block:: cpp

    template<class Key, class Value> void tree23<Key, Value>::fixTree(typename tree23<Key, Value>::Node23 *pnode, std::stack<int>& descent_indecies) noexcept
    {
      if (pnode == root.get()) {

         // If the root is a leaf (now empty), set root to nullptr, else make the merged sibling the root.
         reassignRoot(); 
         return;
      }
    
      int child_index = descent_indecies.top();
      descent_indecies.pop();
    
      // case 1. If the empty node has a sibling with two keys, then we can shift keys and barrow a key for pnode from its parent. 
      int sibling_index;
    
      if (pnode->siblingHasTwoItems(child_index, sibling_index)) { 
    
          barrowSiblingKey(pnode, child_index, sibling_index);
         
      } else  { // No sibling has two items, so we merge a key/value from pnode's parent with the appropriate sibling. 
    
          Node23 *parent = pnode->parent;
    
          // child_index is such that: parent->children[child_index] == pnode
    
          std::unique_ptr<Node23> node2Delete = mergeNodes(pnode, child_index); 
    
          node2Delete.reset(); 
    
           if (parent->isEmpty()) { 
    
              // recurse. parent is an internal empty 2-node with only one non-nullptr child.
              fixTree(parent, descent_indecies);
          }
      }   
    }

    template<class Key, class Value> inline void tree23<Key, Value>::reassignRoot() noexcept
    {
       // The root is a leaf
       if (root->isLeaf()){
    
          root = nullptr; // deletes the memory held by the unique_ptr<Node>.
    
       } else {
       // recursive remove() case:
       // If the root has a sole non-empty child, make it the new root. unique_ptr's assignment operator will first delete the current empty root
       // node pointer before doing the assignment.
          root = std::move(root->getNonNullChild());  
          root->parent = nullptr;   
       }
       
       if (height > 0 ) {
          --height;
       }
    }

The complete source code is at https://github.com/kkruecke/23tree-in-cpp
