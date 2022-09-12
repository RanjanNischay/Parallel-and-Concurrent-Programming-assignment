-	Extract the file from zipped folder. 
-	Create a file named inp-params.txt in the same directory where the file has been extracted, and enter the value for number of threads, loop iteration, lambda and p.
-	Open the terminal from the directory where the file has been extracted.
-	Enter the following command to run the respective files:-
g++ -pthread -std=c++14 testatomic.cpp
./a.out

g++ -pthread -std=c++14 AtomicMRMW.cpp
./.a.out
-	Two text output file named “C++Atomic.txt” and “AtomicMRMW.txt” will be generated and average time taken output will be shown on the screen.




