#include <atomic>
#include <iostream>
#include <thread> 
#include <math.h>
#include<mutex>
#include<chrono>
using namespace std; 
using namespace std::chrono;

mutex a;
int count = 0;


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


void PrimePrint(int i,int n,int m){

    //long long int i = std::this_thread::get_id;
    long long int N = pow(10,n);
    for(int j = i+1; j<=N; j=j+m){
         //a.lock();
        if(isPrime(j)){
            a.lock();
            cout<<j<<" ";
            count++;
            a.unlock();
        }
        //a.unlock();
    }



}





int main(){
    int n; 
    cin>>n; // n is the power of 10
    int m;
    cin>>m; // m is the number of thread
    //long long int N = pow(10,n);
    auto start_time = std::chrono:: high_resolution_clock::now();
    
    std::thread t[m];
    for(int i = 0; i<m; i++){
        t[i] = std::thread(PrimePrint,i,n,m);
    }
    for(int i =0; i<m; i++){
        t[i].join();
    }

    auto end_time = std::chrono:: high_resolution_clock::now();
    auto duration = std::chrono:: duration_cast<microseconds>(end_time-start_time);

    cout<<"Time2:- "<<duration.count()<<endl;
    cout<<count;

    //cout<<N<<endl;
    return 0;
}