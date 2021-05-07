#include "veins/modules/Prext/base/messages/VehicleReply_m.h"
#include <stdio.h>
#include <veins/modules/Prext/base/PrivBase.h>
#include "veins/base/utils/FindModule.h"
#include <veins/modules/Prext/base/FakeBeaconGenerator.h>
#include <veins/modules/Prext/base/AngleofMxZ.h>
#include <iostream>
//#include </home/junchao/veins-veins-4.4/src/veins/base/tinyxml/tinystr.h>
//#include </home/junchao/veins-veins-4.4/src/veins/base/tinyxml/tinyxml.h>

Define_Module(FakeBeaconGenerator);

void FakeBeaconGenerator::initialize(int stage) {
    BaseApplLayer::initialize(stage);
    if (stage == 0) {
        createMap();
        //6 maps validated
/*            outfile.open("mixzonemap.txt",ios::out|ios::app);
            outfile.setf(ios::fixed, ios::floatfield);
            outfile.precision(4);
            map<int, double>::iterator it = mxz0.begin();
               for (; it != mxz0.end(); it++){
                       outfile << it->second << "\t";
               }
            outfile.close();
*/
    }
}

void FakeBeaconGenerator::handleWAVEBeacon(WAVEBeacon* bsm){
    //store all vehicle beacons -- no duplicate
    bcnByVeh[bsm->getTimestamp().dbl()] = bsm2struct(bsm);


}

void FakeBeaconGenerator::storeReply(VehicleReply* vrply) {
    //store all vehicle replies -- no duplicate
//    if(vrply != 0) {
    Enter_Method("storeReply(VehicleReply* vrply) ");

//    }


    map<double, vreply>::iterator it = vehreply.find(vrply->getTimestamp().dbl());
    if (it == vehreply.end()){
        vehreply.insert(pair<double, vreply>(vrply->getTimestamp().dbl(), vrply2struct(vrply)));
        handleReply(vrply);//call only once for duplicate replies
    }
}

void FakeBeaconGenerator::handleReply(VehicleReply* vrply) {
    Enter_Method("handleReply(VehicleReply* vrply) ");
    //last position from the vehicle, used as source point of FAKE ROUTE
    endx = vrply->getSenderPos().x;
    endy = vrply->getSenderPos().y;
    iniPoint = vrply->getEdge();//get source point of fake vehicle
    iniSpeed = fabs((vrply->getSenderVel().y)/sin(fabs(vrply->getSenderAngle())));//get initial speed of fake vehicle
    //cout<<"startPoint =" << iniPoint << endl;
    //cout<<"startSpeed =" << iniSpeed << endl;

    endAngle = vrply->getSenderAngle();//get last angle of vehicle to find a destination

    vehicleID = vrply->getSenderAddress(); //which vehicle needs fake beacon
    MixZoneID = vrply->getEnteredMixZone(); //encountered which mix-zone
    psynm = vrply->getSenderPsynm(); //to avoid further psynm the same as current one
    fakeBcnST = vrply->getTimestamp().dbl(); //fake beacon starts broadcast at this time

    map<double, bcnNoEn>::iterator it = bcnByVeh.begin();
              for (; it != bcnByVeh.end(); it++){
                  if(it->second.vehId == vehicleID){

                      vNeedFake.insert(pair<double, bcnNoEn>(it->second.timestampe,it->second));
                  }
              }

//output all maps for every replies -- no duplicate
    char filename[50];
    sprintf(filename, "whichvehicle.txt");
                  outfile.open(filename,ios::out|ios::app);
                  outfile.setf(ios::fixed, ios::floatfield);
                  outfile.precision(4);
                  map<double, bcnNoEn>::iterator itt = vNeedFake.begin();
                  outfile << "this is another map" << endl;
                     for (; itt != vNeedFake.end(); itt++){
                             outfile << itt->second.timestampe << "\t"
                                     << itt->second.vehId << "\t"
                                     << itt->second.px << "\t"
                                     << itt->second.vx << "\t"
                                     << itt->second.py << "\t"
                                     << itt->second.vy << "\t"
                                     << itt->second.psynm << "\t"
                                     << itt->second.angle << ";" << endl;}
                  outfile.close();
                  vNeedFake.clear();
    tragetMxZ = findTargetMxZ(MixZoneID, endAngle);//get target mix-zone ID
    //the position of target mix-zone, used as destination point of FAKE ROUTE
    tarx = cxzx[tragetMxZ];
    tary = cxzy[tragetMxZ];
    tarRoad = mxz2Road[tragetMxZ];//get destination of fake vehicle
    //cout<<"traget Mxz =" << tragetMxZ << endl;
    //cout<<"traget road =" << tarRoad << endl;

    fRouteCount++;
    //generate trip.xml with fake source and destination
    char fakeRoute[50];
    sprintf(fakeRoute, "fakeRoute_%d.trips.xml", fRouteCount);
    outfile.open(fakeRoute,ios::out);
    outfile.setf(ios::fixed, ios::floatfield);
    outfile << "<?xml version=\"1.0\"?>" << endl
            << "<!-- generated on 2021-04-21 18:17:31.228801 by $Id: randomTrips.py 19495 2015-12-02 12:36:28Z behrisch $options: -n /home/junchao/veins-veins-4.4/examples/prextdemo/munich.net.xml -e 300 -p 2-->" << endl
            << "<trips>" << endl
            << " "" "" "" " << "<trip id=\"0\" depart=\"" << fakeBcnST << "\" from=\"" << iniPoint << "\" to=\"" << tarRoad << "\" />" << endl
            << "</trips>" << endl;
    outfile.close();

    //generate rou.xml for fake route
    char cmd1[512];
    sprintf(cmd1, "/home/junchao/sumo-0.25.0/src/duarouter/duarouter -n /home/junchao/veins-veins-4.4/examples/prextdemo/munich.net.xml --departspeed %f -t /home/junchao/veins-veins-4.4/examples/prextdemo/fakeRoute_%d.trips.xml -o fakeRoute_%d.rou.xml", fRouteCount, iniSpeed, fRouteCount);
    system(cmd1);

    //modify sumocfg file for each fake route
    FILE * pFile;
      pFile = fopen ( "/home/junchao/veins-veins-4.4/examples/prextdemo/fakeOutput.sumo.cfg" , "rb+" );

      fseek ( pFile , 335 , SEEK_SET );
      fprintf( pFile, "%d", fRouteCount);
      fclose ( pFile );

    //simulate for this fake route to obtain results for each second
    char cmd2[512];
    sprintf(cmd2, "/home/junchao/sumo-0.25.0/src/sumo -c /home/junchao/veins-veins-4.4/examples/prextdemo/fakeOutput.sumo.cfg --fcd-output output%d.xml", fRouteCount);
    system(cmd2);

    //now convert the result to an OMNET-readable xml
    char cmd3[512];
    sprintf(cmd3, "/home/junchao/sumo-0.25.0/tools/traceExporter.py --fcd-input /home/junchao/veins-veins-4.4/examples/prextdemo/output%d.xml --omnet-output omnetOutput%d.xml", fRouteCount, fRouteCount);
    system(cmd3);

    //parse xml file and store essential information

}



bcnNoEn FakeBeaconGenerator::bsm2struct(WAVEBeacon* bsm) {
    bcnNoEn bcnByV;
    bcnByV.timestampe = bsm->getTimestamp().dbl() + 0.0001*bsm->getSenderAddress();
    bcnByV.vehId = bsm->getSenderAddress();
    bcnByV.px = bsm->getSenderPos().x;
    bcnByV.vx = bsm->getSenderVel().x;
    bcnByV.py = bsm->getSenderPos().y;
    bcnByV.vy = bsm->getSenderVel().y;
    bcnByV.psynm = bsm->getSenderPsynm();
    bcnByV.angle = bsm->getSenderAngle();
    return bcnByV;
}

vreply FakeBeaconGenerator::vrply2struct(VehicleReply* vrply) {
    vreply vr;
    vr.timestamp = vrply->getTimestamp().dbl();
    vr.vID = vrply->getSenderAddress();
    vr.psynm = vrply->getSenderPsynm();
    vr.px = vrply->getSenderPos().x;
    vr.py = vrply->getSenderPos().y;
    vr.vx = vrply->getSenderVel().x;
    vr.vy = vrply->getSenderVel().y;
    vr.angle = vrply->getSenderAngle();
    vr.mID = vrply->getEnteredMixZone();

    return vr;
}

double FakeBeaconGenerator::angle_diff(double a, double b){
    double d1, d2;
    d1 = a-b;
    d2 = 2*M_PI - fabs(d1);
    if(d1 > 0)
        d2 *= -1.0;
    if(fabs(d1) < fabs(d2))
        return(fabs(d1));
    else
        return(fabs(d2));
}


void FakeBeaconGenerator::finish() {
//output all vehicle replies -- no duplicate
    char filename2[50];
        sprintf(filename2, "CheckReplyMap.txt");
        outfile.open(filename2,ios::out);
        outfile.setf(ios::fixed, ios::floatfield);
        outfile.precision(4);
    map<double, vreply>::iterator it = vehreply.begin();
       for (; it != vehreply.end(); it++){
           outfile << it->second.timestamp << "\t"
                   << it->second.vID << "\t"
                   << it->second.psynm << "\t"
                   << it->second.px << "\t"
                   << it->second.py << "\t"
                   << it->second.vx << "\t"
                   << it->second.vy << "\t"
                   << it->second.mID << "\t"
                   << it->second.angle << endl;
           }
       outfile.close();

//output all vehicle beacons -- no duplicate
       char filename3[50];
           sprintf(filename3, "beacons.txt");
           outfile.open(filename3,ios::out|ios::app);
           outfile.setf(ios::fixed, ios::floatfield);
           outfile.precision(4);
           map<double, bcnNoEn>::iterator itt = bcnByVeh.begin();
   //           outfile << "veinsdata{" << timestepIdx++ << "} = [" << endl;
              for (; itt != bcnByVeh.end(); itt++){
                      outfile //<< it->first << "\t"
                              << itt->second.timestampe << "\t"
                              << itt->second.vehId << "\t"
                              << itt->second.px << "\t"
                              << itt->second.vx << "\t"
                              << itt->second.py << "\t"
                              << itt->second.vy << "\t"
                              << itt->second.psynm << "\t"
                              << itt->second.angle << ";" << endl;}
   //           outfile << "];" << endl;
           outfile.close();
}

int FakeBeaconGenerator::findTargetMxZ(int id, double angle){
//    cout<<"endangle =" << angle << endl;
    switch(id){
        case 0:{//for Mix-Zone 0
            map<int, double>::iterator it0 = mxz0.begin();
            for (; it0 != mxz0.end(); it0++){
                diff0.insert(std::pair<int, double> (it0->first, angle_diff(it0->second,angle)));
            }
            double minValue = 4;//larger than pie
            map<int, double>::iterator itd0 = diff0.begin();
            for (; itd0 != diff0.end(); itd0++){
                if(itd0->second<minValue)
                minValue = itd0->second;
            }
            map<int, double>::iterator itdd0 = diff0.begin();
            for (; itdd0 != diff0.end(); itdd0++){
                if(itdd0->second == minValue)
                return itdd0->first;
                diff0.clear();
            }
            break;
        }
        case 1:{
            map<int, double>::iterator it1 = mxz1.begin();
                        for (; it1 != mxz1.end(); it1++){
                            diff1.insert(std::pair<int, double> (it1->first, angle_diff(it1->second,angle)));
                        }
                        double minValue = 4;
                        map<int, double>::iterator itd1 = diff1.begin();
                        for (; itd1 != diff1.end(); itd1++){
                            if(itd1->second<minValue)
                            minValue = itd1->second;
                        }
                        map<int, double>::iterator itdd1 = diff1.begin();
                        for (; itdd1 != diff1.end(); itdd1++){
                            if(itdd1->second == minValue)
                            return itdd1->first;
                            diff1.clear();
                        }
                        break;
        }
        case 2:{
            map<int, double>::iterator it2 = mxz2.begin();
                        for (; it2 != mxz2.end(); it2++){
                            diff2.insert(std::pair<int, double> (it2->first, angle_diff(it2->second,angle)));
                        }
                        double minValue = 4;
                        map<int, double>::iterator itd2 = diff2.begin();
                        for (; itd2 != diff2.end(); itd2++){
                            if(itd2->second<minValue)
                            minValue = itd2->second;
                        }
                        map<int, double>::iterator itdd2 = diff2.begin();
                        for (; itdd2 != diff2.end(); itdd2++){
                            if(itdd2->second == minValue)
                            return itdd2->first;
                            diff2.clear();
                        }
                        break;
        }
        case 3:{
            map<int, double>::iterator it3 = mxz3.begin();
                        for (; it3 != mxz3.end(); it3++){
                            diff3.insert(std::pair<int, double> (it3->first, angle_diff(it3->second,angle)));
                        }
                        double minValue = 4;
                        map<int, double>::iterator itd3 = diff3.begin();
                        for (; itd3 != diff3.end(); itd3++){
                            if(itd3->second<minValue)
                            minValue = itd3->second;
                        }
                        map<int, double>::iterator itdd3 = diff3.begin();
                        for (; itdd3 != diff3.end(); itdd3++){
                            if(itdd3->second == minValue)
                            return itdd3->first;
                            diff3.clear();
                        }

                        break;
        }
        case 4:{
            map<int, double>::iterator it4 = mxz4.begin();
                        for (; it4 != mxz4.end(); it4++){
                            diff4.insert(std::pair<int, double> (it4->first, angle_diff(it4->second,angle)));
                        }
                        double minValue = 4;
                        map<int, double>::iterator itd4 = diff4.begin();
                        for (; itd4 != diff4.end(); itd4++){
                            if(itd4->second <= minValue)
                            minValue = itd4->second;
                        }
                        map<int, double>::iterator itdd4 = diff4.begin();
                        for (; itdd4 != diff4.end(); itdd4++){
                            if(itdd4->second == minValue)
                            return itdd4->first;
                            diff4.clear();
                        }

                        break;
        }
        case 5:{
            map<int, double>::iterator it5 = mxz5.begin();
                        for (; it5 != mxz5.end(); it5++){
                            diff5.insert(std::pair<int, double> (it5->first, angle_diff(it5->second,endAngle)));
                        }
                        double minValue = 4;
                        map<int, double>::iterator itd5 = diff5.begin();
                        for (; itd5 != diff5.end(); itd5++){
                            if(itd5->second <= minValue)
                            minValue = itd5->second;
                        }

                        map<int, double>::iterator itdd5 = diff5.begin();
                        for (; itdd5 != diff5.end(); itdd5++){
                            if(itdd5->second == minValue)
                            return itdd5->first;
                            diff5.clear();
                        }

                        break;
        }
        default: {
                    return 100;
                    break;
        }
    }
}
/*
bool ReadParaXml(string m_strXmlPath, vector<BoxSize>& vecNode)
{
    BoxSize *pNode = new BoxSize;

    //start reading
    TiXmlDocument* Document = new TiXmlDocument();

    TiXmlElement* RootElement = Document->RootElement(); //mobility_trace

    TiXmlElement* NextElement = RootElement->FirstChildElement();
    //for(NextElement;NextElement;NextElement = NextElement->NextSiblingElement())
    while(NextElement!=NULL)
    {
        if(NextElement->ValueTStr() == "waypoint")        //distinguish each timestamp
        {
            //NextElement = NextElement->NextSiblingElement();

            TiXmlElement* BoxElement = NextElement->FirstChildElement();
            while(BoxElement->ValueTStr() != "bndbox")      //link to specific line
            {
                BoxElement = BoxElement->NextSiblingElement();
            }
            //link to the first element
            TiXmlElement* xminElemeng = BoxElement->FirstChildElement();
            {
                //parse information
                pNode->xMin = atof(xminElemeng->GetText());
                TiXmlElement* yminElemeng = xminElemeng->NextSiblingElement();
                pNode->yMin = atof(yminElemeng->GetText());
                TiXmlElement* xmaxElemeng = yminElemeng->NextSiblingElement();
                pNode->xMax = atof(xmaxElemeng->GetText());
                TiXmlElement* ymaxElemeng = xmaxElemeng->NextSiblingElement();
                pNode->yMax = atof(ymaxElemeng->GetText());


                vecNode.push_back(*pNode);
            }
        }
        NextElement = NextElement->NextSiblingElement();
    }

    //release memory
    delete pNode;
    delete Document;
    return true;
}
*/
