.. include:: <isopub.txt>

.. role:: kurt-code

Heap  and Priority Queues
=========================

Description
-----------

A heap is a complete binary tree, i.e., a binary tree in which nodes are added from left to the right until the level is complete, and in which no node has a key that is less than the key of either of its children. In a complete binary tree
the height of the left subtree is at most one more than the height of the right subtree. Unlike a binary search tree, in a heap the ordering of siblings is undetermined.

.. figure:: ../images/heap-depiction.jpg
   :alt: Logical Structure 
   :align: center 
   :scale: 100 %
   :figclass: custom-figure

   **Figure 1. Logical Structure of a binary heap.** From: https://www.cs.uaf.edu/2009/spring/cs311/slides/cs311_20090417_heap_pq_stl.pdf

While a heap is a type of complete binary tree, it can be unique represented by storing its level order traversal in an array. In the array representation below, index zero is unoccupied, and the root is placed at index one. 

.. .. figure:: ../images/heap-nodes-numbered.jpg
..    :alt: Logical Structure 
..    :align: center 
..    :scale: 100 %
..    :figclass: custom-figure

..    **Figure 1. Numbering the Nodes of a Complete Binarya binary heap.** 

.. figure:: ../images/heap-as-array1.gif
   :alt: Logical Structure 
   :align: center 
   :scale: 100 %
   :figclass: custom-figure

In the array representation of a heap no space is required for pointers; instead, the parent and children of each node can be found by simple arithmetic on array indices:

===================================== ==============================

===================================== ==============================
  Parent(i) = i/2                      returns position of parent
  Left(i)  = 2i                        returns left child position
  Right(i) = 2i + 1                    returns right child position
===================================== ==============================

.. figure:: ../images/heap-as-array1.gif
   :alt: Logical Structure 
   :align: center 
   :scale: 100 %
   :figclass: custom-figure

If instead of one, we begin the array at index zero, the calculations for the position of a node's parent and its two children become:

===================================== ==============================

===================================== ==============================
  Parent(i) = (i-1)/2                  returns position of parent
  Left(i)  = 2i + 1                    returns left child position
  Right(i) = 2i + 2                    returns right child position
===================================== ==============================

.. todo:: Organize the remaining discussion after reading the bookmarked articles and understanding them and how they flow.

Max Heap Operations
-------------------

The abstract heap data type has three primary operatons:

1. **peekTop()**  
2. **add(int priority, const T& t)** 
3. **remove()**

**peekTop()** retrieve the root of the tree. **add(int priority, const T& t)** adds a new node to the end of the heap, which then "trickles up" to its position in the ordering. **remove()**  deletes the root and reheapifyies array by making the last
item the root, and then moving in downward until we again have a value heap.  


Code of Array Implementation of Max Heap
----------------------------------------

**add(int priority, const T& t)** Code 
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Pseudocode for Adding an Element to Max Heap:

1. Place the new element in the heap in the last location. 
2. Repair the heap ordering by swapping the element with its parent(s) as long as the new element has the greater value.

C++ Code
--------

The C++ implementation uses a functor to do the comparison operation. This allows the same code to be used for both a max heap and a min heap.

.. code-block:: cpp

    template<class T, class Comp=std::less<int> > class heap;  // forward declaration
    
    template<class T> class max_heap : public heap<T>  {};     // max_heap
    template<class T> class min_heap : public heap<T, std::greater<T>>  {};  // min_heap
    
    template<class T, class Comp> class heap {
      
        class Node {
         // snip...
        };        
    
        std::vector<Node> vec;
        Comp compare_functor;
        
        int size;
    
        bool compare(const Node& lhs, const Node& rhs)
        {
           return compare_functor(lhs.getPriority(), rhs.getPriority());		
        }	
    
       heap(int sz) : vec(sz), size{0} {}
    
       heap() : vec(), size{0} {}
    
       // snip....
     };
  
.. code-block:: cpp

  template<class T, class Comp> void heap<T, Comp>::add(int x, const T& t)
  {
      ++size;
  
      vec.push_back(Node(x, t)); 
  
      int index = vec.size() - 1;
  
      swim(index); // repair heap property
  }
  
  template<class T, class Comp> void heap<T, Comp>::swim(int index)
  {
      // Move new item up until we have a valid heap
      int parentIndex;
  
      while (index > 0) { 
  
          parentIndex = (index - 1) / 2;
          
         if (compare(vec[index], vec[parentIndex])) {
         
             break; 
  
          }  else {        
  
             std::swap(vec[index], vec[parentIndex]);
  
             index = parentIndex;
          }
      }
  }
  
**remove()** Code 
~~~~~~~~~~~~~~~~~

Removes the root. Places the last item in the heap as the root, and the repair the root by invoking `swim(int index)`, which swapshe new root going down is swapped with its largest child. We continue to study this algorithm in more details in the next Chapter,
 
.. code-block:: cpp

  template<class T, class Comp> bool heap<T, Comp>::remove()
  {
     if (vec.empty()) {
  
         return false;
     }
  
     --size;
       
     // put last item in root
     vec[0] = vec[vec.size() - 1];
             
     vec.pop_back(); // then remove the formerly last item
  
     sink(0);  // repair heap property
  
     return true;
  }
  
  template<class T, class Comp> inline void heap<T, Comp>::sink(int root)
  {
    int child = 2 * root + 1; // Determine if root is a leaf.
  
    if (child < vec.size()) {  // If root is not a leaf, get the index of its largest child, either left or right
  
       int right_child = child + 1; 
  
       if (compare(vec[child], vec[right_child])) {
              
             child = right_child; 
        }
  
        // If root smaller than right child, swap root with larger child...
        if (compare(vec[root], vec[child])) {
  
             std::swap(vec[root], vec[child]); 
  
             // ... and continue the process 
             sink(child);
        }  
    }
  }
