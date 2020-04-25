Shubham Mittal
CS 214
Assignment 1

Design:
mymalloc.c
Declare the array myblock[MAX_SIZE] - create an array of size 4096 (Defined in mymalloc.h)

mymalloc() function - 
To begin with, we do blunder checks by returning 0 whenever passed parameter, 'estimate', is > MAX_SIZE or if size==0. At that point we emphasize through the myblock cluster and check for the main record that isn't as of now involved. When discovered, we increase by 1 space each time until we have discovered that the present square is sufficiently substantial to apportion memory. In the event that we experience a space that !=0 we reset our incrementer and keep scanning for another square of memory. On the off chance that anytime I == MAX_SIZE, it implies we try not to have an accessible square to allot memory and return 0. Something else, when we effectively find a square of accessible memory, we discover the file of the start of this square (which is put away in 'record'). At that point, we store measure in each file from myblock[record] to myblock[record+size]. At last, we return the location of the start of this square.

myfree() function -
To start with, we do mistake checks: Error whenever passed parameter = 0. At that point we repeat through the myblock cluster until we discover a memory address that coordinates the location from the client's information. On the off chance that the circle completes without any matches OR if the coordinated location's esteem = 0, at that point the location they are endeavoring to free is as of now liberated. Since involved records of the cluster contain the span of their square, we use that incentive to free the suitable area in myblock[] (by setting equivalent to 0).

Workloads:
Workload A 0.000008 seconds
Workload B 0.000026 seconds
Workload C 0.000011 seconds
Workload D 0.000034 seconds
Workload E 0.000581 seconds
Workload F 0.004262 seconds

We see that outstanding burden A and C are the quickest remaining tasks at hand. This is on the grounds that in the two remaining burdens we are progressing in the direction of the "front" of the pile which implies that mallocing and liberating does not expect us to repeat far through the pile. An outstanding task at hand B and D are comparable and a bit slower than An and C. B is slower than A and C on the grounds that rather than as it was utilizing the front of the load, it goes 50 bytes down and liberates them all. This implies we should navigate the stack 1+2+3...+49+50 occasions with every 50 mallocs, and again with every 50 liberates. D takes much longer on the grounds that rather than as it were one-byte lumps, it can go as far as possible up to 64 bytes. This implies it is conceivable to get somewhere down in the store which implies more emphasis required, which implies more runtime.
