#include <atomic>
#include <iostream>
#include <thread> 
#include <math.h>
#include <mutex>
#include <fstream>
using namespace std; 
using namespace std::chrono;

mutex m;
mutex a;
mutex b;
FILE *f;
FILE *g;
FILE *h;



long long int temp = 1;

long long int count = 0;


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool isPrime(long int n)
{
    
    
    // Corner cases
    if (n <= 1)
        return false;
    if (n <= 3)
        return true;
  
    // This is checked so that we can skip
    // middle five numbers in below loop
    if (n % 3 == 0 || n % 2 == 0)
        return false;
  
    for (int i = 5; i * i <= n; i = i + 6)
        if (n % i == 0 || n % (i + 2) == 0)
            return false;
  
    return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//DAM Method

void DAM(int n){

  long int N = pow(10,n);
  long int i = 0;
 
  
  h = fopen("Primes-DAM.txt","w");
 while(i<N){
     m.lock();
     i = temp;
     temp++;
     m.unlock();
     if(i<N){
     if(isPrime(i)) {
       a.lock();
       cout<<i<<" ";
       fprintf(h,"%ld ",i);
       a.unlock();
       
      }
      
     }
 }
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//SAM1 Method

void SAM1(int i,int n,int m){

    //long long int i = std::this_thread::get_id;
    long int N = pow(10,n);
    
    g = fopen("Primes-SAM1.txt", "w");
    for(long int j = i+1; j<=N; j=j+m){
        
         
        if(isPrime(j)){
            a.lock();
            cout<<j<<" ";
            fprintf(g, "%ld ", j);
            a.unlock();
        }
        
    }
    
    



}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void SAM2(int i,int n,int m){

    //long long int i = std::this_thread::get_id;
    long int N = pow(10,n);
    
    f = fopen("Primes-SAM2.txt", "w");
    for(long int j = 2*i+1; j<=N; j=j+2*m){
         
        if(isPrime(j)){
            b.lock();
            cout<<j<<" ";
            fprintf(f, "%ld ", j);
            b.unlock();
        }
        
    }



}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int main(){
    int n; // n is the number of 10
    //cin>>n;
    int m; // m is the thread
    //cin>>m;
    //long long int N = pow(10,n);
    ifstream infile;
    infile.open("inp-params.txt");
    infile>>n;
    infile>>m;
    infile.close();
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    auto start_time = std::chrono:: high_resolution_clock::now();
    std::thread t1[m];
    for(int i = 0; i<m; i++){
        t1[i] = std::thread(DAM,n); //DAM Technique
    }
    for(int i =0; i<m; i++){
        t1[i].join();
    }
    cout<<endl;
    auto end_time = std::chrono:: high_resolution_clock::now();
    auto duration1 = std::chrono:: duration_cast<milliseconds>(end_time-start_time);

    ofstream timefile("time.txt",ios::trunc);

    timefile<<"Time1:- "<<duration1.count()<<endl;
 /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////   

 auto starttime = std::chrono:: high_resolution_clock::now();
    
    std::thread t[m];
    for(int i = 0; i<m; i++){
        t[i] = std::thread(SAM1,i,n,m); //SAM1 Technique
    }
    for(int i =0; i<m; i++){
        t[i].join();
    }
    cout<<endl;
    auto endtime = std::chrono:: high_resolution_clock::now();
    auto duration2 = std::chrono:: duration_cast<milliseconds>(endtime-starttime);

     timefile<<"Time2:- "<<duration2.count()<<endl;
    //timefile.close(); 

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    auto starttime1 = std::chrono:: high_resolution_clock::now();
    
    std::thread t2[m];
    for(int i = 0; i<m; i++){
        t2[i] = std::thread(SAM2,i,n,m); //SAM2 Technique
    }
    for(int i =0; i<m; i++){
        t2[i].join();
    }

    auto endtime1 = std::chrono:: high_resolution_clock::now();
    auto duration3 = std::chrono:: duration_cast<milliseconds>(endtime1-starttime1);
    timefile<<"Time3:- "<<duration3.count()<<endl;
    timefile.close();



    return 0;
}