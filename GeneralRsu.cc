#include "veins/modules/Prext/base/messages/WAVEBeacon_m.h"
#include <stdio.h>
#include <veins/modules/Prext/base/PrivBase.h>
#include "veins/base/utils/FindModule.h"
#include <veins/modules/Prext/base/GeneralRsu.h>
Define_Module(GeneralRsu);

void GeneralRsu::initialize(int stage) {
    BaseApplLayer::initialize(stage);
    if (stage == 0) {
        myId = getParentModule() -> getIndex();

        nBcn = 0;
        encounteredPsynms.clear();

//        tracker = FindModule<VehicleTracker*>::findSubModule(getParentModule()->getParentModule());
//        ASSERT(tracker);


        sBcns = registerSignal("nbcn");
        sPsynms = registerSignal("npsynm");
    }
}
void GeneralRsu::handleLowerMsg(cMessage* msg) {

    WAVEBeacon* bsm = dynamic_cast<WAVEBeacon*>(msg);
    if(bsm != 0) {
        if (bsm->getKind() == PrivateApplMessageKinds::WAVE_BEACON ) {       //if beacon is encrypted, then it is supposed to be unreadable by the eavesdropper

                char filename[50];
                sprintf(filename, "%d.txt", bsm->getSenderAddress());
                outfile.open(filename,ios::app|ios::out);
                outfile.setf(ios::fixed, ios::floatfield);
                outfile.precision(4);
                dumpWAVEBeacon(outfile, bsm);
            }

    }
    delete msg ;
}

void GeneralRsu::finish() {
    outfile.close();
    emit(sBcns, nBcn);
    emit(sPsynms, encounteredPsynms.size());

}

