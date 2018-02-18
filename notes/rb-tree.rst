A red-black tree is a binary tree representation of a 2-3-4 tree. The child pointer of a node is either red or black. If the child pointer was present in the original 2-3-4 tree, it
is a black pointer; otherwise, it is a red pointer.

A 2-, 3- and 4-nodes is transformed into its red-black representation as follows:

.. include:: <isonum.txt>

Red Black Trees (under develpment)
==================================

**Contents Under Development...**

The algorithms shown in :ref:`2-3-4-trees` ensure a tree structure that is always balanced, but a 2-3-4 tree wastes storage because its 3-node and 4-nodes are not always full. A red black tree is a way of representing a 2 3 4 tree as a nearly\ |apos|\ balanced
binary search tree.  

"Every 2-, 3-, and 4-node in a 2 3 4 tree can be converted to at least 1 black node and 1 or 2 red children of the black node.
Red nodes are always ones that would join with their parent to become a 3- or 4-node in a 2-3-4 tree" from http://ee.usc.edu/~redekopp/cs104/slides/L19b_BalancedBST_BTreeRB.pdf slide 45.

These links have good explanations of red black trees and how to transform the nodes of a 2-3-4 tree into red black tree:

National Chung University pdf shows `how to transform a 2 3 4 tree into a red black tree <http://smile.ee.ncku.edu.tw/old/Links/MTable/Course/DataStructure/2-3,2-3-4&red-blackTree_952.pdf>`_ starting at slide 67 and following. It makes more sense than the USC slides.
These `Red black lecture notes <http://cs.armstrong.edu/liang/intro6e/supplement/CH10TreeRBTree.pdf>`_ look really good. They introduce the concept of external nodes, and it seems like the more thorough and tutorial-oriented of the lectures.
It has succinct proofs about 2 3 4 tree and red black tree equivalence.

NIU also has `a very succinct illustration and explanation of how 2-, 3- and 4-nodes correspond to red and black nodes <http://faculty.cs.niu.edu/~freedman/340/340notes/340redblk.htm>`_. 

As does `this stackover flow explanation <https://stackoverflow.com/questions/35955246/converting-a-2-3-4-tree-into-a-red-black-tree>`_.


Thoughts so far
---------------

A red-black tree is a binary tree representation of a 2-3-4 tree. The 2- and 4-nodes have only one equiavlent representation in a red black tree, but a 3-node can be represented two possilbe ways. <Describe node coloring in reb black trees>
The nodes of a 2-, 3- and 4-nodes ares transformed into red-black tree nodes as follows:

A 2-node

a 4-node

a 3-node


Ulitmately explain why the invarient of the red black tree always holds true under the mappings described above.

A red black tree corresponds to a unique 2-3-4 tree; however, that 2-3-4 tree can be represented by different red black trees, but for all "tallest leaf" - "shortest" leaf <= 2 (I need the correct term for tallest and shortest leaf).

.. todo::

   Take one of the trees outputted by my 2 3 4 tree test code and convert it into a red black tree.

Other sources:

* `Trees <https://azrael.digipen.edu/~mmead/www/Courses/CS280/Trees-2.html>`_.
* `Mapping 2 3 4 Trees to Red Black Trees <https://azrael.digipen.edu/~mmead/www/Courses/CS280/Trees-Mapping2-3-4IntoRB.html>`_.
* `USC lecture slides on B-Trees (2-3, 2 - 3 - 4) and Red/Black Trees <http://ee.usc.edu/~redekopp/cs104/slides/L19b_BalancedBST_BTreeRB.pdf>`_. 
* `Lecture on Red-Black Trees <http://web.eecs.umich.edu/~sugih/courses/eecs281/f11/lectures/11-Redblack.pdf>`_. Discussion of red black trees starts on p. 44. Also see this University of Michigan
