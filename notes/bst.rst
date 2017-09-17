.. include:: <isopub.txt>


Introduction to Binary Search Trees
-----------------------------------

In a binary search tree (BST) each node has two children, generally designated **left** and **right**, and all nodes in the left subtree have values less than the root and all values in the right subtree have values
greater than the root. `CHAPTER 13: BINARY SEARCH TREES <http://staff.ustc.edu.cn/~csli/graduate/algorithms/book6/chap13.htm>`_ of "Introduction to Algorithms
by Thomas H. Cormen, Charles E. Leiserson, and Ronald L. Rivest" has a complete discussion together with pseudo code.

:ref:`2-3-trees` and :ref:`2-3-4-trees` provide the basis for understanding red black trees, a type of self\ |dash| balancing BST that provides space savings over 2 3 trees or 2 3 4 trees. The BST implementation below is available on
`github <https://github.com/kkruecke/binary-search-tree>`_.

.. code-block:: cpp 
    
    
    #include <memory>
    #include <utility>
    #include <queue>
    #include <initializer_list>
    #include <iostream>  
    #include <exception>
    
    template<class Key, class Value> class bstree; // forward declarations of template classes.
    
    template<class Key, class Value> class bstree {
        
      public:
      
       /*
        * The tree consists of heap-allocated Node nodes managed by std::unique_ptr<Node>'s.
        */ 
       class Node {
        public:   
    
            friend class bstree<Key, Value>;    
    
            Node(Key key, const Value& value, Node *ptr2parent=nullptr);
    
            // We disallow copy construction and assignment...
            Node(const Node&) = delete;  
        
            Node& operator=(const Node&) = delete; 
            
            Node(Node&&); // ...but we allow move assignment and move construction.
    
            std::ostream& print(std::ostream& ostr) const noexcept; 
    
            friend std::ostream& operator<<(std::ostream& ostr, const Node& node) noexcept
            { 
                node.print(ostr);
                return ostr << ' ' << std::endl;  
            }
            
        private:      
            
            Node& operator=(Node&&) noexcept;
            
        public:    
            constexpr bool isLeaf() const noexcept { return (left == nullptr && right == nullptr) ? true : false; } 
    
            /* 
             * Note: Functors passed to bstree<Key, Value>::inOrderTraverse(Functor f) should use these two methods below.
             * because the functor's function call operator will be passed 'const Node&'
             */ 
            constexpr Key key() const { return nc_pair.first; }  
            constexpr const Value& value() const noexcept { return const_pair.second; }  
            
        private:
               Node *parent;
                                     
               union {           
                  std::pair<Key, Value>        nc_pair;  // ...this eliminates constantly casting of const_cast<Key>(p.first) = some_noconst_key;
                  std::pair<const Key, Value>  const_pair;  // but always return this member of the union.
                  
               };   
     
               std::unique_ptr<Node> left;
               std::unique_ptr<Node> right;
        }; 
    
      private: 
        std::unique_ptr<Node> root; 
    
        template<typename Functor> void DoInOrderTraverse(Functor f, const std::unique_ptr<Node>& root) const noexcept;
    
        template<typename Functor> void DoPostOrderTraverse(Functor f,  const std::unique_ptr<Node>& root) const noexcept;
        template<typename Functor> void DoPreOrderTraverse(Functor f, const std::unique_ptr<Node>& root) const noexcept;
    
        void destroy_tree() noexcept 
        {
           destroy_tree(root);   
        }
    
        void destroy_tree(std::unique_ptr<Node> &current) noexcept; 
    
        void clone_tree(const std::unique_ptr<Node> &src, std::unique_ptr<Node>& dest, const Node *parent) noexcept; 
    
        void DestroyTree(std::unique_ptr<Node> &root) noexcept; 
    
        const Node *min(const Node *current) const noexcept;
       
        const Node *getSuccessor(const Node *current) const noexcept;
       
        const Node *findNode(Key key, const Node *current) const noexcept; 
    
        const std::unique_ptr<Node>& get_unique_ptr(const Node *pnode) const noexcept;
    
      public:
        // Container typedef's used by STL.
    
        using value_type      = std::pair<const Key, Value>; 
        using difference_type = long int;
        using pointer         = value_type*; 
        using reference       = value_type&; 
    
        bstree() noexcept : root{nullptr} { }
    
        bstree(std::initializer_list<value_type> list) noexcept; 
    
        void test_invariant() const noexcept;
    
        bstree(const bstree&) noexcept; 
    
        bstree(bstree&& lhs) noexcept : root{std::move(lhs.root)} {} // move constructor
    
        bstree& operator=(const bstree&) noexcept; 
    
        bstree& operator=(bstree&&) noexcept; // move assignemtn
    
        int getHeight() const noexcept;
     
        bool isEmpty() const noexcept;
    
        const Value& operator[](Key key) const;
    
        Value& operator[](Key key);
    
        void insert(Key key, const Value& value) noexcept;
    
        void remove(Key key) noexcept;
    
        std::pair<bool, const Node *> find(Key key) const noexcept;
        
        // Breadth-first traversal
        template<class Functor> void levelOrderTraverse(Functor f) const noexcept;
    
        // Depth-first traversals
        template<typename Functor> void inOrderTraverse(Functor f) const noexcept { return DoInOrderTraverse(f, root); }
        template<typename Functor> void preOrderTraverse(Functor f) const noexcept  { return DoPreOrderTraverse(f, root); }
        template<typename Functor> void postOrderTraverse(Functor f) const noexcept { return DoPostOrderTraverse(f, root); }
    };
    
    template<class Key, class Value> std::ostream& bstree<Key, Value>::Node::print(std::ostream& ostr) const noexcept
    {
      ostr << "[ { " << nc_pair.first << ", " << nc_pair.second << "} : parent(" << parent << "), this(" << this << ") ]";
      return ostr; 
    }
    
    // Breadth-first traversal. Useful for display the tree (with a functor that knows how to pad with spaces based on level).
    template<class Key, class Value> template<typename Functor> void bstree<Key, Value>::levelOrderTraverse(Functor f) const noexcept
    {
       std::queue< std::pair<const Node*, int> > queue; 
    
       Node* proot = root.get();
    
       if (proot == nullptr) return;
          
       auto initial_level = 1; // initial, top root level is 1.
       
       // 1. pair.first  is: const tree<Key, Value>::Node23*, the current node to visit.
       // 2. pair.second is: current level of tree.
       queue.push(std::make_pair(proot, initial_level));
    
       while (!queue.empty()) {
    
            std::pair<const Node *, int> pair_ = queue.front();
    
            const Node *current = pair_.first;
    
            int current_tree_level = pair_.second;
    
            f(*current, current_tree_level);  
            
            if (current != nullptr && !current->isLeaf()) {
        
                queue.push(std::make_pair(current->left.get(), current_tree_level + 1));  
                queue.push(std::make_pair(current->right.get(), current_tree_level + 1));  
            }
    
            queue.pop(); 
       }
    }
    
    template<class Key, class Value> bstree<Key, Value>::Node::Node(Key key, const Value& value, Node *ptr2parent)  : parent{ptr2parent}, left{nullptr}, right{nullptr}, \
            nc_pair{key, value}
    {
    }
    
    template<class Key, class Value> inline bstree<Key, Value>::Node::Node(Node&& node) : parent{node.ptr2parent}, left{std::move(node.left)}, right{std::move(node.right)}, nc_pair{std::move(node.nc_pair)}
    {
    }
    
    
    template<class Key, class Value> inline bool bstree<Key, Value>::isEmpty() const noexcept
    {
      return root == nullptr ? true : false;
    }
    
    template<class Key, class Value> const std::unique_ptr<typename bstree<Key, Value>::Node>& bstree<Key, Value>::get_unique_ptr(const Node *pnode) const noexcept
    {
      // Get the unique_ptr<Node> that manages the raw pointer ancester. We must consider the case when ancestor is the parent and has 
    
      if (pnode->parent == nullptr) { // Is ancestor the root? 
    
         return root; 
    
      } else {
    
         return (pnode->parent->left.get() == pnode) ? pnode->parent->left : pnode->parent->right;  
      }
    }
    
    template<class Key, class Value> template<typename Functor> void bstree<Key, Value>::DoInOrderTraverse(Functor f, const std::unique_ptr<Node>& current) const noexcept
    {
       if (current == nullptr) {
    
          return;
       }
    
       DoInOrderTraverse(f, current->left);
       /*
       Note: DoInOrderTraver will invoke Functor's function call operator
        * 
        *       Functor::operator()(const Node&) 
        *   
        * which must, in turn, call Node::key() and Node::value() or Node::get_nc_pair() to get access Node's private members.
        */ 
    
       f(*current); //  f(const bstree<Key, Value>::Node&)
       
    
       DoInOrderTraverse(f, current->right);
    }
    
    template<class Key, class Value> template<typename Functor> void bstree<Key, Value>::DoPreOrderTraverse(Functor f, const std::unique_ptr<Node>& current) const noexcept
    {
       if (current == nullptr) {
    
          return;
       }
       /*
       Note: DoInOrderTraver will invoke Functor's function call operator
        * 
        *       Functor::operator()(const Node&) 
        *   
        * which must, then, call Node::key() and Node::value() to get access Node's key and value members.
        */ 
    
       f(*current); // Functor must define: f(const bstree<Key, Value>::Node&)
    
       DoPreOrderTraverse(f, current->left);
    
       DoPreOrderTraverse(f, current->right);
    }
    
    template<class Key, class Value> template<typename Functor> void bstree<Key, Value>::DoPostOrderTraverse(Functor f, const std::unique_ptr<Node>& current) const noexcept
    {
       if (current == nullptr) {
    
          return;
       }
    
       DoPostOrderTraverse(f, current->left);
    
       DoPostOrderTraverse(f, current->right);
    
       /*
       Note: DoInOrderTraver will invoke Functor's function call operator
        * 
        *       Functor::operator()(const Node&) 
        *   
        * which must, in turn, call Node::key() and Node::value() or Node::get_nc_pair() to get access Node's private members.
        */ 
    
       f(*current); //  f(const bstree<Key, Value>::Node&)
    }
    
    
    /*
      return a pair, in which "first" is true, if found, and "second" points to the found node; otherwise, <false, nullptr> is returned.
     */
    template<class Key, class Value> inline std::pair<bool, const typename bstree<Key, Value>::Node *> bstree<Key, Value>::find(Key key) const noexcept
    { 
        const Node *node = findNode(key, root.get());
    
        return std::make_pair(node != nullptr, (node != nullptr) ? node : nullptr); 
    }
    
    template<class Key, class Value> const typename bstree<Key, Value>::Node *bstree<Key, Value>::findNode(Key key, const typename bstree<Key, Value>::Node *current) const noexcept
    {
      while (current != nullptr && key != current->key()) {
    
          current = (key < current->key()) ? current->left.get() : current->right.get(); 
      }
      
      return current;
    }
    
    template<class Key, class Value> const typename bstree<Key, Value>::Node *bstree<Key, Value>::min(const typename bstree<Key, Value>::Node *current) const noexcept
    {
      while (current->left != nullptr) {
    
           current = current->left;
      } 
    
      return current;  
    }
    
    /*
      If the right subtree of node current is nonempty, then the successor of x is just the left-most node in the right subtree, which is found by calling min(current.right.get()). 
      On the other hand, if the right subtree of node x is empty and x has a successor y, then y is the lowest ancestor of x whose left child is also an ancestor of x.
     
     */
    template<class Key, class Value>  const typename bstree<Key, Value>::Node* bstree<Key, Value>::getSuccessor(const typename bstree<Key, Value>::Node *current) const noexcept
    {
      if (current->right != nullptr) return min(current->right);
    
      Node *ancestor = current->parent;
    
      // find the smallest ancestor of current whose left child is also an ancestor of current (by ascending the ancestor chain until we find the first ancestor that is a left child).
      while(ancestor != nullptr && current == ancestor->right.get()) {
    
           current = ancestor;
    
           ancestor = ancestor->parent;
      }
      return ancestor;
    }
    
    template<class Key, class Value> inline bstree<Key, Value>::bstree(std::initializer_list<value_type> list) noexcept 
    {
      for (auto& pair_ : list) {
    
          insert(pair_.first, pair_.second);
     }
    }
    
    template<class Key, class Value> inline bstree<Key, Value>::bstree(const bstree<Key, Value>& lhs) noexcept 
    { 
      destroy_tree(); // free all the nodes of the current tree 
    
      // Traverse in pre-order cloning each node in lhs.
      clone_tree(lhs.root, root, nullptr);
    }
    
    /*
     * destroy_tree(unique_ptr<Node>&) does a post order tree traversal, using recursion and deleting nodes as they are visited. 
     */
    template<class Key, class Value> void bstree<Key, Value>::destroy_tree(std::unique_ptr<Node> &current) noexcept 
    {
      if (current == nullptr) {
    
          return;
      }
      
      destroy_tree(current->left);
      destroy_tree(current->right);
    
      current.reset(); // deletes the underlying raw pointer. 
    }
    
    template<class Key, class Value> bstree<Key, Value>& bstree<Key, Value>::operator=(const bstree<Key, Value>& lhs) noexcept
    {
      if (this == &lhs)  {
          
          return *this;
      }
      
      destroy_tree(); // free all the nodes of the current tree 
    
      // Traverse in pre-order cloning each node in lhs.
      clone_tree(lhs.root, root, nullptr);
    
      return *this;
    }
    
    // Do pre-order traversal, using recursion and clone the source node
    template<class Key, class Value> void bstree<Key, Value>::clone_tree(const std::unique_ptr<Node>& src, std::unique_ptr<Node>& dest, const typename bstree<Key, Value>::Node *parent) noexcept
    {
      if (src == nullptr) return;
      
      dest = std::make_unique<Node>(src->key(), src->value(), const_cast<Node*>(parent));
      
      clone_tree(src->left, dest->left, dest.get());
      clone_tree(src->right, dest->right, dest.get());
    }
     
    /*
     Like the procedure find(), insert() begins at the root of the tree and traces a path downward. The pointer x traces the path, and the pointer parent is maintained as the parent of current.
     The while loop causes these two pointers to move down the tree, going left or right depending on the comparison of key[pnode] with key[x], until current is set to nullptr. This nullptr
     occupies the position where we wish to place the input item pnode. The subsequent lines et the pointers that cause pnode to be inserted.
    */
    template<class Key, class Value> void bstree<Key, Value>::insert(Key key, const Value& value) noexcept
    {  
        Node *parent = nullptr;
    
        Node *current = root.get();
    
        while (current != nullptr) {
    
             if (current->key() == key) return;
    
             parent = current;
              
             current = key < current->key() ? current->left.get() : current->right.get();
        }
    
        std::unique_ptr<Node> pnode = std::make_unique<Node>(key, value, parent); // This ctor is private
    
        if (parent == nullptr) {
    
            root = std::move(pnode);
    
        } else if (pnode->key() < parent->key() ) {
    
              parent->left = std::move(pnode); 
    
        } else {
    
            parent->right = std::move(pnode);
        }
    }
    /*
     * We handle three possible cases:
     * 1. If the node to remove is a leaf, we simply delete it by calling unique_ptr<Node>'s reset method. 
     * 2. If the node to remove is an internal node, we get its in-order successor and move its pair<Key, Value> into node, and then delete the leaf node successor
     * 3. If the node to remove has only one child, we adjust the child pointer of the parent so it will point to this child. We do this by using unqiue_ptr<Node>'s move assignment operator, which has the 
     *    side effect of also deleting the moved node's underlying memory. We then must adjust the parent pointer of the newly 'adopted' child.
     */
    template<class Key, class Value> void bstree<Key, Value>::remove(Key key) noexcept
    {
      const Node *pnode = findNode(key, root.get());
      
      if (pnode == nullptr) return;
    
      // Get the managing unique_ptr<Node> whose underlying raw point is node? 
      std::unique_ptr<Node>& node = const_cast<std::unique_ptr<Node>&>( get_unique_ptr(pnode) );
    
      //std::unique_ptr<Node>& node = (pnode->parent->left.get() == pnode) ? pnode->parent->left : pnode->parent->right;
            
      // case 1: If the key is in a leaf, simply delete the leaf. 
      if (pnode->isLeaf()) { 
          
          node.reset();     
          
          return;
      }  
    
      if (pnode->left != nullptr && pnode->right != nullptr) {// case 2: The key is in an internal node.   
    
          std::unique_ptr<Node>& successor = getSuccessor(pnode);
    
          node->nc_pair = std::move(successor->nc_pair);  // move the successor's key and value into node. Do not alter node's parent or left and right children.
    
          successor.reset(); // safely delete leaf node successor
             
      }  else { 
    
          // case 3: The key is in a node with only one child. 
    
          std::unique_ptr<Node>& successor = (node->left != nullptr) ? node->left : node->right;
    
          Node *parent = node->parent;
                
          node = std::move(successor);
          
          successor->parent = parent;
      }  
    
      return; 
    }
