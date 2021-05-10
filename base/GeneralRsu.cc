#include "veins/modules/Prext/base/messages/WAVEBeacon_m.h"
#include "veins/modules/Prext/base/messages/VehicleReply_m.h"
#include <stdio.h>
#include <veins/modules/Prext/base/PrivBase.h>
#include "veins/base/utils/FindModule.h"
#include <veins/modules/Prext/base/GeneralRsu.h>
#include <veins/modules/Prext/base/FakeBeaconGenerator.h>


Define_Module(GeneralRsu);

void GeneralRsu::initialize(int stage) {
    BaseApplLayer::initialize(stage);
    if (stage == 0) {
        myId = getParentModule() -> getIndex();
        timestepIdx = 1;
        nBcn = 0;
        encounteredPsynms.clear();

//        tracker = FindModule<VehicleTracker*>::findSubModule(getParentModule()->getParentModule());
//        ASSERT(tracker);
        fbg = FindModule<FakeBeaconGenerator*>::findSubModule(getParentModule()->getParentModule());
        ASSERT(fbg);
        sBcns = registerSignal("nbcn");
        sPsynms = registerSignal("npsynm");
    }
}
void GeneralRsu::handleWaveBeacon(WAVEBeacon* bsm) {

//    WAVEBeacon* bsm = dynamic_cast<WAVEBeacon*>(msg);
//   if(bsm != 0) {
        if (bsm->getKind() == PrivateApplMessageKinds::WAVE_BEACON ) {       //if beacon is encrypted, then it is supposed to be unreadable by the eavesdropper
                //output all beacons to make sure all caught
                char filename[50];
                sprintf(filename, "%d.txt", bsm->getSenderAddress());
                outfile.open(filename,ios::out|ios::app);
                outfile.setf(ios::fixed, ios::floatfield);
                outfile.precision(4);
                outfile << bsm->getTimestamp().trunc(SimTimeUnit::SIMTIME_MS) << "\t"
                            << bsm->getSenderAddress() << "\t"
                            << bsm->getSenderPos().x << "\t"
                            << bsm->getSenderVel().x << "\t"
                            << bsm->getSenderPos().y << "\t"
                            << bsm->getSenderVel().y << "\t"
                            << bsm->getSenderPsynm() << "\t"
                            << bsm->getSenderAngle() << "\t"
                            << bsm->getIsEncrypted() << "\t"
                            << getParentModule()->getIndex() << endl;
                outfile.close();

                fbg->handleWAVEBeacon(bsm);

//        bcnByVeh[bsm->getTimestamp().dbl()] = bsm2struct(bsm);
//        bcnByVeh.insert(pair<double, bcnNoEn>(bsm->getTimestamp().dbl(), bsm2struct(bsm)));

        }

//    }

}


void GeneralRsu::handleVehicleReply(VehicleReply* vrply){
    fbg->storeReply(vrply);
}

void GeneralRsu::handleLowerMsg(cMessage *msg)
{
    if (msg->getKind() == PrivateApplMessageKinds::WAVE_BEACON) {
        //if received message is wave beacon
        handleWaveBeacon(dynamic_cast<WAVEBeacon*>(msg));
        }
    else if (msg->getKind() == PrivLayerMessageKinds::VEHICLE_REPLY) {
        //if received message is vehicle reply
        handleVehicleReply(dynamic_cast<VehicleReply*>(msg));
    }
    delete msg;
}


void GeneralRsu::finish() {
//    outfile.close();
    emit(sBcns, nBcn);
    emit(sPsynms, encounteredPsynms.size());



}

