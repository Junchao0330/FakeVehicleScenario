#ifndef GENERALRSU_H
#define GENERALRSU_H

#include <veins/base/modules/BaseApplLayer.h>
#include <fstream>
#include <veins/modules/Prext/base/FakeBeaconGenerator.h>
#include <veins/modules/Prext/base/adversary/VehicleTracker.h>
#include <veins/modules/Prext/include/kalmanTrack.h>
#include "veins/base/connectionManager/ChannelAccess.h"
#include <veins/modules/mac/ieee80211p/WaveAppToMac1609_4Interface.h>
#include <veins/modules/Prext/base/PrivBase.h>
#include <veins/modules/utility/Consts80211p.h>
#include <veins/base/modules/BaseMobility.h>
#include <set>
#include <math.h>
using namespace std;

/**
 * Eavesdropper  Demo
 */
class GeneralRsu : public BaseApplLayer {
    public:
        virtual void initialize(int stage);
        virtual void finish();
        virtual void handleLowerMsg(cMessage * msg);
        virtual void handleSelfMsg(cMessage* msg);
        void handleWaveBeacon(WAVEBeacon* bsm);
        void handleVehicleReply(VehicleReply* vrply);
        void storeFakeBeacon(map<double, FakeInfo> fakeBea);
        void sendFakeBeacon(map<double, FakeInfo> fakeB);

    protected:
        ofstream outfile;
        FakeBeaconGenerator * fbg;
        int myId, timestepIdx;
        cMessage * RSUtrackBSMEvt = 0;
        cMessage * fakeBeaconEvt = 0;
        simsignal_t sBcns;
        simsignal_t sPsynms;
        set<uint64_t> encounteredPsynms;
        int headerLength;
        int dataLength;
        int binterval;
        bool scheduled = false;

        unsigned int nBcn;
        double delayCount = 0.01;
        map<double, FakeInfo> fakeBeacon;
        map<double, FakeInfo> allFakeBeaconMap;
};

#endif

