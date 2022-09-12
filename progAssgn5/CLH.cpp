#include <iostream>
#include <atomic>
#include "Lock.h"
#include <thread>
#include <chrono>
#include <ctime>
#include <mutex>
#include <unistd.h>
#include <random>
#include <fstream>

using namespace std;
long long totalEntryWaitingTime, totalExitWaitingTime;
int64_t worstcaseentry = 0;int64_t worstcaseexit = 0;

class Qnode{
  public:
   //bool locked = false ;  
   atomic<bool> locked;
};

static thread_local Qnode* mypred = NULL;
static thread_local Qnode* mynode = new Qnode();

// using the thread_local variable creates its own instances of mypred and mynode for each thread.

class CLHLock:public Lock{
  private:
   atomic<Qnode*> tail;
   public:
   //constructor
   CLHLock(){
       tail = new Qnode();
   }

   void lock(){
       //mynode->locked = true;
       mynode->locked.store(true);
       mypred = tail.exchange(mynode);
       //while(mypred->locked == true) { };
       while(mypred->locked.load()){ };
   }
   void unlock(){
      //mynode->locked = false;
      mynode->locked.store(false);
      mynode = mypred;
   }
    //destructor
   ~CLHLock(){
       delete tail;
   }
};

void testCS(int i, int n, int k, int t1, int t2,CLHLock* test){
    int id = i;
    double const exp_dist_lambda_1 = t1;
    double const exp_dist_lambda_2 = t2;
    int seed = chrono::system_clock::now().time_since_epoch().count();
    default_random_engine rg(seed);
    exponential_distribution <double> dist_1(exp_dist_lambda_1);
    exponential_distribution <double> dist_2(exp_dist_lambda_2);

    double t1d = dist_1(rg) + 0.5;
    double t2d = dist_2(rg) + 0.5;

    int time1 = (int)t1d;
    int time2 = (int)t2d;
    ofstream fout("CLH_output.txt", ios::app);
    
    for(int j = 0; j<k;j++){
      auto reqEnterTime = std::chrono:: high_resolution_clock::now();
      std::time_t now1 = std::chrono::system_clock::to_time_t(reqEnterTime);
      
      fout<< j<<" th CS Entry Request at "<< ctime(&now1) <<" by thread " << id << "(msg1) " <<endl; //message 1
      
      test->lock();
      
      auto actEntryTime = std::chrono:: high_resolution_clock::now();
      std::time_t now2 = std::chrono::system_clock::to_time_t(actEntryTime);
      auto duration1 = chrono::duration_cast<chrono::microseconds>(actEntryTime - reqEnterTime);
      totalEntryWaitingTime += duration1.count();
      worstcaseentry = max(worstcaseentry,duration1.count());
      fout<< j<<" th CS Entry at "<< ctime(&now2) <<" by thread " << id << "(msg2) " <<endl; //message 2
      std::this_thread::sleep_for(std::chrono::milliseconds(time1));
      auto reqExitTime = std::chrono:: high_resolution_clock::now();
      std::time_t now3 = std::chrono::system_clock::to_time_t(reqExitTime);

      fout<< j<<" th CS Exit Request at "<< ctime(&now3) <<" by thread " << id << "(msg3) " <<endl; // message 3
      test->unlock();
      auto actExitTime = std::chrono:: high_resolution_clock::now();
      auto duration2 = chrono::duration_cast<chrono::microseconds>(actExitTime - reqExitTime);
      totalExitWaitingTime += duration2.count();
      worstcaseexit = max(worstcaseexit,duration2.count());

      std::time_t now4 = std::chrono::system_clock::to_time_t(actExitTime);
      
      fout<< j<<" th CS Exit at "<< ctime(&now4) <<" by thread " << id << "(msg4) " <<endl; // message 4
      
      std::this_thread::sleep_for(std::chrono::milliseconds(time2));

      




    }
}

int main(){
    int n,k,t1,t2;
    ifstream infile{"inp-params.txt",std::ifstream::in};
    if(infile.fail()){
        cout<<"inp-params.txt file not found";
        exit(1);
    }
    infile>>n>>k>>t1>>t2;
    
    cout<<"n: "<<n<<endl<<"k: "<<k<<endl<<"lambda1: "<<t1<<endl<<"lambda2: "<<t2<<endl;
    std::thread t[n];

    CLHLock* clh;
    clh = new CLHLock();
    ofstream fout("CLH_output.txt");
    
    for(int i = 0; i<n;i++){
        
        t[i] = std::thread(testCS,i,n,k,t1,t2,clh);
        
    }

    for(int i = 0; i<n;i++){
        t[i].join();
    }
    cout << "The average waiting time for threads to enter critial section = "
       << (totalEntryWaitingTime + 0.0) / (n * k) << "us" << endl;
    cout << "The average waiting time for threads to exit critial section = "
       << (totalExitWaitingTime + 0.0) / (n * k) << "us" << endl;
    cout<<"The worst case entry time = " << worstcaseentry << "us" <<endl;
    cout<<"The worst case exit time = " << worstcaseexit << "us" <<endl;


    return 0;

}

