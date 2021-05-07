#ifndef GENERALRSU_H
#define GENERALRSU_H

#include <veins/base/modules/BaseApplLayer.h>
#include <fstream>
#include <veins/modules/Prext/base/FakeBeaconGenerator.h>
#include <veins/modules/Prext/base/adversary/VehicleTracker.h>
#include <set>
using namespace std;

/**
 * Eavesdropper  Demo
 */
class GeneralRsu : public BaseApplLayer {
    public:
        virtual void initialize(int stage);
        virtual void finish();
        virtual void handleLowerMsg(cMessage * msg);
        void handleWaveBeacon(WAVEBeacon* bsm);
        void handleVehicleReply(VehicleReply* vrply);

    protected:
        ofstream outfile;
        FakeBeaconGenerator * fbg;
        int myId, timestepIdx;
        cMessage * RSUtrackBSMEvt = 0;
        simsignal_t sBcns;
        simsignal_t sPsynms;
        set<uint64_t> encounteredPsynms;

        unsigned int nBcn;
};

#endif

