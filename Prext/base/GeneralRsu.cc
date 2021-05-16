#include "veins/modules/Prext/base/messages/WAVEBeacon_m.h"
#include "veins/modules/Prext/base/messages/VehicleReply_m.h"
#include <stdio.h>
#include "veins/base/utils/FindModule.h"
#include <veins/modules/Prext/base/GeneralRsu.h>
#include <veins/modules/Prext/base/FakeBeaconGenerator.h>
#include <iostream>


Define_Module(GeneralRsu);

void GeneralRsu::initialize(int stage) {
    BaseApplLayer::initialize(stage);
    if (stage == 0) {
        myId = getParentModule() -> getIndex();
        timestepIdx = 1;
        headerLength = par("headerLength");
        dataLength = par("dataLength");
        binterval = par("beaconInterval");
        nBcn = 0;
        encounteredPsynms.clear();

        fbg = FindModule<FakeBeaconGenerator*>::findSubModule(getParentModule()->getParentModule());
        ASSERT(fbg);
        fakeBeaconEvt = new cMessage("fake beacon evt", PrivateApplMessageKinds::FAKE_BEACON_EVT);
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
    fakeBeacon = fbg->storeReply(vrply);
    if (fakeBeacon.begin() != fakeBeacon.end()){
            storeFakeBeacon(fakeBeacon);
            fakeBeacon.clear();
    }

    fakeBeacon.clear();

}


void GeneralRsu::storeFakeBeacon(map<double, FakeInfo> fakeBea){
    allFakeBeaconMap.insert(fakeBea.begin(), fakeBea.end());
        if (!scheduled){
        scheduleAt(allFakeBeaconMap.begin()->first, fakeBeaconEvt);
        scheduled = true;
        }


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


void GeneralRsu::handleSelfMsg(cMessage* msg) {

    switch (msg->getKind()) {
        case PrivateApplMessageKinds::FAKE_BEACON_EVT: {
            //for(map<double, FakeInfo>::iterator itmap = allFakeBeaconMap.begin();
            //                 itmap != allFakeBeaconMap.end(); itmap++) {
            //                 cout << itmap->first << " " << itmap->second.positionx << " " << itmap->second.positiony << endl;
            //                 if ((simTime() - itmap->first <= 1)&&(simTime() - itmap->first > 0)) {
            //map<double, FakeInfo>::iterator itmap = allFakeBeaconMap.find(simTime().dbl());
            map<double, FakeInfo>::iterator itmap = allFakeBeaconMap.begin();
                                 WAVEBeacon* fbsm = new WAVEBeacon("BSM");
                                     fbsm->addBitLength(headerLength);
                                     fbsm->addByteLength(dataLength);
                                     fbsm->setKind(PrivateApplMessageKinds::WAVE_BEACON);
                                     fbsm->setChannelNumber(Channels::CCH);
                                     fbsm->setSenderAddress(99);
                                     fbsm->setRecipientAddress(0);
                                     fbsm->setSerial(-1);
                                     fbsm->setWsmData("");
                                     fbsm->setWsmLength(headerLength+dataLength);

                                     fbsm->setTimestamp(simTime());
                                     //TODO:check if pseudonym existed
                                     fbsm->setSenderPsynm((uint64_t)(myId+100) << 32);
                                     fbsm->setSenderPos(Coord(itmap->second.positionx, itmap->second.positiony, 1.895));
                                     if (itmap->second.angle < 180){
                                         double tempa = M_PI*(itmap->second.angle)/180;
                                         fbsm->setSenderAngle(tempa);
                                         fbsm->setSenderVel(Coord((itmap->second.speed)*sin(tempa), (itmap->second.speed)*cos(tempa), 0));
                                     }
                                     else{
                                         double tempb = M_PI*(itmap->second.angle-360)/180;
                                         fbsm->setSenderAngle(tempb);
                                         fbsm->setSenderVel(Coord((itmap->second.speed)*sin(tempb), (itmap->second.speed)*cos(tempb), 0));
                                     }
                                     sendDelayedDown(fbsm, 0.05);
                                     allFakeBeaconMap.erase(allFakeBeaconMap.begin());
                                     //cout<<allFakeBeaconMap.size()<<endl; //check map size
                                     if(allFakeBeaconMap.begin()!=allFakeBeaconMap.end()) { //prevent null map
                                     scheduleAt(allFakeBeaconMap.begin()->first, fakeBeaconEvt);
                                     }


            break;
        }
        default: {
            if (msg)
                EV << "APP: Error: Got Self Message of unknown kind! Name: " << msg->getName() << endl;
            break;
        }
    }

}



void GeneralRsu::finish() {
//    outfile.close();
    emit(sBcns, nBcn);
    emit(sPsynms, encounteredPsynms.size());



}

