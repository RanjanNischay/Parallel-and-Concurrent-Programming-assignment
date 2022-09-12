#include <atomic>
#include <iostream>
#include <thread> 
#include <math.h>
#include<mutex>
using namespace std; 
using namespace std::chrono;

mutex m;
mutex a;

//atomic <long long int> temp(1);
long long int temp = 1;

//long long int count = 0;

bool isPrime(int n)
{
    // Corner cases
    if (n <= 1)
        return false;
    if (n <= 3)
        return true;
  
    // This is checked so that we can skip
    // middle five numbers in below loop
    if (n % 2 == 0 || n % 3 == 0)
        return false;
  
    for (int i = 5; i * i <= n; i = i + 6)
        if (n % i == 0 || n % (i + 2) == 0)
            return false;
  
    return true;
}

void PrimePrint(int n){
 long long int N = pow(10,n);
 long long int i = 0;
 while(i<N){
     m.lock();
     i = temp;
     temp++;
     m.unlock();
     
     if(i<N){
     if(isPrime(i)) {
       a.lock();
       cout<<i<<" ";
       a.unlock();
       //count++;
      }
     }
       
       //std::this_thread::sleep_for(std::chrono::milliseconds(50));
     
     
 }



}

int main(){
    int n; // n is the number of 10
    cin>>n;
    int m; // m is the thread
    cin>>m;
    //long long int N = pow(10,n);
    auto start_time = std::chrono:: high_resolution_clock::now();
    std::thread t[m];
    for(int i = 0; i<m; i++){
        t[i] = std::thread(PrimePrint,n);
    }
    for(int i =0; i<m; i++){
        t[i].join();
    }

    auto end_time = std::chrono:: high_resolution_clock::now();
    auto duration = std::chrono:: duration_cast<microseconds>(end_time-start_time);

    cout<<"Time1:- "<<duration.count()<<endl;

    //cout<<N<<endl;
    //cout<<"No of prime : - "<<count;
    return 0;
}