Red Black Trees
---------------

The algorithms shown in :ref:`2-3-trees` and :ref:`2-3-4-trees` ensure a tree structure that is always balanced, but both a 2-3 tree and 2-3-4 tree waste storage since 3-node and 4-nodes are not always full.

`Chapter 13 of Introduction to Algorithms  by Thomas H. Cormen, Charles E. Leiserson, and Ronald L. Rivest <http://staff.ustc.edu.cn/~csli/graduate/algorithms/book6/chap13.htm>`_
discusss the structure and properties of binary search trees(BST), which contain only one key per node (and its associated) value. A BST, however, is necessarily balanced and can degenerate into list with O(n) performance.

There is a one-to-one mapping between 2-3 trees, and 2-3-4 tree, and a type of balanced binary search tree called a red-black tree. This correspondence between 2-3 trees, and 2-3-4 trees, with red black trees is explained in this USC pdf on
`B-Trees (2-3, 2 - 3 - 4) and Red/Black Trees <http://ee.usc.edu/~redekopp/cs104/slides/L19b_BalancedBST_BTreeRB.pdf>`_ and this University of Michigan `Lecture on Red-Black Trees <http://web.eecs.umich.edu/~sugih/courses/eecs281/f11/lectures/11-Redblack.pdf>`_.

.. todo: add the code
