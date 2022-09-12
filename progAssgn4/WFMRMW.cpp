#include<bits/stdc++.h>
#include <unistd.h>
#include <string>
using namespace std;
using namespace std::chrono;
mutex mt;

class Snapshot{
  virtual void update(int l,int v,int tid) = 0;
  virtual int* snapshot() = 0;
};

class StampedSnap{
    public:
        // Data Variables
        int value;
        int pid;
        int snap;
        // Initialisation Functions
        StampedSnap() noexcept {}
        StampedSnap(int value){
            this->value = value;
            this->pid = 0;
            this->snap = 0;
        }
        StampedSnap(int value, int pid, int snap) {
            this->value = value;
            this->pid = pid;
            this->snap = snap;
        }
        int getPid(){
          return pid;
        }
        int getval(){
          return value;
        }
        int getsn(){
          return snap;
        }
        
};

class waitfreeMRMWsnapshot:public Snapshot{
    private:
      int m; // number of components of snapshot object
      int n; // number of threads
      atomic<int> *sn;
      atomic<StampedSnap> *REG;
      atomic<int*>* HELPSNAP;
    public:
      waitfreeMRMWsnapshot() noexcept {}
      waitfreeMRMWsnapshot(int m, int n){
          this->m = m;
          this->n = n;
          sn = new atomic<int> [n];
          REG = new atomic<StampedSnap>[m]; // array of MWMR atomic registers
          HELPSNAP = new atomic<int*>[n];   // array of SWMR atomic registers
          for(int i = 0; i<m; i++){
            StampedSnap obj(0);
            REG[i].store(obj);
          }
          for(int  i =0; i<n;i++){
            HELPSNAP[i] = new int[m];
          }
          
      }

      void update(int l , int v, int tid){
        sn[tid]++;
        int x = sn[tid].load();
        StampedSnap obj(v,tid,x);
        REG[l].store(obj);
        int *temp = snapshot();
        
        HELPSNAP[tid].store(temp);
      }

      atomic<StampedSnap>* collect(){
        atomic<StampedSnap>* temp = new atomic<StampedSnap> [m];
        for(int j = 0; j<m;j++){
          StampedSnap obj1 = REG[j].load();
          StampedSnap obj2(obj1.value,obj1.pid,obj1.snap);
          temp[j].store(obj2);
        }
        return temp;
      }
      
      bool compare(StampedSnap obj1, StampedSnap obj2){
               return ((obj1.getPid() == obj2.getPid()) && (obj1.getsn() == obj2.getsn()) && (obj1.getval() == obj2.getval()));
       }
      int* snapshot(){
        set<int> can_help;
        atomic<StampedSnap>* aa;
        atomic<StampedSnap>* bb;
        aa = collect();
        //bool repeat = false;
        
        while(true){
          bb = collect();
         
          int counter = 0;
          for(int j =0;j<m;j++){
            if(compare(aa[j],bb[j])){
              counter++;
            }
            }
            if(counter == m){
              int* result = new int[m];
              for(int  j= 0; j<m;j++){
              result[j] = bb[j].load().value;
             }
             return result;
            }
            else{
              for(int j =0;j<m;j++){
                if(!compare(aa[j],bb[j])){
                  int pid = bb[j].load().pid;
                  if(can_help.find(pid) != can_help.end()){
                  int *result = new int[m];
                  for(int i = 0; i<m; i++){
                  result[i] = HELPSNAP[pid][i];
                 }
                 return result;            
                }
                else{
                  can_help.insert(pid);
                }
                }

              }
            }
            
            aa = bb;

        }
      }

};
int nw,ns,m,k; float uw,us;
waitfreeMRMWsnapshot* MRMWsnapobj;
bool term;
stringstream ss;
//vector<vector<pair<int,time_t>>> logData;
vector<int64_t> tp1;





void writer(int id){
  int v, pid,l;
  int time1;
  pid = id; // pid means process id
  while(!term){
    //srand(time(0));
    v = rand()%1000; // l is for location
    l = rand()%m;  // v is for value
    MRMWsnapobj->update(l,v,pid);
    auto complTime = std::chrono:: high_resolution_clock::now();
    //std::time_t now2 = std::chrono::system_clock::to_time_t(complTime);
    auto duration1 = chrono::duration_cast<chrono::microseconds>(complTime.time_since_epoch()).count();
    //mt.lock();
    //cout<<"Thread: " <<pid <<"'s writing value: "<<v<<" at location " << l<<" at "<< ctime(&now2)<<endl;
    ofstream fout("wait-free MRMW.txt",ios::app);
    string str = " Th " + to_string(pid) + "'s write of "+to_string(v)+" on location " + to_string(l) + " at "+ to_string(duration1);
    //logData[id].push_back(make_pair(v,now2));
    fout<<str<<endl;
    //mt.unlock();
    double const exp_dist_lambda_1 = uw;
    int seed = chrono::system_clock::now().time_since_epoch().count();
    default_random_engine rg(seed);
    exponential_distribution <double> dist_1(exp_dist_lambda_1);
    double t1d = dist_1(rg) + 0.5;
    time1 = (int)t1d;
    std::this_thread::sleep_for(std::chrono::microseconds(time1));
    
  }
  
}

void snap(int id){
  int i = 0;
  int time2;
  while(i<k){
    auto beginCollect = std::chrono:: high_resolution_clock::now();
    int *arr = MRMWsnapobj->snapshot();
    auto endCollect = std::chrono:: high_resolution_clock::now();
    auto duration2 = chrono::duration_cast<chrono::microseconds>(endCollect - beginCollect).count();
    //std::time_t now1 = std::chrono::system_clock::to_time_t(duration2);
    auto duration1 = chrono::duration_cast<chrono::microseconds>(endCollect.time_since_epoch()).count();
    tp1.push_back(duration2);
    //std::time_t now2 = std::chrono::system_clock::to_time_t(endCollect);
    ofstream fout("wait-free MRMW.txt",ios::app);
    string str = "Sanpshot thr " + to_string(id) + " : ";
    for(int j =0; j<m;j++){
      str += " l" + to_string(j) +":"+ to_string(arr[j]) + " ";
    } 
    str += "which finishes at "+to_string(duration1);
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
   MRMWsnapobj = new waitfreeMRMWsnapshot(m,n);
   ofstream fout("wait-free MRMW.txt");
  
    //term.store(false); 
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

