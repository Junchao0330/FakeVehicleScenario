#include <string>
#include <fstream>
#include <list>
#include <stdexcept>
#include <map>
#include <utility>

#include "veins/base/modules/BaseMobility.h"
#include "veins/base/utils/FindModule.h"
#include "veins/modules/mobility/traci/TraCIScenarioManager.h"
#include "veins/modules/mobility/traci/TraCICommandInterface.h"
using namespace std;

//coords of mix-zones
int cxzx[6] = {780,800,1260,2050,2100,1960};
int cxzy[6] = {1700,2500,1370,1300,1925,2300};
string mxz2Road[6] = {"341824991#0", "-107023497", "145598915#0", "292960292", "31019214", "143927076"};

map<int, double> mxz0;
map<int, double> mxz1;
map<int, double> mxz2;
map<int, double> mxz3;
map<int, double> mxz4;
map<int, double> mxz5;

inline void createMap(){
    for(int i=0;i<5;i++){
        mxz0.insert(std::pair<int, double> (i+1, atan2((cxzy[i+1]-cxzy[0]),(cxzx[i+1]-cxzx[0]))));
    };
    for(int i=1;i<5;i++){
        mxz1.insert(pair<int, double> (i+1, atan2((cxzy[i+1]-cxzy[1]),(cxzx[i+1]-cxzx[1]))));
    };
    mxz1.insert(pair<int, double> (0, atan2((cxzy[0]-cxzy[1]),(cxzx[0]-cxzx[1]))));
    for(int i=2;i<5;i++){
        mxz2.insert(pair<int, double> (i+1, atan2((cxzy[i+1]-cxzy[2]),(cxzx[i+1]-cxzx[2]))));
    };
    mxz2.insert(pair<int, double> (0, atan2((cxzy[0]-cxzy[2]),(cxzx[0]-cxzx[2]))));
    mxz2.insert(pair<int, double> (1, atan2((cxzy[1]-cxzy[2]),(cxzx[1]-cxzx[2]))));
    for(int i=3;i<5;i++){
        mxz3.insert(pair<int, double> (i+1, atan2((cxzy[i+1]-cxzy[3]),(cxzx[i+1]-cxzx[3]))));
    };
    mxz3.insert(pair<int, double> (0, atan2((cxzy[0]-cxzy[3]),(cxzx[0]-cxzx[3]))));
    mxz3.insert(pair<int, double> (1, atan2((cxzy[1]-cxzy[3]),(cxzx[1]-cxzx[3]))));
    mxz3.insert(pair<int, double> (2, atan2((cxzy[2]-cxzy[3]),(cxzx[2]-cxzx[3]))));
    for(int i=4;i<5;i++){
        mxz4.insert(pair<int, double> (i+1, atan2((cxzy[i+1]-cxzy[4]),(cxzx[i+1]-cxzx[4]))));
    };
    mxz4.insert(pair<int, double> (0, atan2((cxzy[0]-cxzy[4]),(cxzx[0]-cxzx[4]))));
    mxz4.insert(pair<int, double> (1, atan2((cxzy[1]-cxzy[4]),(cxzx[1]-cxzx[4]))));
    mxz4.insert(pair<int, double> (2, atan2((cxzy[2]-cxzy[4]),(cxzx[2]-cxzx[4]))));
    mxz4.insert(pair<int, double> (3, atan2((cxzy[3]-cxzy[4]),(cxzx[3]-cxzx[4]))));
    for(int i=5;i>0;i--){
    mxz5.insert(pair<int, double> (i-1, atan2((cxzy[i-1]-cxzy[5]),(cxzx[i+1]-cxzx[5]))));
    };
}

/*
*.MxZ[0].mobility.x = 780
*.MxZ[0].mobility.y = 1700

*.MxZ[1].mobility.x = 800
*.MxZ[1].mobility.y = 2500

*.MxZ[2].mobility.x = 1260
*.MxZ[2].mobility.y = 1370

*.MxZ[3].mobility.x = 2050
*.MxZ[3].mobility.y = 1300

*.MxZ[4].mobility.x = 2100
*.MxZ[4].mobility.y = 1925

*.MxZ[5].mobility.x = 1960
*.MxZ[5].mobility.y = 2300
*/



