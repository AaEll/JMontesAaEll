Authors:
Aaron Elliot
Justin Montes

PA3 Final Assignment:

Notes:

Our final project has been split into two parts: pa31.c and pa32.c .

Our memory allocator makes use of a node based linked list that keeps track of start-position, end-position, and the next node.

Heap Check is located in pa31.c and pa32.c files. They should be located from the start of the "// Heap Checker" tag to the end of the main() function.
However, a hcheck.c file is included for easier analysis.

Weaknesses:
Due to our linked list architecture, speed isn't the strongest point in our allocator.
