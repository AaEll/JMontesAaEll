Authors:
Aaron Elliot
Justin Montes

PA3 Final Assignment:

Notes:

Our final project has been split into two parts: pa31.c and pa32.c .

Our memory allocator makes use of a node based linked list that keeps track of start-position, end-position, and the next node.

Heap Check is located in pa31.c and pa32.c files. They should be located from the start of the "// Heap Checker" tag to the end of the main() function.
However, a hcheck.c file is included for easier analysis.

_______________________________________________________________________________________________________________

Method:
We used a linked list stored in the heap to track the spaces we allocated the user. In the case that Malloc and Free are both
called randomly in varying amounts, our method performs the best. That is because, under this framework, there is no need to 
"merge" free spaces.

Strengths:
Performs well in cases where other methods have to "merge" free spaces often.

Weaknesses:
In other cases, due to our linked list architecture, speed is reduced.
