Most of these data structures were create for CS201(Data Structures and Algorithms), while some were created later.
All are written in C and have their own header files. All includes are either standard or build on other structures(e.g. stack is build on the dynamic array).
Below are some overview specifications of the included structures. More information can be found in the header and source files.

Dynamic array
<li>Built on the C array, but can resize itself when needed(grows to 2x the original size, shrinks by half when at quarter capacity)
<li>Requires: da.c, da.h

Circular dynamic array
<li>Also built on the C array, but can insert/delete from the front or back. Same rules for growing and shrinking apply.
<li>Requires: cda.c. cda.h

Stack
<li>This basic stack is built on the dynamic array. Items can be pushed and popped.
<li>Requires: stack.c stack.h da.c da.h

Queue
<li>This queue is built on the circular dynamic array instead. Items can be enqueued and dequeued.
<li>Requires: queue.c queue.h cda.c cda.h

Doubly linked list
<li>The DLL is built with nodes that are linked together in order. As such, items can be inserted and removed anywhere.
<li>Requires: dll.c dll.h

Binary Search Tree
<li>The BST is built with nodes that are built on one another, starting at the root. The queue is needed to print the tree.
<li>Requires: bst.c bst.h queue.c queue.h

"Green Tree"
<li>The only structure here that was class specific. Essentially a wrapper for the BST, but this one can store duplicates.
<li>Requires: gt.c gt.h bst.c bst.h queue.c queue.h

Red-Black Tree
<li>A self-balancing BST. Extends the previous BST.
<li>Requires: rbt.c rbt.h bst.c bst.h queue.c queue.h