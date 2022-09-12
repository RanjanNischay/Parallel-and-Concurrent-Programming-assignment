#include <atomic>
#include <thread>
#include <iostream>
#include <chrono>
#include <ctime>
#include <mutex>
#include <unistd.h>
#include <random>
#include <fstream>

using namespace std;



class Register{
    virtual int read() = 0;
    virtual void write(int value) = 0;
};


class StampedValue{
    
     
    public:
    long stamp;
    int value;
    
    StampedValue(){
        stamp = 0;
        value = 0;
    }
    StampedValue(int v){
        stamp = 0;
        value = v;
    }
    StampedValue(long stamp, int value){
        this->stamp = stamp;
        this->value = value;
    }
    StampedValue(const StampedValue &p1){
        stamp = p1.stamp;
        value = p1.value;
    }
    StampedValue* MAXStampedValue(StampedValue* x, StampedValue* y){
        if(x->stamp > y->stamp) return x;
        else return y;
    }
    //StampedValue* MIN_VALUE = new StampedValue();
    
};
//thread_local static long lastStamp;
//thread_local static StampedValue lastRead;

class AtomicSRSW:public Register{
  public:
  static thread_local long lastStamp;
  static thread_local  StampedValue *lastRead;
  StampedValue* r_value;
  
  AtomicSRSW(){
      r_value = new StampedValue();
      lastStamp = 0;
      //lastRead = r_value;
      lastRead->stamp = r_value->stamp;
      lastRead->value = r_value->value;
      
  }
  int read(){
    StampedValue* value = r_value;
    StampedValue* last = lastRead;
    StampedValue* result = new StampedValue();
    result = result->MAXStampedValue(value,last);
    lastRead->stamp = result->stamp;
    lastRead->value = result->value;
    //cout<<result->value<<endl;
    return result->value;
 }

 void write(int v){
    long stamp = lastStamp + 1;
    r_value = new StampedValue(stamp,v);
    
    lastStamp = stamp;
 }

};
thread_local long AtomicSRSW::lastStamp(0);
thread_local StampedValue* AtomicSRSW::lastRead(new StampedValue());


/*
mutex m;
long long totalTime,totalreadtime,totalwritetime,total_read=0,total_write = 0;
void testatomic(int k, double p, int lambda,int i,int n,AtomicSRSW obj){
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
     
    ofstream fout("AtomicSRSW.txt", ios::app);
    m.lock();
    fout << j << "th action requested at " << ctime(&now1) << " by thread "<< tid <<endl; 
    m.unlock();
    string str;
    if(action == 0){ // action =0 means read
        auto rs_time = std::chrono::high_resolution_clock::now();
        locVar = obj.read();
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
       obj.write(locVar);
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
AtomicSRSW obj;
ofstream fout("AtomicSRSW.txt");
for(int i = 0;i<n; i++){
  t[i] = std::thread(testatomic,k,p,lambda,i,n,obj);
}
for(int i=0; i<n; i++){
  t[i].join();
}
cout<<" Average reading time for read operation " <<(totalreadtime + 0.0)/(total_read) <<" ns \n";
cout<<" Average writing time for write operation "<<(totalwritetime + 0.0)/(total_write) << " ns \n";
cout<<" Average waiting time for read or write operation "<<(totalTime + 0.0)/(n * k) <<" us "<<endl;

}

*/