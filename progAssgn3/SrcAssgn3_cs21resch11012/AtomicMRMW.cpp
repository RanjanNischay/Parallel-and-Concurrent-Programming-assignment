#include <atomic>
#include <thread>
#include <iostream>
#include <chrono>
#include <ctime>
#include <mutex>
#include <unistd.h>
#include <random>
#include <fstream>
#include "AtomicMRSW.cpp"

using namespace std;

class AtomicMRMW{
  private:
  StampedValue* a_table;
  StampedValue *x = new StampedValue();
  public:
  AtomicMRMW(int capacity,int val){
    a_table = new StampedValue[capacity]; // array of atomic m-valued MRSW registers
    StampedValue *valv = new StampedValue(val);
    for(int i = 0; i<capacity;i++){
        a_table[i].value = val;
        //cout<<a_table[i].value<<endl;
    }
  }

  void write(int id,int value,int capacity){
     int me = id;
     StampedValue* max = new StampedValue();
     for(int i = 0; i< capacity;i++){
          max = max->MAXStampedValue(max,(a_table+i));
     }
     //(a_table + me) = new StampedValue(max->stamp+1,value);
     a_table[me].stamp = max->stamp+1;
     a_table[me].value = value;
     
  }
   int read(int capacity){
      StampedValue* max = new StampedValue();
      for(int i = 0; i<capacity;i++){
         max = max->MAXStampedValue(max,(a_table+i)); 
      }
      return max->value;

   }

   

};
mutex m;
long long totalTime,totalreadtime,totalwritetime,total_read=0,total_write = 0;
void testatomic(int k, double p, int lambda,int i,AtomicMRMW obj,int n){
    int locVar;
    int tid = i;
    double const exp_dist_lambda_1 = lambda;
    int seed = chrono::system_clock::now().time_since_epoch().count();
    default_random_engine rg(seed);
    exponential_distribution <double> dist_1(exp_dist_lambda_1);
    double t1d = dist_1(rg) + 0.5;
    int time1 = (int)t1d;
    for(int j = 0; j<k; j++){
    std::random_device rnd;
    std::mt19937 gen(rnd());
    std::discrete_distribution<> distrib({ p, 1-p });
                                        // ^^^  ^- probability for 1
                                       //  | probability for 0
    int action =  distrib(gen);
    auto reqTime = std::chrono:: high_resolution_clock::now();
    std::time_t now1 = std::chrono::system_clock::to_time_t(reqTime);
    //AtomicMRMW obj(n,17); // taking initial value in the registers as 17.
    ofstream fout("AtomicMRMW.txt", ios::app);
    m.lock();
    fout << j << "th action requested at " << ctime(&now1) << " by thread "<< tid <<endl; 
    m.unlock();
    string str;
    if(action == 0){ // action =0 means read
        auto rs_time = std::chrono::high_resolution_clock::now();
        locVar = obj.read(n);
        auto re_time = std::chrono::high_resolution_clock::now();
        m.lock();
        fout<<" Value Read : " << locVar <<endl;
        m.unlock();
        str = "read";
        auto d1 = chrono::duration_cast<chrono::nanoseconds>(re_time-rs_time);
        totalreadtime += d1.count();
        total_read++;
        }
    else{ // action = 1  means write
       locVar = k * tid;
       auto ws_time = std::chrono::high_resolution_clock::now();
       //shVar.store(locVar,std::memory_order_relaxed);
       obj.write(tid,locVar,n);
       auto we_time = std::chrono::high_resolution_clock::now();
       m.lock();
       fout<<" Value Write: " << locVar<<endl;
       m.unlock();
       str = "write";
       auto d2 = chrono::duration_cast<chrono::nanoseconds>(we_time-ws_time);
       totalwritetime += d2.count();
       total_write++;
       }

       auto complTime = std::chrono:: high_resolution_clock::now();
       std::time_t now2 = std::chrono::system_clock::to_time_t(complTime);
       auto duration = chrono::duration_cast<chrono::microseconds>(complTime - reqTime);
       totalTime += duration.count();
       m.lock();
       fout << j << "th action "<<str<<" completed at " << ctime(&now2) << " by thread "<< tid <<endl;
       m.unlock();
       std::this_thread::sleep_for(std::chrono::milliseconds(time1));
     }


}


int main(){


int n; // number of threads
//cin>>n;
int k;
//cin>>k; 
double p;
//cin>>p;
int lambda;
//cin>>lambda;
ifstream infile{"inp-params.txt",std::ifstream::in};
    if(infile.fail()){
        cout<<"inp-params.txt file not found";
        exit(1);
    }
infile>>n>>k>>lambda>>p;
cout<<"Number of threads: "<<n<<endl;
cout<<"Number of iteration: "<<k<<endl;
cout<<"probability: "<<p<<endl;
cout<<"lambda: "<<lambda<<endl;    
thread t[n];
AtomicMRMW obj(n,17);
ofstream fout("AtomicMRMW.txt");
for(int i = 0;i<n; i++){
  t[i] = std::thread(testatomic,k,p,lambda,i,obj,n);
}
for(int i=0; i<n; i++){
  t[i].join();
}
cout<<" Average reading time for read operation " <<(totalreadtime + 0.0)/(total_read) <<" ns \n";
cout<<" Average writing time for write operation "<<(totalwritetime + 0.0)/(total_write) << " ns \n";
cout<<" Average waiting time for read or write operation "<<(totalTime + 0.0)/(n * k) <<" us "<<endl;

}

