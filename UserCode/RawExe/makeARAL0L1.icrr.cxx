#include <time.h>
#include <string.h>
#include <cstdio>
#include <fstream>
#include <iostream>
#include <zlib.h>
#include<math.h>
#include <libgen.h>     
#include <cstdlib>
 
using namespace std;
int r;
int RFcounter;
#include "TTree.h"
#include "TFile.h"
#include "TSystem.h"

#define HACK_FOR_ROOT

#include "araIcrrStructures.h"
#include "araAtriStructures.h"
#include "RawIcrrStationEvent.h"  
#include "UsefulIcrrStationEvent.h"

void processEvent(UInt_t stationId,UInt_t blind,TString);
void makeEventTree(char *inputName, char *outDir, UInt_t stationId,UInt_t blind);
IcrrEventBody_t theEventBody;
TFile *theFile1;
TTree *eventTree1;
TFile *theFile1b;
TTree *eventTree1b;
TFile *theFile0b;
TTree *eventTree0b;
TFile *theFile0;
TTree *eventTree0;
int doCalib;
unsigned int startT=0;
unsigned int endT=0;

RawIcrrStationEvent *theEvent=0;
UsefulIcrrStationEvent *theUsefulEvent=0;
char outName0[FILENAME_MAX];
char outName[FILENAME_MAX];
UInt_t realTime;
Int_t runNumber;
Int_t lastRunNumber;
Int_t NPerType[4]={0,0,0,0};
Int_t NPerTypeNotBlinded[4]={0,0,0,0};
int i0;
int i1;

int main(int argc, char **argv) {
  if(argc<4) {
    std::cout << "Usage: " << basename(argv[0]) << " <file list> <out file> <stationId (TestBed==0 Station1==1)> <blind factor> <do Calib>" << std::endl;
    return -1;
  }
   doCalib=atoi(argv[5]);

  makeEventTree(argv[1],argv[2], atoi(argv[3]),atoi(argv[4]));
  return 0;
}
  

void makeEventTree(char *inputName, char *outFile, UInt_t stationId, UInt_t blind) {
printf ("%4d",0);
  RFcounter=0;
  bool debug = false; // set to true to increase the amount of commentary output

//  cout << "user specification - input file list = " << inputName << "\t" << "outFile = " << outFile << endl;
  strncpy(outName,outFile,FILENAME_MAX);
  strncpy(outName0,outFile,FILENAME_MAX);
  if ( debug ) {
     cout << "                   - full outFile = " << outFile << endl;
  }
  theEvent = new RawIcrrStationEvent();
  theUsefulEvent = new UsefulIcrrStationEvent();
  ifstream in(inputName);
  int numFiles = 0;
 string line;
  while ( in.good()) { getline(in,line);  ++numFiles;}
  in.close();
  //    cout << sizeof(IcrrEventBody_t) << endl;
  ifstream SillyFile(inputName);

  int numBytes=0;
  char fileName[FILENAME_MAX];
  int error=0;
  //    int eventNumber=1;

  int total_evt_count = 0;
  int file_count=0;
  int r=0;
  int counter=0;
  while( SillyFile >> fileName){// && counter<3) {
counter++;
    string fileString=fileName;
    char *to = (char*) malloc(6);
    i0=fileString.find("run")+1;
    r=atoi(strncpy(to, fileName+i0+3, 6));
    TString outFileName=Form("run%d",r);
ofstream statusFile;
//statusFile.open("/net/user/hagar/ARA2/current");
statusFile.open("current");
statusFile<<"Run "<<r<<" <br>L0/L1 processing: Looking at file "<<counter<<" out of "<<numFiles<<endl;
statusFile.close();
    printf("\b\b\b\b%4d", counter);
 fflush(stdout);
//    cout << "processing file: " << fileName << endl;
   file_count++;
    static int lastEventNumber=-1;    
    const char *subDir = gSystem->DirName(fileName);
    //    const char *subSubDir = gSystem->DirName(subDir);
    //    const char *eventDir = gSystem->DirName(subSubDir);
    const char *runDir = gSystem->DirName(subDir);
    const char *justRun = gSystem->BaseName(runDir);
    sscanf(justRun,"run_%d",&runNumber);
    //    cout << justRun << "\t" << runNumber <<endl;
    
    gzFile infile = gzopen(fileName, "rb");    
    numBytes=gzread(infile,&theEventBody,sizeof(IcrrEventBody_t));
    int evt_count = 1;
    total_evt_count++;
    while ( numBytes == sizeof(IcrrEventBody_t) ) {
      if ( debug    ) //                  ||
	   //( (evt_count % 1234567) == 1 ) ) 
         {cout << "Event count: " << "for_file = " << evt_count << " - all_toll = " << total_evt_count << endl;
      }
      if(numBytes!=sizeof(IcrrEventBody_t)) {
	if(numBytes)
	  cerr << "Read problem: " <<numBytes << " of " << sizeof(IcrrEventBody_t) << endl;
	error=1;
	break;
      }
      //      cout << "Event: " << theEventBody.hd.eventNumber << endl;
      if(TMath::Abs(Double_t(theEventBody.hd.eventNumber)-lastEventNumber)>1000 && lastEventNumber>=0) {
	std::cerr << "Dodgy calib event\t" << "with diff "<< TMath::Abs(Double_t(theEventBody.hd.eventNumber))<<"-"<<lastEventNumber<<" event="<<theEventBody.hd.eventNumber << "\n";
	lastEventNumber=theEventBody.hd.eventNumber;
      } 
      else {     
	processEvent(stationId,blind,outFileName);

	lastEventNumber=theEventBody.hd.eventNumber;
      }
      numBytes=gzread(infile,&theEventBody,sizeof(IcrrEventBody_t));
      evt_count++;
      total_evt_count++;
    } // end of while loop over events in the input file
    gzclose(infile);
    //	if(error) break;
   } // end of while loop over the ev_ files in the file list
   if (runNumber==0) runNumber=r;
   time_t epch=startT;
  cout<<runNumber<<endl;
cout<<startT<<endl;
cout<<asctime(gmtime(&epch))<<endl;
cout<<NPerType[0]<<endl;
cout<<NPerType[1]<<endl;
cout<<NPerType[2]<<endl;
char buffer [50];
int month=gmtime(&epch)->tm_mon+1;
int day=gmtime(&epch)->tm_mday;
if (day<10 && month<10) sprintf(buffer,"0%d0%d",month,day);
if (day>9 && month<10) sprintf(buffer,"0%d%d",month,day);
if (day<10 && month>9) sprintf(buffer,"%d0%d",month,day);
if (day>9 && month>9) sprintf(buffer,"%d%d",month,day);

 char *stime=asctime(gmtime(&epch));
stime[strlen(stime)-1]='\0';


  ofstream myfile;
//  myfile.open ("/net/user/hagar/ARA2/minbiasLog.txt",ios::app );
  myfile.open ("/srv01/xenon/hagar/ARA/log/minbiaslog.txt",ios::app );
   char sb[500];
  
  sprintf (sb," run %d\t dir %s\t start %d\t %s \t duration %d\t N %d %d %d %d \t not_blinded %d %d %d %d\n ", runNumber, buffer,startT,stime, endT-startT, NPerType[0], NPerType[1], NPerType[2], NPerType[3], NPerTypeNotBlinded[0],NPerTypeNotBlinded[1],NPerTypeNotBlinded[2],NPerTypeNotBlinded[3]);
  printf (" run %d\t dir %s\t start %d\t %s \t duration %d\t N %d %d %d %d \t not_blinded %d %d %d %d\n ", runNumber, buffer,startT,stime, endT-startT, NPerType[0], NPerType[1], NPerType[2], NPerType[3], NPerTypeNotBlinded[0],NPerTypeNotBlinded[1],NPerTypeNotBlinded[2],NPerTypeNotBlinded[3]);
myfile<<sb;
myfile.close();
for (int ii=0; ii<4; ii++) NPerType[ii]=0;
for (int ii=0; ii<4; ii++) NPerTypeNotBlinded[ii]=0;
cout<<(gmtime(&epch))->tm_mday<<" "<<gmtime(&epch)->tm_mon+1<<endl;
  eventTree0->AutoSave();
  if (doCalib)  eventTree1->AutoSave();
  eventTree0b->AutoSave();
  if (doCalib) eventTree1b->AutoSave();
  //    theFile->Close();
 
}

void processEvent(UInt_t stationId, UInt_t blind,TString outFileName) {
  //  cout << "processEvent:\t" << theEventBody.eventNumber << endl;
  static int doneInit=0;
  
  if(!doneInit) {
    //cout << "Creating File: " << outName << endl;

//   TString OutDir="/net/user/hagar/ARA2/blind/out/";	
   TString OutDir="./";	

    if (doCalib)
{
   cout<<"doing calib \n";

//    theFile1 = new TFile("f1.root","RECREATE");
    theFile1 = new TFile(OutDir+"L1."+outFileName+".root","RECREATE");

    eventTree1 = new TTree("eventTree","Tree of ARA Events");
    eventTree1->Branch("run",&runNumber,"run/I");
    eventTree1->Branch("event","RawIcrrStationEvent",&theEvent);
    eventTree1->Branch("calevent","UsefulIcrrStationEvent",&theUsefulEvent);
//    theFile1b = new TFile("f1b.root","RECREATE");
    theFile1b = new TFile(OutDir+"L1."+outFileName+".root.blind","RECREATE");
    eventTree1b = new TTree("eventTree","Tree of ARA Events");
    eventTree1b->Branch("run",&runNumber,"run/I");
    eventTree1b->Branch("event","RawIcrrStationEvent",&theEvent);
    eventTree1b->Branch("calevent","UsefulIcrrStationEvent",&theUsefulEvent);
}

//    TString OutDir="/mnt/Lustre/xenon/hagar/ARA/data/L0/";
 //   TString OutDir="/data/exp/ARA/2012/unbiased/L0/byRun/";
    theFile0 = new TFile(OutDir+"L0."+outFileName+".root","RECREATE");
    eventTree0 = new TTree("eventTree","Tree of ARA Events");
    eventTree0->Branch("run",&runNumber,"run/i");
    eventTree0->Branch("event","RawIcrrStationEvent",&theEvent);


    theFile0b = new TFile(OutDir+"L0."+outFileName+".root.blind","RECREATE");
    eventTree0b = new TTree("eventTree","Tree of ARA Events");
    eventTree0b->Branch("run",&runNumber,"run/I");
    eventTree0b->Branch("event","RawIcrrStationEvent",&theEvent);
    
    doneInit=1;
  }
//  cout << "Here: "  << theEvent.eventNumber << endl;
  if(theEvent) delete theEvent;
  if(doCalib==1 && theUsefulEvent) delete theUsefulEvent;
  theEvent = new RawIcrrStationEvent(&theEventBody, stationId);
  if (doCalib)   theUsefulEvent = new UsefulIcrrStationEvent( theEvent , AraCalType::kLatestCalib );
 //  TGraph *g=theUsefulEvent->getGraphFromRFChan(0);
 // double x,y;
 //  g->GetPoint(5,x,y); cout<<"la la "<<x<<"\t"<<y<<endl;
  Int_t triggerType  = (Int_t) theEvent->trig.trigType;
  Double_t RbClock = (Double_t) theEvent->trig.getRubidiumTriggerTimeInSec();
  int EventType=0;
  if (startT==0) startT=theEvent->head.unixTime;
  endT=theEvent->head.unixTime;

  if (triggerType==68) EventType=2;  // Forced
  else if (fabs(RbClock-20.7e-6)<1e-7) EventType=3; // Pulser
  else if (triggerType==1) EventType=1;  // RF
  NPerType[EventType]++;
  if (EventType==1) RFcounter++;
  int Id=theEvent->head.eventNumber;
  //  if ((Id/blind)*blind == Id) eventTree->Fill();  
 //cout<<"RFcounter="<<RFcounter<<"   blind="<<blind<<"   eventtype="<<EventType<<"   trig="<<triggerType<<endl;
 
  if (EventType==1 && (RFcounter/blind)*blind == RFcounter) { if (doCalib) eventTree1->Fill();  eventTree0->Fill(); NPerTypeNotBlinded[EventType]++;} 
  //if (EventType==1 && (RFcounter/blind)*blind == RFcounter) { cout<<"  not blinding"<<endl;}
  else if (EventType!=1 ) {if (doCalib) eventTree1->Fill();  eventTree0->Fill();   NPerTypeNotBlinded[EventType]++;} 
  else  {  if (doCalib) eventTree1b->Fill();  eventTree0b->Fill();}
  //  eventTree->Fill();
r=runNumber;  
  lastRunNumber=runNumber;
  //  delete theEvent;
}
