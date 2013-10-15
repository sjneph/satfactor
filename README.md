## Convert Integer Factorization into a boolean SATISFIABILITY problem ##
> Shane Neph


Overview
=========
Determining factors of a large integer number has been of interest to man since at least Euclid's time. There is no known 
general algorithm for this problem that scales in less than exponential time with respect to the number of bits needed 
to represent the integer number. 

Integer factorization is more than just of theoretical interest.  It is the foundation of public RSA encryption that keeps
your transactions with your bank or an online retailer safe!

What this code does
==================== 
Convert an integer factorization problem into a boolean SATISFIABILITY problem. 
If the problem is solved, extracts the integer factors of interest.
 
Boolen satisfiability solvers improve every year. Every 2 years, an international competition between solvers takes place (see 
http://www.satcompetition.org/ and http://www.satlive.org/).  How well can these state-of-the-art solvers do against one of the 
oldest open math problems in existence? 

This project has 2 main purposes: 
1) Factor an integer of interest! 
2) Quickly create either a solvable or unsolvable SATISFIABILITY problem, whose difficulty is easily controlled by the creater. 

o To create an unsolvable SATISFIABILITY problem, simply encode a prime number. 
o To create more difficult but solvable SATISFIABILITY problems, choose larger composite numbers with fewer factors.
 
The number of interest may be any size! 
 
There are some open-source SATISFIABILITY solvers.  See http://www.satlive.org/ for some of these.
 
Build
======
cd src  
make  
../bin/iencode -h  
../bin/extract-sat -h  

How-To
=======
Input a number of interest in binary form: 

iencode 10101 > composite.21
solve with your favorite solver and put results in solution.txt
extract-sat composite.21 solution.txt

The output would be: 
00011 
00111 

which are binary representations for decimals 3 and 7, the factors of 21. 
 
If an input integer has more than 2 factors, and the SAT problem is solved, the output will be two of the factors only.  These 
may not be prime numbers (you could test that case easily in Maxima, Maple, or Mathematica).  You could input each of them to 
iencode and try to solve further. UNSAT means an input integer was prime.

Not all SATISFIABILITY solvers output results in the same format.  You may need to doctor the results up slightly.  extract-sat 
requires a solution file containing a list of integers (on any number of lines).  For example, 

1 -2 3 4 -5 ...

To-Do 
====== 
Use the gmp library to allow inputs/outputs of any size in decimal form.  Currently I only allow binary integer form. 
