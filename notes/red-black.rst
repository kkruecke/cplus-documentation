Introduction
------------

The algorithms of a 2-3 trees and 2-3-4 trees ensure it is always balanced (perfectly balanced?), but a 2-3-4 tree wastes storage since 3-node and 4-nodes ...s sometimes have only one element. 

A red-black tree solves this inefficient storage problem of a 2-3 tree.  Unlike a 2-3 tree a red black tree contains only 2-nodes. There is a one-to-one mapping between 
2-3 trees and red-black trees. To achieve this one-to-one mapping, the 2-nodes of a red-black tree contain extra information to represent the 2-3 tree node counterpart.

The red-black node
------------------

.. code-block:: cpp


Isomorphism between 2-3 trees and red-black trees
-------------------------------------------------

A red black tree is type of 2-3 tree implementation that contains only 2-nodes.
Encoding 3-nodes. The basic idea behind red-black BSTs is to encode 2-3 trees by starting with standard BSTs (which are made up of 2-nodes) and adding extra information to encode 3-nodes. We think of the links as being of two different types: red links, which bind together two 2-nodes to represent 3-nodes, and black links, which bind together the 2-3 tree. Specifically, we represent 3-nodes as two 2-nodes connected by a single red link that leans left. We refer to BSTs that represent 2-3 trees in this way as red-black BSTs.

One advantage of using such a representation is that it allows us to use our get() code for standard BST search without modification. 
