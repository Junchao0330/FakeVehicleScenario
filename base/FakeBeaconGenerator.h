#ifndef FAKEBEACONGENERATOR_H
#define FAKEBEACONGENERATOR_H

#include <veins/base/modules/BaseApplLayer.h>
#include <veins/modules/Prext/include/kalmanTrack.h>
#include <veins/modules/mobility/traci/TraCIMobility.h>
#include <fstream>
#include <math.h>
#include <set>
using namespace std;

/**
 * Eavesdropper  Demo
 */
class FakeBeaconGenerator : public BaseApplLayer {
    public:
        virtual void initialize(int stage);
        virtual void finish();
        virtual void handleWAVEBeacon(WAVEBeacon* bsm);
        virtual void storeReply(VehicleReply* vrply);
        virtual void handleReply(VehicleReply* vrply);

    protected:
        virtual double angle_diff(double a, double b);
        virtual int findTargetMxZ(int id, double angle);
        bcnNoEn bsm2struct(WAVEBeacon* bsm);
        vreply vrply2struct(VehicleReply* vrply);
        ofstream outfile;
        ifstream infile;
        map<double, bcnNoEn> bcnByVeh;
        map<double, bcnNoEn> vNeedFake;
        map<double, vreply> vehreply;

        map<int, double> diff0;
        map<int, double> diff1;
        map<int, double> diff2;
        map<int, double> diff3;
        map<int, double> diff4;
        map<int, double> diff5;

        double endx, endy, endAngle, tarx, tary;
        string iniPoint, tarRoad;
        double iniSpeed;
        int vehicleID, MixZoneID, targetMxZ;
        uint64_t psynm;
        double fakeBcnST;
        bool handleAlready;

        int fRouteCount = 0;
        map<std::string, FakeInfo> allFakeInfo;
        virtual map<std::string, FakeInfo> readXML(int fCount);

};


#endif
