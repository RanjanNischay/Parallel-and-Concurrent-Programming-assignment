#include<bits/stdc++.h>
#include <unistd.h>
using namespace std;
using namespace std::chrono;

class Snapshot{
  virtual void update(int l,int v) = 0;
  virtual int* snapshot() = 0;
};

class StampedValue{
    
     
    public:
    long stamp;
    int value;
    
    StampedValue() noexcept {}
    StampedValue(int v){
        stamp = 0;
        value = v;
    }
    StampedValue(long stamp, int value){
        this->stamp = stamp;
        this->value = value;
    }
    StampedValue* MAXStampedValue(StampedValue* x, StampedValue* y){
        if(x->stamp > y->stamp) return x;
        else return y;
    }
    //StampedValue* MIN_VALUE = new StampedValue();
    
};

class Obstructionfree:public Snapshot{
   private:
    atomic<StampedValue>* a_table;
    int capacity;
    atomic<StampedValue>* collect(){
       atomic<StampedValue>* copy = new atomic<StampedValue>[capacity];
       for(int j = 0; j<capacity;j++){
           StampedValue copyval = a_table[j].load();
           copy[j].store(copyval);
       }
       return copy;
     }
    public:
     Obstructionfree(int m,int val){
         capacity = m;
         a_table = new atomic<StampedValue>[m];
         for(int i =0; i<m;i++){
             StampedValue ob(val);
             a_table[i].store(ob);
         }
     }

     void update(int l,int v){
         int me = l;
         StampedValue oldvalue = a_table[me].load();
         StampedValue newvalue(oldvalue.stamp+1,v);
         a_table[me].store(newvalue);

     }
     //bool compare(StampedSnap obj1, StampedSnap obj2){
       //        return ((obj1.getPid() == obj2.getPid()) && (obj1.getsn() == obj2.getsn()) && (obj1.getval() == obj2.getval()));
       //}

       bool comparearray(atomic<StampedValue>* oldcopy, atomic<StampedValue>* newcopy){
           int len = capacity;
           for(int i =0;i<len;i++){
                 StampedValue temp1 = oldcopy[i].load();
                 StampedValue temp2 = newcopy[i].load();
                 if(temp1.stamp != temp2.stamp || temp1.value != temp2.value) return false;
           }
           return true;
       }
     
     int* snapshot(){
         atomic<StampedValue>* oldcopy;
         atomic<StampedValue>* newcopy;
         oldcopy = collect();
         bool flag = false;
         while(true){
             newcopy = collect();
             if(comparearray(oldcopy,newcopy)){
                 flag = true;
                 break;
             }
             else{
                
                 //oldcopy = newcopy;
                 for(int j=0; j<capacity; j++){
                  StampedValue x = newcopy[j].load();
                  oldcopy[j].store(x);
                  
                 }
                 continue;
             }
             
         }
         int* result = new int [capacity];
         for(int i =0; i<capacity; i++){
             result[i] = newcopy[i].load().value;
         }
         return result;
     }


     

     

};

int nw,ns,m,k; float uw,us;
Obstructionfree* obj;
atomic<bool> term;
vector<int64_t> tp1;


void writer(int id){
  int v, pid,l;
  int time1;
  pid = id;
  while(!term){
    v = rand()%1000; // v is for value
    l = rand()%m; // l is for location
    obj->update(l,v);
    auto complTime = std::chrono:: high_resolution_clock::now();
    //std::time_t now2 = std::chrono::system_clock::to_time_t(complTime);
    auto duration1 = chrono::duration_cast<chrono::microseconds>(complTime.time_since_epoch()).count();
    //cout<<"Thread: " <<l <<" writing value: "<<v<<" at location " << l<<" at "<< ctime(&now2)<<endl;
    ofstream fout("obstruction-free MRMW.txt",ios::app);
    string str = " Th " + to_string(pid) + "'s write of "+to_string(v)+" on location " + to_string(l) + " at "+ to_string(duration1);
    fout<<str<<endl;
    double const exp_dist_lambda_1 = uw;
    int seed = chrono::system_clock::now().time_since_epoch().count();
    default_random_engine rg(seed);
    exponential_distribution <double> dist_1(exp_dist_lambda_1);
    double t1d = dist_1(rg) + 0.5;
    time1 = (int)(t1d);
    std::this_thread::sleep_for(std::chrono::microseconds(time1));
    
  }
  
}

void snap(int id){
  int i = 0;
  int time2;
  while(i<k){
   auto beginCollect = std::chrono:: high_resolution_clock::now();
   int *arr = obj->snapshot();
   auto endCollect = std::chrono:: high_resolution_clock::now();
   auto duration1 = chrono::duration_cast<chrono::microseconds>(endCollect-beginCollect).count();
   //string str = "Snapshot Thr’s snapshot: ";
    tp1.push_back(duration1);
    //std::time_t now2 = std::chrono::system_clock::to_time_t(endCollect);
    auto duration2 = chrono::duration_cast<chrono::microseconds>(endCollect.time_since_epoch()).count();
    ofstream fout("obstruction-free MRMW.txt",ios::app);
    string str = "Sanpshot thr " + to_string(id) + " : ";
    for(int j =0; j<m;j++){
      str += " l" + to_string(j) +":"+ to_string(arr[j]) + " ";
    } 
    str += "which finishes at " + to_string(duration2);
    fout<<str<<endl;
    str.clear();
   double const exp_dist_lambda_1 = us;
    int seed = chrono::system_clock::now().time_since_epoch().count();
    default_random_engine rg(seed);
    exponential_distribution <double> dist_1(exp_dist_lambda_1);
    double t1d = dist_1(rg) + 0.5;
    time2 = (int)t1d;
    std::this_thread::sleep_for(std::chrono::microseconds(time2));
    i++;

  }
}

int main(){
  //cin>>nw>>ns>>m>>uw>>us>>k;
  ifstream infile{"inp-params.txt",std::ifstream::in};
    if(infile.fail()){
        cout<<"inp-params.txt file not found";
        exit(1);
    }
   infile>>nw>>ns>>m>>uw>>us>>k;
   cout<<"No of writer thread: "<<nw<<endl;
   cout<<"No.of snapshot thread: "<<ns<<endl;
   cout<<"No of snapshot objects: "<<m<<endl;
   cout<<"uw: "<<uw<<endl;
   cout<<"us: "<<us<<endl;
   cout<<"No. of iteration: "<<k<<endl;
  int n = nw+ns;
  
   obj = new Obstructionfree(m,0);
   ofstream fout("obstruction-free MRMW.txt");
   term = false;
   thread write[nw];
   for(int i =0;i<nw;i++){
      write[i] = std::thread(writer,i);
    }
    thread snaps[ns];
    for(int i=0;i<ns;i++){
      snaps[i] = std::thread(snap,i);
    }
     for(int i=0;i<ns;i++){
      snaps[i].join();
    }
    //term.store(true);
    term = true;
    for(int i=0;i<nw;i++){
      write[i].join();
    }
    long avgtime = 0,worsttime;
    for(int i =0;i < tp1.size();i++){
        avgtime += tp1[i];
    }
    avgtime = (avgtime + 0.0) / (ns * k);
    cout<<"average time : " <<avgtime<<" us "<<endl;
    worsttime = *max_element(tp1.begin(),tp1.end());
    cout<<"worst time : "<<worsttime<<" us "<<endl;
    return 0;
   


}