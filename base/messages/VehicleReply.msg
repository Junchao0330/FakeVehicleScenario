cplusplus {{
#include <veins/base/utils/Coord.h>
#include <veins/modules/messages/WaveShortMessage_m.h>
}}
class noncobject Coord;
message WaveShortMessage;
message VehicleReply extends WaveShortMessage {
    int enteredMixZone; //The Mix-Zone the vehicle is going to leave 
    Coord senderVel;
    double senderAngle; 
    string edge; 
    uint64_t senderPsynm;    
}