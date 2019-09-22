In C++17 range-base for loops, the type of the __begin and __end do not need to be the same. __end does not need to be an iterator. It only needs to be comparable to 
__begin.

See: https://stackoverflow.com/questions/39117330/how-the-new-range-based-for-loop-in-c17-helps-ranges-ts

Does this apply to the iterators for 234 tree and 23 tree? To decide, review how the non-recursive in-order traveral works: write down the pseudo code for iterative in-order
traversal using only a pointer to the current node.
