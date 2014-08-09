//for ara01, use VPol antennas ONLY in reco on VPOL input file
//change ihit<8 in LHit definition
//testbed: stationId<2
//check to see if we are saturating ADC values
//fStationInfo[station].fAntInfo[ich].polType==AraAntPol::kHorizontal
//TMarker::DisplayMarkerTypes()
//default colors at: https://www.phenix.bnl.gov/~enterria/root_colors.jpg
//less /net/local/ara/repositories/ara/AraRoot/branches/3.8/AraEvent/AraAntennaInfo.cxxs
///net/local/ara/repositories/ara/AraRoot/trunk/AraEvent has most of .cxx files
//10May13: MEMORY CREEPING UP - SOMETHING IS NOT BEING DELETED!

gSystem->Reset();
#include "vtxutil.h"
#include "vtxgrid.h"
const int NChannl=16;
#include "vtxlibvars.h"
#include "vtxlib.h"
#include "loadlib.h"
using namespace std;
const Float_t ft2m=0.3048;

bool goodEvent=false;
bool addDelay = false;
Int_t NEvWrite=0;
const Int_t MDT_VTHRESH_VMAX=20;
const Int_t MEVWRITE=20;
const Int_t MAXEVDRAW=20;
const Int_t MCH=20;
const Int_t MCHINICE=16;
Int_t MCH0;
Int_t index[MCH];
Int_t nhits, trigCode, nhitsV, nhitsH, nhitsS;
Int_t niter=0;
Bool_t LEvAna;
Bool_t isCalPulser;
Bool_t LL0=true;
Bool_t LL1=true;
Float_t TempTime[MCH];
Float_t t0ch[MCH];
Float_t vvtx[3];
ULong64_t unixTime=0;
ULong64_t unixTime0;
ULong64_t unixTimeUs=0;
ULong64_t unixTimeUs0;
UInt_t eventNumber;
Int_t chNumber[20];
string chanName[20];
Float_t threshold[20];

Float_t Tx1[3]={122.067*ft2m,-61.0946*ft2m,-633.199*ft2m};
Float_t Tx2[3]={56.3175*ft2m,114.299*ft2m,-551.514*ft2m};

const Float_t m2km=0.001;
const Float_t RxTOTMAX=100;
const Float_t rice=1.78;
const Double_t sLightAir = 0.29970551819; //velocity of light in meters/ns
const Double_t nIce=rice; //1.78; for calibration pulser

Float_t myAntLocX[20];
Float_t myAntLocY[20];
Float_t myAntLocZ[20];
Int_t iuseAnt[MCH]={0};
Int_t antForFit[MCH]={0};
Float_t sX, sY, sZ, sTime;
Float_t rawTime[MCH];

Bool_t LuseAnt[MCH]={false};
Bool_t LHit;
Int_t NUseAnt[MCH]={0};
Int_t iVMaxBin[MCH]={0};
Int_t iNSigBin[MCH]={0};
Float_t VRMS[MCH], Rxtresid[MCH], RxTOT[MCH], WfPCh[MCH], chisqvtx;
Double_t gains[MCH]={1.};
Double_t FitPoint[4], FitPerror[4]; //, FitPoint[4], FitPerror[4];
Double_t chi_square;
Float_t RxHitCode2, RxHitCode10;
Double_t tmp_double = 0.0; //dummy parameters
Int_t tmp_int = 0; //dummy parameters
const Float_t MSIGMACUT0=4.5; //4.5=good for ara2/ara3
//const Float_t MSIGMACUT0=5; //changed to 6 as it is in testbed
Float_t MSIGMACUT[MCH];
const Float_t RScaleFirstHit=4;
const int MBINRMS=64;
Float_t tresidMax=0;
const Float_t fInterp=0.5; //interpolation factor
TH3F *hhits[MCH];
struct RxChs{
  char cID1[4], cID2[4], cLABID1[2], cLABID2[2], cHV[2], cRxType[9], cFullName[18], cPol, cLocation[3], cdum10[10], cdum11[11];
  int LPF, HPF, iLogCh, iRxno, ichno, indx;
  float NF, x, y, z, xt, yt, zt, cableDelayNs, HitTime, RawHitTime, RawHitTime0, TimeDelayNsCorr, TimeDelayNsCorrt, hitweight, dtFitResid, dtResidSum, A, dtSimTx, deldTij, dtNsCorr;
};

RxChs Rx[MCH];
//Rx[0].dtNsCorr=0; Rx[1].dtNsCorr[1]=-3.5; Rx[2].dtNsCorr[2]=105; Rx[3].dtNsCorr=-2; Rx[4].dtNsCorr=2; Rx[5].dtNsCorr=3; Rx[6].dtNsCorr=110; Rx[7].dtNsCorr=-5; Rx[8].dtNsCorr=0; Rx[9].dtNsCorr=0; Rx[10].dtNsCorr=0; Rx[11].dtNsCorr=0; Rx[12].dtNsCorr=0; Rx[13].dtNsCorr=0; Rx[14].dtNsCorr=0; Rx[15].dtNsCorr=0; Rx[16].dtNsCorr=0; Rx[17].dtNsCorr=0; Rx[18].dtNsCorr=0; Rx[19].dtNsCorr=0;
string tmp_string;
//TGraph *gVrms[MCH];
//TGraph *grch[MCH];
//TGraph *grfftch[MCH];
//TGraph *grFFTch[MCH];
char ofname[128];

Float_t T0;
double channelDelay[16];	// Additional time delays from calibration
Float_t deltaTime[MCH];		// delta fit time - channel time
Int_t convergence[2] = {0,0};	// Convergence flags for SIMPLEX and MIGRAD (0 - conv, 4 - failed)

int RootScript(char *in, char *out, string miscOut){
  myL2vrtx13(-1, 2, 0, in, out, miscOut);
  return 0;
}

void myL2vrtx13(Int_t MAXEVT, Int_t istation, Int_t iPol, char *input, char *output, string misc){
  cout <<"misc"<< misc;
  Bool_t LPol;
    
  MCH0=20;
  if(istation==0)MCH0=16;
  //  if(istation==0)ifstream geomFile("/home/dbesson/src/TB_geom_Rx.txt");
  //  if(istation==1)
  //ifstream geomFile("ara01_geom_Rx.txt");
  if(istation==2)
    ifstream geomFile("ara02_geom_Rx.txt");
  if(istation==3)
    ifstream geomFile("ara03_geom_Rx.txt");
  
  if(istation>=2)
    if(!geomFile)
      cout<<"MISSING GEOMETRY FILE!"<<endl;

  
  Int_t ievtdraw=0;
  char cblank[128], cpngname[128], chName[128];
  Float_t xmin=9999;
  Float_t xmax=-9999;
  Float_t ymin=9999;
  Float_t ymax=-9999;
  Float_t zmin=9999;
  Float_t zmax=-9999;

  Float_t xminTx=9999;
  Float_t xmaxTx=-9999;
  Float_t yminTx=9999;
  Float_t ymaxTx=-9999;
  Float_t zminTx=9999;
  Float_t zmaxTx=-9999;

  Float_t WfPSum=0;

  if(istation>0){
    for(Int_t ich=0; ich<MCH0; ich++){
      geomFile>>Rx[ich].cFullName>>Rx[ich].xt>>Rx[ich].yt>>Rx[ich].zt;
      Rx[ich].xt*=ft2m;
      Rx[ich].yt*=ft2m;
      Rx[ich].zt*=ft2m;
      geomFile.getline(cblank,sizeof(cblank));
      //    printf("Rx[%d].x=%f / Rx[%d].y=%f / Rx[%d].z=%f\n",ich,Rx[ich].x,ich,Rx[ich].y,ich,Rx[ich].z);
    }
  }

  
  xminTx=xmin;
  xmaxTx=xmax;
  yminTx=ymin;
  ymaxTx=ymax;
  zminTx=zmin;
  zmaxTx=zmax;

  if(xmin>Tx1[0])xminTx=Tx1[0];
  if(xmax<Tx1[0])xmaxTx=Tx1[0];
  if(ymin>Tx1[1])yminTx=Tx1[1];
  if(ymax<Tx1[1])ymaxTx=Tx1[1];
  if(zmin>Tx1[2])zminTx=Tx1[2];
  if(zmax<Tx1[2])zmaxTx=Tx1[2];

  if(xmin>Tx2[0])xminTx=Tx2[0];
  if(xmax<Tx2[0])xmaxTx=Tx2[0];
  if(ymin>Tx2[1])yminTx=Tx2[1];
  if(ymax<Tx2[1])ymaxTx=Tx2[1];
  if(zmin>Tx2[2])zminTx=Tx2[2];
  if(zmax<Tx2[2])zmaxTx=Tx2[2];

  printf("xmin=%f xmax=%f ymin=%f ymax=%f zmin=%f zmax=%f\n",xmin,xmax,ymin,ymax,zmin,zmax);
  
  UInt_t run;
  char cwfofname[256];
  char crootfileout[256];
  char cinteger[2];
  //  itoa(irandom,cinteger,2);
  cout<<"Using rice="<<rice<<endl;
  datestamp=datechar();
  
  sprintf(cwfofname,"/%sL2vrtx_CPwf_%s_ara%d_pol%d.txt",misc,datestamp,istation,iPol);
  ofstream CPwfof(cwfofname);
  sprintf(ofname,"/%sL2vrtx_%s_ara%d.txt",misc,datestamp,istation);
  ofstream of(ofname);
  sprintf(ofname,"/%sL2vrtx_residuals_%s_ara%d.txt",misc,datestamp,istation);
  ofstream of1(ofname);
  //  sprintf(crootfileout,"/home/cbora/L2vrtx13/Output/L2vrtx_%s_ara%d_pol%d.root",datestamp,istation,iPol);
  //  sprintf(crootfileout, "A2D6HPol_Dave_Threshold_station_0%d_Pol_%d.root", istation, iPol);
  char *outp;
  //  sprintf(outp,"/%sHitTimesA2D6HPol_DavesThreshold_Pol%d.txt", misc,iPol);
    misc.append("hitTimes_Dave_threshold.txt");
  outp =misc.c_str();
  ofstream outTimes(outp);
  //  string rootfileoutstring=string("~/ctem/L2vrtx_")+datestamp+string("_")+cinteger+string("_")+ofcomment+string(".root");  cout<<rootfileoutstring<<endl;
  TFile f(output,"recreate");
  TH1F *hVrms[MCH];
  TH1F *hSNRVMax[MCH];
  TH1F *hVrms0[MCH];
  TH1F *hFirstHitTime=new TH1F("hFirstHitTime","hFirstHitTime",500,-250,250);
  TH1F *hTus=new TH1F("hTus","hTus",500,0,1000000);
  Float_t Tus;
  TH2F *hdeldTij_CP1=new TH2F("hdelTij_CP1","hdelTij_CP1",500,-100,100,MCH,-0.5,MCH-0.5);
  TH2F *hdeldTij_CP2=new TH2F("hdelTij_CP2","hdelTij_CP2",500,-100,100,MCH,-0.5,MCH-0.5);
  
  TH2F *htRx=new TH2F("htRx","htRx",250,0,250,MCH,-0.5,MCH-0.5);
  TH2F *htRxCP=new TH2F("htRxCP","htRxCP",250,0,250,MCH,-0.5,MCH-0.5);
  TH2F *hVrmsChCP=new TH2F("hVrmsChCP","hVrmsChCP",500,0,500,MCH,-0.5,MCH-0.5);
  TH2F *hVrmsChnoCP=new TH2F("hVrmsChnoCP","hVrmsChnoCP",500,0,500,MCH,-0.5,MCH-0.5);
  TH2F *ht0Ch=new TH2F("ht0Ch","ht0Ch",500,-250,250,MCH,-0.5,MCH-0.5);
  TNtuple *N=new TNtuple("N","N","WfP:Tus");
  Float_t phi, the, phivtx, thevtx;
  TTree *tree=new TTree("tree","L2vrtx");
  tree->Branch("FitPoint",FitPoint,"FitPoint[4]/D");
  tree->Branch("FitPerror",FitPerror,"FitPerror[4]/D");
  tree->Branch("threshold", &threshold, "threshold[20]/F");
  tree->Branch("vvtx",vvtx,"vvtx[3]/F");
  tree->Branch("niter",&niter,"niter/I");
  tree->Branch("nhits",&nhits,"nhits/I");
  tree->Branch("nhitsS",&nhitsS,"nhitsS/I");
  tree->Branch("nhitsV",&nhitsV,"nhitsV/I");
  tree->Branch("nhitsH",&nhitsH,"nhitsH/I");
  tree->Branch("trigCode",&trigCode,"trigCode/I");
  tree->Branch("RxHitCode2",&RxHitCode2,"RxHitCode2/F");
  tree->Branch("RxHitCode10",&RxHitCode10,"RxHitCode10/F");
  tree->Branch("Tus",&Tus,"Tus");
  tree->Branch("phi",&phi,"phi");
  tree->Branch("the",&the,"the");
  tree->Branch("run",&run,"run/I");
  tree->Branch("eventNumber", &eventNumber, "eventNumber/I");
  tree->Branch("phivtx",&phivtx,"phivtx");
  tree->Branch("thevtx",&thevtx,"thevtx");
  tree->Branch("WfPSum",&WfPSum,"WfPSum");
  tree->Branch("T0",&T0,"T0");
  tree->Branch("isCalPulser",&isCalPulser,"isCalPulser/I");
  tree->Branch("chisqvtx",&chisqvtx,"chisqvtx");
  tree->Branch("Rxtresid",&Rxtresid,"Rxtresid[20]/F");
  tree->Branch("RxTOT",&RxTOT,"RxTOT[20]/F");
  tree->Branch("WfPCh",&WfPCh,"WfPCh[20]/F");
  tree->Branch("dvtxChnl",&dvtxChnl,"dvtxChnl[20][3]");
  tree->Branch("deltaTime",&deltaTime,"deltaTime[20]/F"); // Delta between reconstructed pulser time and time when this chanel detected the pulse
  tree->Branch("convergence", &convergence, "convergence[2]/I");
  tree->Branch("antForFit", &antForFit, "antForFit[20]/I");
  tree->Branch("sTime", &sTime, "sTime/F");
  tree->Branch("sX", &sX, "sX/F");
  tree->Branch("sY", &sY, "sY/F");
  tree->Branch("sZ", &sZ, "sZ/F");
  tree->Branch("rawTime", &rawTime, "rawTime[20]/F");
  tree->Branch("myAntLocX", &myAntLocX, "myAntLocX[20]/F");
  tree->Branch("myAntLocY", &myAntLocY, "myAntLocY[20]/F");
  tree->Branch("myAntLocZ", &myAntLocZ, "myAntLocZ[20]/F");
  tree->Branch("chNumber", &chNumber, "chNumber[20]/I");
  tree->Branch("chanName", (void*)chanName, "chanName[20]/C");

  Int_t NEvTot=0;
  Int_t ierflg = 0;
  Float_t Vtx[4];
  Double_t arglist[2]={200,1}; //,10,10};	// UP = 200, 1 - is not used
  Double_t argsSimplexMigrad[2]={1000, 1};	// MaxCalls = 1000, tolerance = 1
  Double_t step[4]={20,4,4,4};
  const Float_t MINUIT_TMIN=-512*6;
  const Float_t MINUIT_TMAX=512;
  const Float_t MINUIT_XMIN=-512;
  const Float_t MINUIT_XMAX=512;
  const Float_t MINUIT_YMIN=-512;
  const Float_t MINUIT_YMAX=512;
  const Float_t MINUIT_ZMIN=-512;
  const Float_t MINUIT_ZMAX=512;
  
  TMinuit* eventrecoMinuit = new TMinuit(4);
  Int_t IPrintLevel=1;  
  eventrecoMinuit->SetPrintLevel(IPrintLevel); //should be first command after TMinuit* definition
  eventrecoMinuit->SetMaxIterations(1000);
  eventrecoMinuit->SetFCN(calc_chi_square); //chi square routine
  eventrecoMinuit->mnexcm("SET ERR", arglist, 1, ierflg);	// Parameter UP for both SIMPLEX and MIGRAD is set up here to 200
  
  loadlib();

  //  ifstream InFileList;
  
  RawIcrrStationEvent *rawIcrrEvPtr;
  UsefulIcrrStationEvent *realIcrrEvPtr;
  RawIcrrStationEvent *eventPtrIcrrZ=0; //should work on processed files as of 31jan11
  
  RawAtriStationEvent *rawAtriEvPtr;
  UsefulAtriStationEvent *realAtriEvPtr;
  RawAtriStationEvent *eventPtrAtriZ=0; //should work on processed files as of 31jan11

  //see /net/local/ara/repositories/ara/AraRoot/branches/3.8/dbScripts/plotAntennaPositions.C
  AraStationInfo *stationInfo=new AraStationInfo(istation);
  //void AraGeomTool::readChannelMapDb(Int_t stationId) -  Found in AraGeomTool.cxx (AraRoot/branches/3.2/AraEvent)
  //  AraGeomTool *araGeom=AraGeomTool::Instance();
  //  AraAntennaInfo antennaInfo; //=AraAntennaInfo::Instance();
  //  cout<<"THE CORRELATOR!"<<endl;
  //  AraEventCorrelator *theCorrelator= AraEventCorrelator::Instance();
  if(istation==0)AraEventCorrelator *theCorrelator= AraEventCorrelator::Instance();
  else AraEventCorrelator *theCorrelator= AraEventCorrelator::Instance(8,istation);
  //  AraEventCorrelator::fillAntennaPositions();
  //  cout<<"END THE CORRELATOR!"<<endl;
  //  AraEventCorrelator::fDebugMode(1);
  //  AraCorrelatorType::EAraCorrelatorType corType;
  //  corType=AraCorrelatorType::kPlaneWave;


  Bool_t LHoleD3Ara02[MCH]={false,false,true,false,false,false,true,false,
			    false,false,true,false,false,false,true,false,false,false,false,false};
  Float_t antLocX, antLocY, antLocZ;
  Int_t chanNum0, daqChanNum;
  Float_t Station_cog_z=0;

  if(MCH0==0)abort();
  for(Int_t ich=0; ich<MCH0; ich++){
    if(istation>0)stationInfo->getAntennaInfo(ich)->printAntennaInfoAtri();    
    else stationInfo->getAntennaInfo(ich)->printAntennaInfoIcrr();    
    
    Double_t *antLoc=stationInfo->getAntennaInfo(ich)->getLocationXYZ();
    Rx[ich].x=antLoc[0];
    Rx[ich].y=antLoc[1];
    Rx[ich].z=antLoc[2];
    myAntLocX[ich]= antLoc[0];
    myAntLocY[ich]= antLoc[1];
    myAntLocZ[ich]= antLoc[2];

    if(ich<MCHINICE)Station_cog_z+=antLoc[2];

    if(Rx[ich].x<xmin)xmin=Rx[ich].x;
    if(Rx[ich].y<ymin)ymin=Rx[ich].y;
    if(Rx[ich].z<zmin)zmin=Rx[ich].z;
    if(Rx[ich].x>xmax)xmax=Rx[ich].x;
    if(Rx[ich].y>ymax)ymax=Rx[ich].y;
    if(Rx[ich].z>zmax)zmax=Rx[ich].z;

    chanNum0=stationInfo->getAntennaInfo(ich)->chanNum;
    Rx[ich].TimeDelayNsCorr=stationInfo->getCableDelay(ich);
    daqChanNum=stationInfo->getAntennaInfo(ich)->daqChanNum;
    printf("RxX[%d]=%f, RxY[%d]=%f, RxZ[%d]=%f / chanNum0=%d / daqChanNum=%d / Delay=%f / poltype=%f\n",ich,Rx[ich].x,ich,Rx[ich].y,ich,Rx[ich].z,chanNum0,daqChanNum,Rx[ich].TimeDelayNsCorr,stationInfo->getAntennaInfo(ich)->polType);
    printf("AC geo file: %s %f %f %f\n",Rx[ich].cFullName,Rx[ich].xt,Rx[ich].yt,Rx[ich].zt);
  }

  Station_cog_z/=MCHINICE;
  cout<<"Station c.o.g. in z="<<Station_cog_z<<endl;

  /*
  if(istation==0 && false)InFileList.open("/home/dbesson/lists/TB0513.list"); 
  if(istation==0){
    if(iPol==0)InFileList.open("/home/dbesson/lists/TB_iclrooftop_13_HPol.list");
    else InFileList.open("/home/dbesson/lists/TB_iclrooftop_13_VPol.list");
  }
  if(istation==1){
    if(iPol==0)InFileList.open("/home/dbesson/lists/ara01_iclrooftop_13_HPol.list"); 
    else InFileList.open("/home/dbesson/lists/ara01_iclrooftop_13_VPol.list"); 
  }
  if(istation==2)
    {
      cout <<" station 2 **************" << endl;
      //      InFileList.open("/home/dbesson/lists/L0ARA02_13.list");
      //                  InFileList.open("/home/cbora/L2vrtx13/ara02A2D6_example.txt");
      //            InFileList.open("/home/cbora/L2vrtx13/Ara02A2D6_HPol_1sample.txt");
      //            InFileList.open("/home/cbora/L2vrtx13/Ara02A2D5_VPol_sample.txt");
      //            InFileList.open("/home/cbora/L2vrtx13/bigListAra02D5_VPol.txt");
      //                       InFileList.open("/home/cbora/L2vrtx13/bigListAra02D5_HPol.txt");
      // InFileList.open("/home/cbora/L2vrtx13/laterDates.txt");
      //            InFileList.open("/home/cbora/L2vrtx13/bigListAra02D6_VPol.txt");
      //      InFileList.open("/home/cbora/L2vrtx13/bigListAra02D5_HPol.txt");
	             InFileList.open("/home/cbora/L2vrtx13/bigListA2D6_HPol.txt");
      //            InFileList.open("/home/cbora/L2vrtx13/bigListAra02D6_VPol.txt");
      //      InFileList.open("/home/cbora/L2vrtx13/Ara02A2D5_HPol_sample.txt");
    }
  if(istation==3 && true)InFileList.open("/home/dbesson/lists/L0ARA03_13.list");
  if(istation==2 && false){
    cout <<"here**********"<< endl;
    if(iPol==0)InFileList.open("/home/dbesson/lists/ara02_iclrooftop_13_HPol.list");
    else InFileList.open("/home/dbesson/lists/ara02_iclrooftop_13_VPol.list");
  }
  if(istation==3)InFileList.open("/home/dbesson/lists/ara03_iclrooftop_13.list");
  cout<<"Reading  infilelist"<<endl;
  */  //  cout <<InFileList <<endl;

  string path;
  TFile *fp;
  stringstream str;

  TCanvas *c1=new TCanvas("c1","c1");
  TCanvas *cIFG=new TCanvas("cIFG","cIFG");
  cIFG->Divide(2,1);
  
  const Int_t MFILESREADINMAX=20;
  Int_t NFilesReadIn=0;
  for(Int_t ich=0; ich<MCH0; ich++){
    char ctitle[128];
    sprintf(ctitle,"hVrms%d",ich);
    hVrms[ich]=new TH1F(ctitle,ctitle,1000,0,200); //1000 bins from 0 to 200 mV
    sprintf(ctitle,"hVrms0_%d",ich);
    hVrms0[ich]=new TH1F(ctitle,ctitle,512,0,512);
    sprintf(ctitle,"hhits%d",ich);
    hhits[ich]=new TH3F(ctitle,ctitle,100,xmin-1,xmax+1,100,ymin-1,ymax+1,100,zmin-1,zmax+1);
    sprintf(ctitle,"hSNRVMax%d",ich);
    hSNRVMax[ich]=new TH1F(ctitle,ctitle,100,0,40); //1000 bins from 0 to 200 mV
    MSIGMACUT[ich]=MSIGMACUT0;
    //    if(istation==1)MSIGMACUT[ich]=6;
    hhits[ich]->Fill(Rx[ich].x, Rx[ich].y, Rx[ich].z);
    if(ich>=8){
      hhits[ich]->SetMarkerStyle(29);
      hhits[ich]->SetMarkerSize(5);
    }
  }
  
  TH3F *h3dTx1=new TH3F("h3dTx1","h3dTx1",100,xminTx-1,xmaxTx+1,100,yminTx-1,ymaxTx+1,100,zminTx-1,zmaxTx+1);
  TH3F *h3d0=new TH3F("h3d0","h3d0",100,xmin-1,xmax+1,100,ymin-1,ymax+1,100,zmin-1,zmax+1);
  TH3F *h3dTx2=new TH3F("h3dTx2","h3dTx2",100,xminTx-1,xmaxTx+1,100,yminTx-1,ymaxTx+1,100,zminTx-1,zmaxTx+1);
  h3dTx1->Fill(Tx1[0],Tx1[1],Tx1[2]);
  h3dTx2->Fill(Tx2[0],Tx2[1],Tx2[2]);

  h3dTx1->SetXTitle("X-coordinate");
  h3dTx1->GetXaxis()->SetTitleOffset(1.3);
  h3dTx1->SetYTitle("Y-coordinate");
  h3dTx1->GetYaxis()->SetTitleOffset(1.3);
  h3dTx1->SetZTitle("Z-coordinate");
  h3dTx1->GetZaxis()->SetTitleOffset(1.3);
  h3dTx1->SetMarkerStyle(kFullSquare);
  h3dTx1->SetMarkerColor(30);
  h3dTx1->SetMarkerSize(3);
  h3dTx2->SetMarkerStyle(kFullSquare);
  h3dTx2->SetMarkerSize(3);
  h3dTx2->SetMarkerColor(31);

  h3d0->SetMarkerStyle(kFullSquare);
  h3d0->SetMarkerColor(kBlack);
  h3d0->SetMarkerSize(4);
  h3d0->SetXTitle("X-coordinate");
  h3d0->GetXaxis()->SetTitleOffset(1.3);
  h3d0->SetYTitle("Y-coordinate");
  h3d0->GetYaxis()->SetTitleOffset(1.3);
  h3d0->SetZTitle("Z-coordinate");
  h3d0->GetZaxis()->SetTitleOffset(1.3);
  /****
  while(!InFileList.eof() && (MAXEVT<0||NEvTot<MAXEVT) && NFilesReadIn<MFILESREADINMAX){
    InFileList>>path;     
    if(path.find("L0")!=string::npos){
      cout<<"Input File is L0!"<<endl;
      LL1=false;
    }
    else if(path.find("L1")!=string::npos){
      cout<<"Input File is L1!"<<endl;
      LL0=false;
    }
    else cout<<"Cannot decode filename!"<<endl;

    if(InFileList.eof()||!InFileList.good())
      break;
    str=path;
	
    filename = str.str();
    **/
      //    if(fp)delete fp;
    fp=new TFile(input);  
    
    //    fp->cd();
    int isIcrrEvent=0;
    int isAtriEvent=0;

    TTree *eventTreeData=(TTree*)gDirectory->Get("eventTree");
    if(istation==0)eventTreeData->SetBranchAddress("event",&eventPtrIcrrZ);
    else eventTreeData->SetBranchAddress("event",&eventPtrAtriZ);
    eventTreeData->SetBranchAddress("run",&run);
    eventTreeData->GetEntry(0);
    cout<<"run="<<run<<endl;
    Int_t iSta;
    if(istation==0)iSta=eventPtrIcrrZ->stationId;
    else iSta=eventPtrAtriZ->stationId;
    
    cout<<"iSta=="<<iSta<<endl;
    if(iSta!=istation){
      cout<<"CAUTION! ISTA DOES NOT EQUAL ISTATION!"<<endl;
      //abort();
    }
    if(iSta==0){
      isIcrrEvent=1;
      isAtriEvent=0;
    }
    else{
      isIcrrEvent=0;
      isAtriEvent=1; 
    }
    
    eventTreeData->ResetBranchAddresses();
    
    //Now set the appropriate branch addresses
    //The Icrr case
    if(isIcrrEvent){
      eventTreeData->SetBranchAddress("event", &rawIcrrEvPtr);
      std::cerr << "Set Branch address to Icrr\n";
    }
    //The Atri case
    else{
      cout<<"Getting event!"<<endl;
      eventTreeData->SetBranchAddress("event", &rawAtriEvPtr);
      cout<<"Got event!"<<endl;
      std::cerr << "Set Branch address to Atri\n";
    }
    
    for(Int_t ievt=0; ievt<eventTreeData->GetEntries() && (MAXEVT<0||NEvTot<MAXEVT); ievt++){
      niter=0;
      eventTreeData->GetEntry(ievt); 
      if(ievt==0)
	eventTreeData->Show(ievt);
      if(isIcrrEvent)      	     //Here we create a useful event Either an Icrr or Atri event
	realIcrrEvPtr = new UsefulIcrrStationEvent(rawIcrrEvPtr, AraCalType::kLatestCalib);
      else if(isAtriEvent){
	//	cout<<"Getting ATRI DATA!!!!!"<<endl;
	realAtriEvPtr = new UsefulAtriStationEvent(rawAtriEvPtr, AraCalType::kLatestCalib);
      }
      
      if(isIcrrEvent){
	unixTime = realIcrrEvPtr->head.unixTime;
	unixTimeUs = realIcrrEvPtr->head.unixTimeUs;
	eventNumber = realIcrrEvPtr->head.eventNumber;
	int shortWaveform=0;
	for(int chip=0; chip<3;chip++){
	  shortWaveform=realIcrrEvPtr->shortWaveform(chip);
	  if(shortWaveform){
	    fprintf(stderr, "shortWaveform on labChip %i\n", chip);
	  }
	}	 
      }
      else if(isAtriEvent){
	unixTime = realAtriEvPtr->unixTime;
	unixTimeUs = realAtriEvPtr->unixTimeUs;
	eventNumber = realAtriEvPtr->eventNumber;
	printf ("Reading Atri NEvTot=%d / ppsNumber=%d trig blocks= %d %d %d %d \n",NEvTot,realAtriEvPtr->ppsNumber,realAtriEvPtr->triggerBlock[0],realAtriEvPtr->triggerBlock[1],realAtriEvPtr->triggerBlock[2],realAtriEvPtr->triggerBlock[3]);
      }

      Tus=unixTimeUs;
      hTus->Fill(Tus);
      isCalPulser=unixTimeUs<2000;
      if(istation==3)isCalPulser=unixTimeUs>290000&&unixTimeUs<320000;
      if(isCalPulser==1){
	NEvWrite++;
	cout<<"CAL PULSER EVENT!!!!"<<endl;
      }
      memset(RxTOT,0,sizeof(RxTOT));
      memset(WfPCh,0,sizeof(WfPCh));
      nhits=0;
      T0=9999;
      nhitsS=0;
      nhitsV=0;
      nhitsH=0;
      RxHitCode2=0;
      RxHitCode10=0;
      WfPSum=0;

      Float_t dtdt=0;      
      Float_t T0event=99999;
      Int_t iChFirstHit=-1;
      Int_t iantMax=-1;

      if(isCalPulser==1 && NEvWrite<MEVWRITE)CPwfof<<"#"<<eventNumber<<" "<<unixTime<<" "<<unixTimeUs<<endl;      
      //      LEvAna=NEvTot<3210 || NEvTot>3250;
      for(int i=0; i<20; i++) chNumber[i]=0;
      // outTimes <<"*********************"<<"\n";
      //      outTimes <<"\n";
      for(Int_t ich=0; ich<MCH0; ich++){ //use  maximum bin for hit time
	//       	if((stationInfo->getAntennaInfo(ich)->chanNum == 0)||(stationInfo->getAntennaInfo(ich)->chanNum == 1)||(stationInfo->getAntennaInfo(ich)->chanNum == 2)||(stationInfo->getAntennaInfo(ich)->chanNum == 3)) break;
	//if((stationInfo->getAntennaInfo(ich)->chanNum == 4)||(stationInfo->getAntennaInfo(ich)->chanNum == 5)||(stationInfo->getAntennaInfo(ich)->chanNum == 6)||(stationInfo->getAntennaInfo(ich)->chanNum == 7)) break;
	if(ich<8)hhits[ich]->SetMarkerStyle(kFullCircle);
	Rx[ich].indx=-1;
	Rx[ich].A=-1;
	iuseAnt[ich]=0;
	antForFit[ich]= iuseAnt[ich];
	Rx[ich].RawHitTime=-9999;
	Rx[ich].HitTime=-9999;
	TempTime[ich]=-9999;
	LuseAnt[ich]=false; // leave false if known bad channel
	
	if(isIcrrEvent){
	  TGraph *gVt=realIcrrEvPtr->getGraphFromRFChan(ich);
	}
	else if(isAtriEvent){
	  TGraph *gVt=realAtriEvPtr->getGraphFromRFChan(ich);
	}
	TGraph *gVt0=FFTtools::getInterpolatedGraph(gVt,fInterp);
	delete gVt;
	//	TGraph *gVt0=FFTtools::getInterpolatedGraph(grch[ich],fInterp); //no gain
	Double_t *xt=gVt0->GetX();
	///	outTimes<<"channel number: "<< ich <<" " << gVt0->GetX() <<"\n";
	Double_t *Vt=gVt0->GetY();
	ht0Ch->Fill(xt[0],ich);
	Int_t nVt0=gVt0->GetN();
	
	TH1F *hVt=new TH1F("hVt","hVt",nVt0,0,nVt0);
	TH1F *habsVt=new TH1F("habsVt","habsVt",nVt0,0,nVt0);
	if(isCalPulser==0)hVrms[ich]->Fill(gVt0->GetRMS(2)); //Fill(hVt->GetRMS());
	for(Int_t ibins=0; ibins<nVt0; ibins++){
	  WfPSum+=Vt[ibins]*Vt[ibins];
	  WfPCh[ich]+=Vt[ibins]*Vt[ibins];
	  hVt->SetBinContent(ibins,Vt[ibins]);
	  habsVt->SetBinContent(ibins,fabs(Vt[ibins]));
	  if(isCalPulser==0)
	    hVrms0[ich]->Fill(ibins,Vt[ibins]);
	}
	
	iVMaxBin[ich]=hVt->GetMaximumBin();
	Float_t VMaxBin=hVt->GetMaximum();
	VRMS[ich]=hVrms[ich]->GetMean();
	if(isCalPulser==1)hVrmsChCP->Fill(VRMS[ich],ich);
	else hVrmsChnoCP->Fill(VRMS[ich],ich);
	if(VRMS[ich]==0){
	  cout<<"VRMS["<<ich<<"]="<<VRMS[ich]<<endl;
	  VRMS[ich]=9999;
	}
	//	if(trigCode==68)cout<<" hVrms0max="<<hVrms0[ich]->GetRMS()<<"+/-"<<hVrms0[ich]->GetRMSError()<<" hVrms0Max="<<hVrms0[ich]->GetMaximum()<<" hVrms="<<hVrms[ich]->GetMean()<<"+/-"<<hVrms[ich]->GetMeanError()<<" hVt="<<hVt->GetRMS()<<"+/-"<<hVt->GetRMSError()<<" gVt->GetRMS(2)="<<gVt->GetRMS(2)<<" gVt0->GetRMS(2)="<<gVt0->GetRMS(2)<<endl;
	
	iNSigBin[ich]=habsVt->FindFirstBinAbove(VRMS[ich]*MSIGMACUT[ich]);
	RxTOT[ich]=habsVt->FindLastBinAbove(VRMS[ich]*MSIGMACUT[ich])-iNSigBin[ich];
	Rx[ich].A=habsVt->GetMaximum();
	if(iNSigBin[ich]>0)hSNRVMax[ich]->Fill(Rx[ich].A/VRMS[ich]);
	if(hSNRVMax[ich]->GetEntries()>10 && hSNRVMax[ich]->GetMean()/2.>MSIGMACUT[ich]){
	  //	  printf("Changing MSIGMACUT[%d] from %f to %f\n",ich,MSIGMACUT[ich],hSNRVMax[ich]->GetMean()/2.);
	  //	  MSIGMACUT[ich]=hSNRVMax[ich]->GetMean()/2.;
	}
	if(isCalPulser==1 && NEvWrite<MEVWRITE){
	  if(iNSigBin[ich]>=0)
	    CPwfof<<"#Ch "<<ich<<" "<<xt[iNSigBin[ich]]<<" "<<Rx[ich].A<<endl;
	  else
	    CPwfof<<"#Ch "<<ich<<" NO HIT"<<endl;
	  for(Int_t ibins=0; ibins<nVt0; ibins++){
	    CPwfof<<xt[ibins]<<" "<<Vt[ibins]<<endl;
	  }
	}
	habsVt->Delete();
	hVt->Delete();
	//	gVt->Delete();
	//	grch[ich]->Delete();
	//	grfftch[ich]->Delete();
	delete gVt0;
	//	gVt0->Delete();
	//	gVt1->Delete();
	
	Float_t VMinHit=0; //tbd!!!!
	
	//	if(araGeom->fAntInfo[ich].polType==0 && iNSigBin[ich]>0 && ich<14){ 
	Rx[ich].dtFitResid=9999;
	//vertex here with VPol only (polType==0):
	if(iPol==0)LHit=iNSigBin[ich]>0 && int(stationInfo->getAntennaInfo(ich)->polType)==0;
	if(iPol==1)LHit=iNSigBin[ich]>0 && int(stationInfo->getAntennaInfo(ich)->polType)==1;
        if(iPol>=2)LHit=iNSigBin[ich]>0 && abs(iVMaxBin[ich]-iNSigBin[ich])<MDT_VTHRESH_VMAX; 
	if(iPol<=2)LHit=LHit && ich<MCHINICE;
	if(istation==1)LHit=LHit&&ich!=10;
	//	LHit=LHit && RxTOT[ich]<RxTOTMAX;
	
	if(LHit){ 
	  iantMax=ich;
	  iuseAnt[ich]=1;
	  antForFit[ich]=iuseAnt[ich];
	  NUseAnt[ich]++;
	  LuseAnt[ich]=true; // leave false if known bad channel
	  RxHitCode2+=pow(2,ich);
	  RxHitCode10+=pow(10,ich);
  	  threshold[ich]=VRMS[ich]*MSIGMACUT[ich];
	  Rx[ich].HitTime=xt[iNSigBin[ich]];
	  outTimes <<"EventNumber: "<< eventNumber <<" chan number: "<< ich << " insigbin: "<<iNSigBin[ich] <<" \t xt[used]: "<<xt[iNSigBin[ich]]<<" \t Threshold: " << threshold[ich] <<" \t xt[0]: "<<xt[0] <<" \t xt[n-1]: "<<xt[nVt0-1] <<" \n";

	  
	  if(istation==2){
	  	    if(LHoleD3Ara02[ich])Rx[ich].HitTime-=10;
	    //	    if(ich==3||ich==7||ich==11||ich==15)Rx[ich].HitTime-=15;
	    //	    Rx[ich].HitTime-=Rx[ich].dtNsCorr;
	  }

	  TempTime[ich]=Rx[ich].HitTime;
	  rawTime[ich]=Rx[ich].HitTime;
	  chNumber[ich]=stationInfo->getAntennaInfo(ich)->chanNum;
	  chanName[ich]=stationInfo->getAntennaInfo(ich)->getRFChanName();
	  //	  if(Rx[ich].cPol=='H' && ich<8)Rx[ich].RawHitTime+=HPolFeedDelaynsBCS;  
	  //	  if(Rx[ich].cPol=='V' && ich<8)Rx[ich].RawHitTime+=VPolFeedDelaynsBiC;  
	  
	  TGrid[nhits]=Rx[ich].HitTime*1e-9;
	  VGrid[nhits]=VMaxBin;
	  
	  xyzGrid[nhits][0]=Rx[ich].x;
	  xyzGrid[nhits][1]=Rx[ich].y;
	  xyzGrid[nhits][2]=Rx[ich].z;

	  nhits++;
	  if(int(stationInfo->getAntennaInfo(ich)->polType)==1)nhitsV++;
	  if(int(stationInfo->getAntennaInfo(ich)->polType)==0)nhitsH++;
	  if(istation>=1&&ich>=16)nhitsS++;
	  if(istation==0&&ich>=14)nhitsS++;
	//	if(Rx[ich].cPol!='V' && araGeom->fAntInfo[ich].polType==0)cout<<"mismatch"<<endl;
	  if(addDelay){
	    Rx[ich].HitTime -= stationInfo->getCableDelay(ich);
	  }
	  if(Rx[ich].HitTime<T0event){
	    T0event=Rx[ich].HitTime;
	    iChFirstHit=ich;
	  }
	}
	if(isCalPulser==1 && NEvWrite<MEVWRITE)CPwfof<<endl<<endl;
      } //end of iteration over channels

      N->Fill(WfPSum,Tus);
      Bool_t LDum;
      if(isCalPulser==1 && ievtdraw<MAXEVDRAW){
	cIFG->cd(1);
	//	sprintf(cpngname,"/home/dbesson/ctem/ahIFG%d_station%d.png",ievt,istation);
	if(istation>0)TH2D *histMapV=theCorrelator->getInterferometricMap(realAtriEvPtr,AraAntPol::kVertical);
	else TH2D *histMapV=theCorrelator->getInterferometricMap(realIcrrEvPtr,AraAntPol::kVertical);
	sprintf(chName,"vmap%d",ievt);
	histMapV->SetName(chName);
	histMapV->SetTitle("VPol");
	histMapV->Draw("colz");

	cIFG->cd(2);
	if(istation>0)TH2D *histMapH=theCorrelator->getInterferometricMap(realAtriEvPtr,AraAntPol::kHorizontal);
	else TH2D *histMapH=theCorrelator->getInterferometricMap(realIcrrEvPtr,AraAntPol::kHorizontal);
	sprintf(chName,"hmap%d",ievt);
	histMapH->SetName(chName);
	histMapH->SetTitle("HPol");
	histMapH->Draw("colz");
       
	//	cIFG->SaveAs(cpngname);
	delete histMapV;
	delete histMapH;
      }

      TMath::Sort(MCH,TempTime,index,true);
      //      for(Int_t ich=0; ich<MCH; ich++){
	/*
	Int_t jch=index[ich];
	if(LuseAnt[jch]){
	  hhits[jch]->Fill(Rx[jch].x, Rx[jch].y, Rx[jch].z, Rx[jch].A);
	  printf("TT=%f / Rx[%d].A=%f\n",TempTime[jch],jch,Rx[jch].A);
	}
	*/
	//      }

      Bool_t LVtxEv=nhits>=4;
      
      if(goodEvent){
	Bool_t isGood=true;
	for(int i=0; i<20; i++){
	  if(antForFit[i])
	    if(rawTime[i]<0)
	      isGood=false;
	}
	//	LVtxEv= LVtxEv && isGood;
	if(LVtxEv && isGood)
	  LVtxEv=true;
	else 
	  LVtxEv=false;
      }
     

      if(LVtxEv){
	outTimes <<"Was event with >=4 nhits"<<"\n";
	cout<<"VERTEXING EVENT "<<endl;
      
	vx=vy=vz=9999;
	// This test reconstrction heavily slows down all process
	//	if(fmod(ievt,10)==0)
	if(false){
	  vertexfinder(nhits,rice,vtx,vtxp,&xyzGrid[0][0],&TGrid[0],&iuseAnt[0],&VGrid[0],&gains[0],t0vtx,tresvtx,vtxrms,vtxcone);
	  abort();
	}
	vvtx[0]=vx;
	vvtx[1]=vy;
	vvtx[2]=vz;
	if(vx!=0)phivtx=atan2(vy,vx);
	thevtx=acos(vz/(vx*vx+vy*vy+vz*vz));
	cout<<"vx/vy/vz="<<vx<<"/"<<vy<<"/"<<vz<<endl;
	/**
	Vtx[0]=T0event-256; //Should be set to the earliest cable-delay corrected time
	Vtx[1]=Rx[iChFirstHit].x*RScaleFirstHit; //x-coord of earliest antenna
	Vtx[2]=Rx[iChFirstHit].y*RScaleFirstHit; //y-coord of earliest antenna
	Vtx[3]=Rx[iChFirstHit].z ;//z-coord of earliest antenna
	**/
	
	Vtx[0]=T0event; //Should be set to the earliest cable-delay corrected time
	Vtx[1]=Rx[iChFirstHit].x; //x-coord of earliest antenna
	Vtx[2]=Rx[iChFirstHit].y; //y-coord of earliest antenna
	Vtx[3]=Rx[iChFirstHit].z;//z-coord of earliest antenna
	sTime = Vtx[0];
	sX = Vtx[1];
	sY = Vtx[2];
	sZ = Vtx[3];
	eventrecoMinuit->mnparm(0,"t0",Vtx[0],step[0],MINUIT_TMIN,MINUIT_TMAX,ierflg);
	eventrecoMinuit->mnparm(1,"x0",Vtx[1],step[1],MINUIT_XMIN,MINUIT_XMAX,ierflg);
	eventrecoMinuit->mnparm(2,"y0",Vtx[2],step[2],MINUIT_YMIN,MINUIT_YMAX,ierflg);
	eventrecoMinuit->mnparm(3,"z0",Vtx[3],step[3],MINUIT_ZMIN,MINUIT_ZMAX,ierflg);
	
	eventrecoMinuit->mnexcm("SIMPLEX", argsSimplexMigrad, 2, convergence[0]);	// SIMPLEX doesn't use UP parameter as soon as tolerance is specified
	eventrecoMinuit->mnexcm("MIGRAD", argsSimplexMigrad, 2, convergence[1]);	// But MIGRAD somehow uses UP along with tolerance
	
	eventrecoMinuit->mnstat(chi_square, tmp_double, tmp_double, tmp_int, tmp_int, tmp_int);
	eventrecoMinuit->GetParameter(0, FitPoint[0], FitPerror[0]);
	eventrecoMinuit->GetParameter(1, FitPoint[1], FitPerror[1]);
	eventrecoMinuit->GetParameter(2, FitPoint[2], FitPerror[2]);
	eventrecoMinuit->GetParameter(3, FitPoint[3], FitPerror[3]);
	//	FitPoint[3]-=Station_cog_z;
	chisqvtx=chi_square;
	phi=atan2(FitPoint[2],FitPoint[1]);
	the=acos(FitPoint[3]/sqrt(FitPoint[1]*FitPoint[1]+FitPoint[2]*FitPoint[2]+FitPoint[3]*FitPoint[3]));
	tresidMax=0;
	for(int ich=0; ich<MCH0; ich++){
	  bool useFitPoint = false; // Use fit points to calculate deltaTime (false for calibration procedure)
	  int usedPulser = 2; // Number of the pulser currently in use in the Tx array
	  //p Assumed real position of the pulser
	  if(false){
	    abort();
	    const double x = useFitPoint ? FitPoint[1] : Tx[usedPulser].x;
	    const double y = useFitPoint ? FitPoint[2] : Tx[usedPulser].y;
	    const double z = useFitPoint ? FitPoint[3] : Tx[usedPulser].z;
	  }
	  else{
	    const double x=FitPoint[1];
	    const double y=FitPoint[2];
	    const double z=FitPoint[3];
	  }
	  float DistanceSq=
	    (x-Rx[ich].x)*(x-Rx[ich].x)+
	    (y-Rx[ich].y)*(y-Rx[ich].y)+
	    (z-Rx[ich].z)*(z-Rx[ich].z);
	  Float_t TransitTimens=ntimexp(0.001*z, 0.001*Rx[ich].z, 0.001*sqrt(DistanceSq));
	  deltaTime[ich]=FitPoint[0]+TransitTimens-Rx[ich].HitTime;
	  Rxtresid[ich]=Rx[ich].dtFitResid;
	  of1<<Rx[ich].dtFitResid<<" ";
	  if(fabs(Rx[ich].dtFitResid)>tresidMax && iuseAnt[ich])
	    tresidMax=fabs(Rx[ich].dtFitResid);
	}
	SimRxHitTimeNs(122.067*ft2m,-61.0946*ft2m,-633.199*ft2m);
	for(Int_t ich=0; ich<MCH0; ich++){
	  if(LuseAnt[ich] && LuseAnt[0]){
	    printf("Tx HOLE 1: Channel %d / deldTij=%f\n",ich,Rx[ich].deldTij);
	    hdeldTij_CP1->Fill(Rx[ich].deldTij,ich);
	  }
	}

	SimRxHitTimeNs(56.3175*ft2m,114.299*ft2m,-551.514*ft2m);
	for(Int_t ich=0; ich<MCH0; ich++){
	  if(LuseAnt[ich] && LuseAnt[0]){
	    printf("Tx HOLE 2: Channel %d / deldTij=%f\n",ich,Rx[ich].deldTij);
	    hdeldTij_CP2->Fill(Rx[ich].deldTij,ich);
	  }
	}
	of1<<endl;

	//	hhits[iChFirstHit]->SetMarkerStyle(kFullSquare);
	T0=Rx[iChFirstHit].HitTime;
	hFirstHitTime->Fill(Rx[iChFirstHit].HitTime);
	cout<<TempTime[index[nhits-1]]<<"/"<<Rx[iChFirstHit].HitTime<<endl;
	ievtdraw++;
	sprintf(ctitle,"h3d0_xvtx=%f_yvtx=%f_zvtx=%f",FitPoint[1],FitPoint[2],FitPoint[3]);
	c1->cd();
	h3d0->SetTitle(ctitle);
        h3d0->Reset();
	h3d0->Fill(FitPoint[1],FitPoint[2],FitPoint[3]);
	h3d0->Draw();
	//	h3dTx1->Draw();
	//	h3dTx2->Draw("same");

	for(Int_t ich=0; ich<MCH0; ich++){
	  if(LuseAnt[ich]){
	    htRx->Fill(Rx[ich].HitTime-Rx[iChFirstHit].HitTime,ich);
	    if(isCalPulser==1)htRxCP->Fill(Rx[ich].HitTime-Rx[iChFirstHit].HitTime,ich);
	    Int_t iMarkerColor=(Rx[ich].HitTime-Rx[iChFirstHit].HitTime)/5;
	    if(iMarkerColor<0)
	      printf("Rx[%d].HitTime=%f TimeDelayNsCorr=%f / First Rx[%d].HitTime=%f TimeDelayNsCorr=%f\n",ich,Rx[ich].HitTime,Rx[ich].TimeDelayNsCorr,iChFirstHit,Rx[iChFirstHit].HitTime,Rx[iChFirstHit].TimeDelayNsCorr);
	    hhits[ich]->SetMarkerColor(51+iMarkerColor);
	    hhits[ich]->SetMarkerSize(Rx[ich].A/200);
	    hhits[ich]->Draw("same");
	  }
	}
	//	sprintf(cpngname,"~/ctem/aa%d.png",ievtdraw);
	//	if(ievtdraw<MAXEVDRAW)c1->SaveAs(cpngname);
      NEvTot++;
      tree->Fill();
      }
      if(isIcrrEvent) delete realIcrrEvPtr;
      else if(isAtriEvent)	delete realAtriEvPtr;
      if(fmod(NEvTot,100)==0)tree->Show(NEvTot);
      //      NEvTot++;
    }
   delete eventTreeData;
  
  f.cd();
  f.Write();
  f.Close();
  //  cout<<"Closing "<<crootfileout<<" after reading in "<<NFilesReadIn<<" files"<<endl;
  exit(0);
}

void calc_chi_square(Int_t &npar, Double_t *gin, Double_t &chisquare, Double_t *par, Int_t iflag){
  const double sLightAir = 0.29970551819; //velocity of light in meters/ns
  const double nIce=rice; //1.78; for calibration pulser
  float chi=0.;
  chisquare = 0.0;
  ndof=0;
  niter++;
  for(int i=0; i<MCH0; i++){
    if(iuseAnt[i]>0){
      ndof++;
      float DistanceSq=
	(par[1]-Rx[i].x)*(par[1]-Rx[i].x)+
	(par[2]-Rx[i].y)*(par[2]-Rx[i].y)+
	(par[3]-Rx[i].z)*(par[3]-Rx[i].z);
      Float_t TransitTimens=sqrt(DistanceSq)/(sLightAir/nIce);
      TransitTimens=ntimexp(0.001*par[3],0.001*Rx[i].z,0.001*sqrt(DistanceSq));
      //      if(Rx[i].TimeDelayNsCorr!=0)cout<<"ERROR!!!"<<endl;
      Float_t delta=par[0]+TransitTimens-Rx[i].HitTime;
      Rx[i].dtFitResid=delta;
      chi=delta; //Rx[i].hitweight;
      chisquare+=chi*chi; //hack!!!!!
      ChiSqTot=chisquare;
    }
  }
  return;
}

//returns array of hit time differences between data and sim in deldT
 void SimRxHitTimeNs(Float_t xTx, Float_t yTx, Float_t zTx){
  Float_t dsqr=0;
  for(Int_t ich=0; ich<MCH0; ich++){
    Rx[ich].deldTij=-9999;
    dsqr=(xTx-Rx[ich].x)*(xTx-Rx[ich].x) +
      (yTx-Rx[ich].y)*(yTx-Rx[ich].y) +
      (zTx-Rx[ich].z)*(zTx-Rx[ich].z);
    Rx[ich].dtSimTx=sqrt(dsqr)/sLightAir;
    Rx[ich].dtSimTx=ntimexp(m2km*zTx,m2km*Rx[ich].z,m2km*sqrt(dsqr));
    Rx[ich].deldTij=(Rx[ich].dtSimTx-Rx[0].dtSimTx)-(Rx[ich].HitTime-Rx[0].HitTime);
  }
}
