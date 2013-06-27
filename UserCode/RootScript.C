//AraRoot v3.3 - currently runs on list files indicated below in /home/dbesson/list
//to run on condor, would have to modify the iDataSet to run on all janfeb11->august11 data
//variables: iDataSet, cListFileIn (no need to worry about them)


gROOT->Time();
gSystem->Reset();
#include "TVirtualFFT.h"
#include "loadlib.h"
#include "vtxutil.h"
TRandom rand;
//#include <string.h>
//#include "VMath.h"
using namespace std;

const Int_t IBINOFFSET=3;
const Int_t DELIPKVALS=150;
const Int_t NSECLIVECALC=1000; //calculate livetime every NSECLIVECALC seconds
const Int_t NDTSETMIN=8;
const Int_t TNS_OFFSET_TIMERX=64+16; //offset
const Int_t TLatch=16;
const Float_t TusCPMAX=4000;
const Float_t MSIGKTMAX=16;
const Float_t RWFPMIN=1.5;
//const Float_t MILLION=1e6;
//const Float_t FMAXSIGMIN=0.06;
const Int_t MCHPASSFMAXSIGCUT=2;
const Float_t xcvalUnbXUnb; //xcval for MC events embedded into same unbiased waveform
const Float_t micro=1.e-6;
const Float_t RxHitTime00=99999;
const Int_t MAXPAT=16; 
const Int_t MAXPATAVG=32;
const Int_t MAXUNB=MAXPAT;
const Int_t MPNGSAVE=16;
const Int_t MPRINTEV=64;
const Int_t MPRESCALE=1;
const Int_t MCH=16;
//const Int_t MAXPATMC=256*4;
const Float_t fInterpScale=0.5;

const Float_t xcvalMinCorrAndAvg=20000;
//const Float_t xcvalMinNewTemplate=10000;
const Float_t hdT1d_rmsValMaxCor_MatchTemplateMax=2056; //not sure what the intent of this was...
const Int_t MAXANACALPULSER=1024; //maximum number of cal pulser events to analyze
const Int_t MBINS=8192;
const Int_t MBINSIG=32;
const Int_t MBINDIFFNSIGMAX=32;
Double_t residChi2[MBINSIG*MBINSIG];
/* TIGHT CUTS: Give essentially ZERO templates:
const Float_t CHISQPERHITMAX=500; 
const Float_t WfPowerMinNewTemplate=1.5e7; //1.6e7; 
const Float_t xcvalMinMatchTemplate=16000; 
const Int_t NHITSMIN=3; //must be large enough to have >1 ndeltaTSqSum
const Float_t FMAXSIGMIN=0.06;
const Float_t XCORXXMIN=8000;
const Float_t VMAXRMSMIN=0.6;
const Float_t VMAXAVGMIN=5;
const Float_t RXTOTMAXPERDOF=32; //128; //avg 16 samples over threshold per channel
const Float_t dTSqSumMatch=4;
const Float_t grXCorAMinMatchTemplates=248;
const Float_t delTTMin=2; //minimum trigger time between successive templates
const Int_t MTHRESH=20;
const Float_t NCHTRIG=5;
const Float_t NSIGHIT=4.5;
*/

const Float_t THETEASMAX=1.75; //cross-over between something from above (EAS) vs. below (nu)
const Float_t CHISQPERHITMAX=1600; 
Float_t WfPowerMinNewTemplate=1.6e7; //1.6e7; 
Float_t xcvalMinMatchTemplate=16000; //16000; 
const Int_t NHITSMIN=4; //must be large enough to have >1 ndeltaTSqSum
const Int_t NHITSRECO=4; //must be large enough to have >1 ndeltaTSqSum
const Float_t SNRMAXFREQMAX=6.5; //7.5;
const Float_t SNRMINFREQFILTER=6.5;
const Float_t XCORVHMAX=2.2;
const Float_t FMAXSIGMIN=0.06;
const Float_t XCORXXMIN=7000;
const Float_t XCORXXMAX=10000;
const Float_t VMAXRMSMAX=6; //3.5;  
const Float_t VMAXAVGMIN=-9999;  //5
const Float_t RXTOTMAXPERDOF=64; //128; //avg 16 samples over threshold per channel
const Float_t dTSqSumMatch=0.1;
const Float_t grXCorAMinMatchTemplates=248;
const Float_t delTTMin=2; //minimum trigger time between successive templates
const Float_t VMAXMV=1024;
const Int_t MTHRESH=20;
const Float_t NCHTRIG=5;
Float_t NSIGHIT=4.5;

Float_t xNtuple[59]; //*xNtuple no work 

//Int_t NThresh[MTHRESH][3]={{0}};
Int_t NThresh[MTHRESH]={0};
Int_t NChHitTrigger=0;
Int_t NEvSequential=0;
Int_t NComb32=0;
Float_t SkewSum=0;
Float_t KurtSum=0;
Float_t XCorSum=0;
Float_t ZDot;
Float_t ZDot32[MAXPAT]={0};
Float_t fresidChi2_32[MAXPAT]={0};
Float_t fKGTest32[MAXPAT]={0};
Float_t fChi2Test32[MAXPAT]={0};
Float_t fChi2Test, fKGTest, fresidChi2Test;
Float_t ExMax, EyMax, EzMax;
Float_t RxCOGz=0;
Float_t ScaleFac;
Float_t xCorVV, xCorHH, xCorVH, xcMax;
Float_t zFirstHit[MAXPAT]={0};
Float_t dzFirstHit;
Float_t SNRMaxFreq, fSNRMaxFreq;
Float_t Chi2TestOut;
Float_t MaxFreqFFTSNR[MCH][MAXPAT]={{0}};
Float_t MaxFreqFFT[MCH][MAXPAT]={{0}};
Float_t MaxFreqFFTSNRch[MCH]={0};
Float_t MaxFreqFFTch[MCH]={0};
Float_t fpkValMapV[MAXPAT]={0};
Float_t fpkValMapH[MAXPAT]={0};
Float_t fPkValMapV[MAXPAT]={0}; 
Float_t fPkValMapH[MAXPAT]={0};
Int_t ipkValPhiV[MAXPAT], ipkValTheV[MAXPAT], ipkValPhiH[MAXPAT], ipkValTheH[MAXPAT];
Int_t iPkbinPhiV[MAXPAT], iPkbinTheV[MAXPAT], iPkbinPhiH[MAXPAT], iPkbinTheH[MAXPAT];
const Float_t WfPowerMinNewTemplateMax=3.2e7;
//Float_t WFPMNTM=WfPowerMinNewTemplateMax;
Bool_t LDebug=false;
Bool_t LPrintx=false;
Bool_t LMaxTemplates=false;
Bool_t LSaveUnbiased=false;
Bool_t LCalPulser, LUnBiased;
//Bool_t LSNRMaxFreqAna=false;
Bool_t LFitNuSig=true;
Bool_t LChi2Test=true;
Bool_t LStoreAllChs=false;
Bool_t LDrawPngs=false;
Bool_t LTemplate0, LTemplate1; //, LFitRxCh[MCH], 
//Bool_t LMatchedTemplate[MAXPAT]={0};
Bool_t LMatchedTemplate0[MAXPAT]={1};
Bool_t LUseChT[MCH][MAXPAT];
Int_t it0, jt0, NlenUnb;
Float_t grXCorT, grXCorA;
Double_t TEventRF=0;
Double_t TEventRF0=0;
Double_t TFirstEvent=0;
Double_t TEvent=0;
Double_t TEvent0=0;
Double_t *xUnb;
Double_t *yUnb;
Float_t deltaTxcSqSum=0;
Float_t deltaTSqSum=0;
Float_t deltaASqSum=0;
Float_t deltaTxcSqSum=0;
Float_t fdelTSqSum=0;
Int_t ndeltaTSqSum;
Int_t ndeltaTSqSumMin;
Float_t NSigWfPT=4;
//Float_t WfPowerCalPulserEv=0;
Float_t WfPowerTF1MC=0;
Float_t WfPowerMC0=0;
Float_t WfPowerUnbMC=0;
Float_t WfPowerUnb0=0;
Float_t WfPowerCh=0;
Float_t WFPMNT=WfPowerMinNewTemplate;
Double_t Chi2X;
Int_t ndfX, igoodX;
Int_t iTemplCalPulser=0;
Int_t iUnbiasEvNo[MAXUNB];
Int_t npat[MAXPAT];
Int_t iTrigger[MAXPAT]={0};
//Int_t NEvCuts[16];
Int_t itrigCodePat[MAXPAT];
Int_t NEvCutsFail=0;
Int_t iEvFailCode2;
Int_t NTrigs[3]={0};
Int_t NbinDiffNSigMax[MAXPAT], NbindelNSigMax;
Int_t nhits0[MAXPAT];
Int_t irunno[MAXPAT];
Int_t ievtno[MAXPAT]; 
Float_t RXTOT[MAXPAT];
Float_t grVT[MBINS];
Float_t grVT0[MAXPAT]; //0
Float_t grVT00;
Float_t fLiveInst=0;
Int_t iEvtNo, iRunNo;
Int_t nCalPulserMatch=0;
Int_t nCalPulserPass0=0;
Int_t nEvCalPulser=0;
Int_t nEvCalPulser0=0;
UInt_t trighour=0;      
UInt_t trigmin=0;      
UInt_t trigsec=0;
UInt_t trigday=0;
Float_t xcvalMax;
Float_t fdelTSqSumMin;
Float_t xcValMax[MAXPAT];
Float_t RxTOT, RxTOT0;
Float_t fMaxSig[MCH];
Float_t peakBinXMC[MCH], peakValXMC[MCH], peakBinXMC1[MCH], peakValXMC1[MCH];
const Float_t LPRx[MCH]={.11,.11,.11,.11, .11,.11,.11,.11, .11,.11,.11,.11, .03,.03,.03,.03};
const Float_t HPRx[MCH]={0.8,0.8,0.8,0.8, 0.8,0.8,0.8,0.8, 0.8,0.8,0.6,0.6, 0.6,0.6,0.3,0.3};
const Float_t BWRx[MCH]={0.8,0.8,0.8,0.8, 0.8,0.8,0.8,0.8, 0.8,0.8,0.6,0.6, 0.6,0.6,0.3,0.3};
const Float_t CCWD0=0.05; //c-cone scale width
Float_t RxdTPos[MCH];
Float_t VMax[MCH];
Int_t NEvUnbias=0;
Int_t itrigCodeZ;
Float_t Vrms68[MCH];
Float_t RxHitTime[MCH], RxHitTimeTest[MCH], fFFTMax[MCH], AFFTMax[MCH];
Float_t RxHitTime0[MCH][MAXPAT];
Float_t RxHitTimeTempl[MCH][MCH][MAXPAT];
Float_t xcHitTimeTempl[MCH][MCH][MAXPAT];
Float_t dtMCTempl[MCH][MCH][MAXPAT];
Float_t SNRRxHitTime[MCH];
Float_t SNRRxMaxVCh[MCH];
Float_t SNRRxMaxVAvg;
Float_t SNRRxMaxV[MCH][MAXPAT]={{0}};
Int_t jTemplate;
Float_t WfPowerc[MCH];
Float_t WfPowerMaxPos[MAXPAT]={0};
Float_t WfPowerMaxNeg[MAXPAT]={0}; 
Float_t NuSigChisquare[MAXPAT]={0};
Float_t WFPMaxPos, WFPMaxNeg, NuSigChi2;
Float_t dtVT[MCH], t0VT[MCH], dfVF[MCH], f0VF[MCH];
Float_t WfPower, WfPowerSurf, WfPSig;
Float_t ShowerZenithAngle, ShowerAzimuthAngle, PrimaryParticleEnergy;
Int_t NX0;
#include "vtxMinuit.h"
const Int_t TB_ICRR_StationNo=0;
const Int_t station=TB_ICRR_StationNo;
const Float_t VkT=50;

Int_t iCutsLevel=0;
Int_t itemplate=0;
Float_t deadTime1_tot, deadTime2_tot;
Double_t deadTime_tot;
Double_t fracLive;
  
TH2F *hEvCuts=new TH2F("hEvCuts","hEvCuts|trigType",M_NEVCUTS,-0.5,M_NEVCUTS-0.5,3,-0.5,2.5);
TH1F *hFailX=new TH1F("hFailX","hFailX",M_NEVCUTS,-0.5,M_NEVCUTS-0.5);
TH1F *hFailI=new TH1F("hFailI","hFailI",M_NEVCUTS,-0.5,M_NEVCUTS-0.5);

void IncrementCuts(Bool_t LCut){
  LTemplate0=(LTemplate0&&LCut);
  if(LTemplate0){
    hEvCuts->Fill(iCutsLevel,itrigCodeZ);
    if(iCutsLevel>=0)
      NEvCuts[iCutsLevel][itrigCodeZ]++;
  }
  if(!LCut){
    iEvFailCode2+=pow(2,iCutsLevel); 
    if(!LCalPulser)
      hFailI->Fill(iCutsLevel);
  }
  iCutsLevel++;
}


void PrintOutNEvCuts(){
  cout<<"NMatch/NUnMatch CP evts="<<nCalPulserMatch<<"/"<<nCalPulserPass0<<endl;
  deadTime_tot=(deadTime1_tot*65536.0+deadTime2_tot*1.0)/10000000.0;
  fracLive=1.-deadTime_tot/(TEvent-TFirstEvent);
  printf("TotalDataTakingTime=%g nEV=%d nEV0=%d nCP=%d nUB=%d dead1=%g dead2=%g fracLive=%g fracLiveCP=%g fracLiveInstCP=%g fracLiveUB=%g\n",TEvent-TFirstEvent,NEvTot,NEvTot0,nEvCalPulser,NEvUnbias,deadTime1_tot,deadTime2_tot,fracLive,float(nEvCalPulser)/(TEvent-TFirstEvent),fLiveInst,float(NEvUnbias)/(TEvent-TFirstEvent));
  for(Int_t jtrigCodeZ=0; jtrigCodeZ<3; jtrigCodeZ++){
    for(Int_t icut=0; icut<M_NEVCUTS; icut++){
      if(NEvCuts[icut][jtrigCodeZ]>0)
	printf("NEvCuts[%d][%d]=%d  ",icut,jtrigCodeZ,NEvCuts[icut][jtrigCodeZ]);
    }
    cout<<endl;
  }
  cout<<endl;
}

Float_t NoiseOnly(Float_t Vchrms){
  Float_t noise_Im=gRandom->Gaus(0,Vchrms);
  Float_t noise_Re=gRandom->Gaus(0,Vchrms);
  Float_t noise_phase=atan2(noise_Im,noise_Re);
  Float_t noise_mag=sqrt(noise_Im*noise_Im+noise_Re*noise_Re);
  return noise_mag*cos(noise_phase);
}
  
Float_t DampExpPlusNoise(Double_t *x, Double_t *par){
  //  if(x[0]<TFirstNSig||x[0]>TFirstNSig+32){TF1::RejectPoint();      return 0;}
  //A*exp((x-t0)/tau)*cos(omega*(x-t0)+phi)
  Float_t noise_Im=gRandom->Gaus(0,VkT/sqrt(2.));
  Float_t noise_Re=gRandom->Gaus(0,VkT/sqrt(2.));
  Float_t noise_phase=atan2(noise_Im,noise_Re);
  Float_t noise_mag=sqrt(noise_Im*noise_Im+noise_Re*noise_Re);
  if(x[0]<par[1] || fabs(par[0])>VMAXMV)
    return noise_mag*cos(noise_phase);
  else
    return par[0]*exp(-(x[0]-par[1])/par[2])*cos(par[3]*(x[0]-par[1])+par[4])+noise_mag*cos(noise_phase);
    
}

Double_t DampExp(Double_t *x, Double_t *par){
  //  if(x[0]<TFirstNSig||x[0]>TFirstNSig+2*SigNanoSec){TF1::RejectPoint();     return 0;}
  //A*exp((x-t0)/tau)*cos(omega*(x-t0)+phi)
  if(fabs(par[0])<VMAXMV)
    return par[0]*exp(-(x[0]-par[1])/par[2])*cos(par[3]*(x[0]-par[1])+par[4]);
  else
    return 0;
}

int RootScript(char * inFileName, char * outFileName, string outputPath){
  
  Int_t MAXEVT=30;
  //  Int_t iDataSet=0; //this variable will have to be modified!
  Int_t iDataSet=4;
  Int_t iEV=0;
  Int_t iEAS=0;
  Int_t iMC=0;
  Int_t iCP=0;
  Int_t iUB=0;
  Int_t iTM=0;

  const Int_t MAXCALPULSERTEMPLATES=MAXPAT/4;

  TCanvas *c3=new TCanvas("c3","c3");
  TCanvas *c2=new TCanvas("c2","c2");
  c2->Divide(4,4);
  TCanvas *c1=new TCanvas("c1","c1");
  const Float_t AraRootv=3.3;
  gRandom->SetSeed(0);
  cout<<"First random number="<<gRandom->Rndm(1)<<endl;
  loadlib();
  datestamp=datechar();

  Int_t NFileAna=0;
  Int_t NEvMatchedXCor=0;
  Int_t NEvMatcheddT=0;
  Int_t NEvTemplate0=0;
  Int_t NCHgt400MHz;
  Int_t runNumber;    
  UInt_t headerIevt;
  Int_t iEvNoOld, iEvNoThis;
  Int_t jrx;
  Int_t NRXMC;

  Int_t nPat, iTrig;
  Double_t Tsec, Tus;
  Float_t the0[MAXPAT], phi0[MAXPAT], r2vtx0[MAXPAT], chisq0[MAXPAT];
  Float_t the=0;
  Float_t phi=0;
  Float_t r2vtx=0;
  Float_t dTT,VMaxVRms,VMaxHRms,VMaxVAvg,VMaxHAvg,xCorVV,xCorHH,xCorVH;
  Double_t TTemplate1=0;
  Double_t TTemplate0=0;
  Double_t delTT;
  Float_t WfPT, WfPTS, WfPThresh;
  const Int_t MRXSAMPLE=512;
  Float_t xtemp[MRXSAMPLE*2], ytemp[MRXSAMPLE*2], VMC[MRXSAMPLE*2], tMC[MRXSAMPLE*2], VtMC[MRXSAMPLE*2], VtUnbMC[MRXSAMPLE*2], tUnbMC[MRXSAMPLE*2], tSim[MRXSAMPLE*2], tSimMC[MRXSAMPLE*2], VSim[MRXSAMPLE*2];

  Int_t nSurfHitMC[MAXPAT], nSigktHitsMC[MAXPAT], nSurfHit, nSigktHits;
  Float_t NSigkTMax, TimeRx, ConeWid, showerX, showerY, showerZ, showerE, nuE, launchAngle, showerPhi, showerTheta;
  Float_t showerXMC[MAXPAT], showerYMC[MAXPAT], showerZMC[MAXPAT], showerEMC[MAXPAT], nuEMC[MAXPAT], launchAngleMC[MAXPAT], showerPhiMC[MAXPAT], showerThetaMC[MAXPAT];
  Float_t TimeRxMC[MCH][MAXPAT];
  //  Float_t VTXx[MAXPAT], VTXy[MAXPAT], VTXz[MAXPAT];
  Int_t ipkValIFGphiV, ipkValIFGphiH, ipkValIFGtheV, ipkValIFGtheH;
  Float_t fpkValIFGV, fpkValIFGH;
  Int_t iPkbinIFGphiV, iPkbinIFGphiH, iPkbinIFGtheV, iPkbinIFGtheH;
  Float_t fPkValIFGV, fPkValIFGH;
  char crootfileout[256], gName[64], hName[64], cName[64], blank[64], chname[64], cpngname[64], cUnbiasOut[128], cFileIn[256], cListFileIn[64], cMatchGraphsOut[128], cTemplateOut[128];

  if(iDataSet<0)strcpy(cListFileIn,"janfeb11_3.2_TB");
  if(iDataSet==0)strcpy(cListFileIn,"mar11_3.2_TB");
  if(iDataSet==1)strcpy(cListFileIn,"apr11_3.2_TB");
  if(iDataSet==2)strcpy(cListFileIn,"may11_3.2_TB");
  if(iDataSet==99)strcpy(cListFileIn,"jun11_3.2_TB");
  if(iDataSet==3)strcpy(cListFileIn,"jul11_3.2_TB");
  if(iDataSet==4)strcpy(cListFileIn,"aug11_3.2_TB");
  //  strcpy(cListFileIn,"feb12_3.7_TB");
  //  strcpy(cListFileIn,"mar12_3.7_TB");
  //  strcpy(cListFileIn,"apr12_3.7_TB");
  //  strcpy(cListFileIn,"may12_3.7_TB");

  //output text files of templates:
  //  sprintf(cMatchGraphsOut,"/home/dbesson/ctem/MatchGraphs_%s_%s.txt",cListFileIn,datestamp);
  //sprintf(cMatchGraphsOut,"/home/cbora/Unlev4/MatchGraphs_%s_%s.txt",cListFileIn,datestamp);
  //ofstream MatchGraphsOut(cMatchGraphsOut);
  //  sprintf(cTemplateOut,"/home/dbesson/ctem/TemplateOut_%s_%s.txt",cListFileIn,datestamp);
 
    
  string path = inFileName;
  int nchar=path.find("Run");
  string fileNum=path.substr(nchar,9);
  
  //for MatchGraphs
  string matchOut="MatchGraphs_for_";
  matchOut.append(fileNum).append(".txt");
  // by cbora for testing 
  string outPath="/home/cbora/TestJob/";
    matchOut= outPath.append(matchOut);
  char *nameMatchOut=(char*)matchOut.c_str();

  ofstream MatchGraphsOut(nameMatchOut);
  //  MatchGraphsOut <<"testing by cbora" << endl;

  //for TemplateOut
  //  string fileTemplate="TemplateOut_for_";
  //  fileTemplate.append(fileNum).append(".txt");
  //  char *nameTemplateOut=(char*)fileTemplate.c_str();

  string fileTemplate = outputPath + "TemplateOut.txt";
  char *nameTemplateOut=(char*)fileTemplate.c_str();
  //ofstream TemplateOut(cTemplateOut);
  ofstream TemplateOut(nameTemplateOut);
   
  //  TemplateOut <<"testing by cbora"<<endl; //Testing by cbora


  const Int_t MBINFREQ=10000;
  const Int_t NS2MHZ=1000;
  const Float_t FDTVTFFT=float(NS2MHZ)*2/float(MBINFREQ);

  TH1F *hDot;
  TH1F *hdNdOmega[MCH]; //Askaryan signal in frequency domain 
  //purposely book early so not stored
  for(Int_t ich=0; ich<MCH; ich++){
    sprintf(cName,"hdNdOmega_ch%d",ich);
    hdNdOmega[ich]=new TH1F(cName,cName,MBINFREQ,0,MBINFREQ); //1000 bins from 0->1000 MHz
  }

  //must change appropriate path for output root file 
  //  sprintf(crootfileout,"~/ctem/EV4_%s_%s_ieas%d_imc%d_icp%d_iub%d_itm%d.root",cListFileIn,datestamp,iEAS,iMC,iCP,iUB,iTM);  
  //  sprintf(crootfileout,"/home/cbora/Unlev4/EV4_%s_%s_ieas%d_imc%d_icp%d_iub%d_itm%d.root",cListFileIn,datestamp,iEAS,iMC,iCP,iUB,iTM);  

  //  TFile f(crootfileout,"recreate");
  TFile f(outFileName, "recreate");

  //  TNtuple *NXC=new TNtuple("NXC","NXC","Zdot:Chi2:Chi2resid:KG:ChiSqNu:XCorSum:nhitsH:nhitsV:nhits:igoodX:delTT:SkewSum:KurtSum");
  TNtuple *TdT=new TNtuple("TdT","TdT","dTT:time"); //:hktemp0:hktemp1:hktemp2:hktemp3:hktemp4:hktemp5:hktemp6:hktemp7");
  TNtuple *N0CUTS=new TNtuple("N0CUTS","N0CUTS","SNRMaxFreq:RxTOT:WFP:WFPMaxPos:WFPMaxNeg:Nbindel:nhits:NComb32:xcvalMax:VMaxVRms:VMaxHRms:xCorVV:xCorHH:xCorVH:Chi2minuit:Tus:SkewSum:KurtSum:jt:iTrig:KG:r:the:phi:ChiNuSigFit:ipkValPhiV:ipkValTheV:ipkValPhiH:ipkValTheH:fpkValMapV:fpkValMapH:Zdot:Chi2:delTT:nhitsV:nhitsH:eMC:xMC:yMC:zMC:theMC:phiMC:WfPSurf:WfPSig:run:evt:SNRRxMaxVAvg:NThresh5:NThresh6:NThresh7:fdelTSqSumMin:ndeltaTSqSum:NThresh8:NThresh9:NThresh10:NThresh2:NThresh3:NThresh4:fSNRMaxFreq");
  //  TNtuple *NIFG=new TNtuple("NIFG","IFGPars","ipkValPhiV:ipkValTheV:ipkValPhiH:ipkValTheH:fpkValMapV:fpkValMapH:jt:iTrig:VMaxVRms:VMaxHRms:VMaxVAvg:VMaxHAvg:xCorVV:xCorHH:xCorVH:ipkValPhiV:ipkValTheV:ipkValPhiH:ipkValTheH:fpkValMapV:fpkValMapH");
  TNtuple *NTMPL=(TNtuple*)N0CUTS->Clone();
  NTMPL->SetName("NTMPL");
  NTMPL->SetTitle("NTMPL");
  //new TNtuple("NTMPL","NTMPL","WfP:WfPS:delTT:iTrig:T:Tus:iev:irun:jt:nPat:SNRMaxFreq");
  TNtuple *NUSIG=new TNtuple("NUSIG","NUSIG","ich:A0:t0:tau:omega:phi0:chisq:jt:iTrig");
  TNtuple *NVTX=new TNtuple("NVTX","NVTX","ipkValPhiV:ipkValTheV:ipkValPhiH:ipkValTheH:fpkValMapV:fpkValMapH:eMC:xMC:yMC:zMC:x:y:z:failcode");
  TNtuple *NMC0=new TNtuple("NMC0","NMC0","eMC:xMC:yMC:zMC");
  TNtuple *NMC1=new TNtuple("NMC1","NMC1","eNU:eshMC:xMC:yMC:zMC:LTmpl:NChTrg:iFail:wfP:NChHit:LaunchAng:phiMC:theMC");
  TNtuple *NHITS=new TNtuple("NHITS","NHITS","t0:tMC0:t1:tMC1:t2:tMC2:t3:tMC3:t4:tMC4:t14:tMC14:t15:tMC15");

  TF1 *fNuSig=new TF1("fNuSig","[0]*exp([1]-x)/[2]*cos([3]*x+[4])",0,16);
  //  TF1 *NuSig=new TF1("NuSig",DampExp,-256,256,5);
  TF1 *NuSig0=new TF1("NuSig0",DampExpPlusNoise,-512,512,5);
  
  Int_t nhitV, nhitH;

  TTree *TRUN=new TTree("TRUN","TRUN");
  TRUN->Branch("CHISQPERHITMAX",&CHISQPERHITMAX,"CHISQPERHITMAX");
  TRUN->Branch("VMAXRMSMAX",&VMAXRMSMAX,"VMAXRMSMAX");
  TRUN->Branch("VMAXAVGMIN",&VMAXAVGMIN,"VMAXAVGMIN");
  TRUN->Branch("FMAXSIGMIN",&FMAXSIGMIN,"FMAXSIGMIN");
  TRUN->Branch("xcvalMinMatchTemplate",&xcvalMinMatchTemplate,"xcvalMinMatchTemplate");
  TRUN->Branch("delTTMin",&delTTMin,"delTTMin");
  TRUN->Branch("RXTOTMAXPERDOF",&RXTOTMAXPERDOF,"RXTOTMAXPERDOF");
  TRUN->Branch("WfPowerMinNewTemplate",&WfPowerMinNewTemplate,"WfPowerMinNewTemplate");
  TRUN->Branch("NHITSMIN",&NHITSMIN,"NHITSMIN/I");
  TRUN->Branch("NSIGHIT",&NSIGHIT,"NSIGHIT");
  TRUN->Branch("TEvent",&TEvent,"TEvent/D");
  TRUN->Branch("TFirstEvent",&TFirstEvent,"TFirstEvent/D");
  TRUN->Branch("MPRESCALE",&MPRESCALE,"MPRESCALE/I");
  TRUN->Branch("NEvTot",&NEvTot,"NEvTot/I");
  TRUN->Branch("iEV",&iEV,"iEV/I");
  TRUN->Branch("nCalPulserMatch",&nCalPulserMatch,"nCalPulserMatch/I");
  TRUN->Branch("nCalPulserPass0",&nCalPulserPass0,"nCalPulserPass0/I");
  TRUN->Branch("nEvCalPulser",&nEvCalPulser,"nEvCalPulser/I");
  TRUN->Branch("NEvTemplate0",&NEvTemplate0,"NEvTemplate0/I");
  TRUN->Branch("SNRMAXFREQMAX",&SNRMAXFREQMAX,"SNRMAXFREQMAX");
  TRUN->Branch("XCORVHMAX",&XCORVHMAX,"XCORVHMAX");
  TRUN->Branch("MBINDIFFNSIGMAX",&MBINDIFFNSIGMAX,"MBINDIFFNSIGMAX");
  TRUN->Branch("NTrigs",&NTrigs,"NTrigs[3]/I");

  TTree *TVt=new TTree("TVt","TVt");
  TVt->Branch("VMaxVRms",&VMaxVRms,"VMaxVRms");
  TVt->Branch("VMaxHRms",&VMaxHRms,"VMaxHRms");
  TVt->Branch("VMaxVAvg",&VMaxVAvg,"VMaxVAvg");
  TVt->Branch("VMaxHAvg",&VMaxHAvg,"VMaxHAvg");
  TVt->Branch("xCorVV",&xCorVV,"xCorVV");
  TVt->Branch("xCorHH",&xCorHH,"xCorHH");
  TVt->Branch("xCorVH",&xCorVH,"xCorVV");
  TVt->Branch("grVT",&grVT,"grVT[8192]/F");
  TVt->Branch("itrigCodeZ",&itrigCodeZ,"itrigCodeZ/I");

  TTree *T=new TTree("T","T0");
  //  T->Branch("FitPoint",FitPoint,"FitPoint[4]/D");
  //  T->Branch("FitPerror",FitPerror,"FitPerror[4]/D");
  T->Branch("ZDot",&ZDot,"ZDot");
  T->Branch("fresidChi2Test",&fresidChi2Test,"fresidChi2Test");
  T->Branch("fKGTest",&fKGTest,"fKGTest");
  T->Branch("fChi2Test",&fChi2Test,"fChi2Test");
  T->Branch("WFPMaxPos",&WFPMaxPos,"WFPMaxPos");
  T->Branch("WFPMaxNeg",&WFPMaxNeg,"WFPMaxNeg");
  T->Branch("NuSigChi2",&NuSigChi2,"NuSigChi2");
  T->Branch("the",&the,"the");
  T->Branch("phi",&phi,"phi");
  T->Branch("r2vtx",&r2vtx,"r2vtx");
  T->Branch("ipkValIFGphiV",&ipkValIFGphiV,"ipkValIFGphiV/I");
  T->Branch("ipkValIFGphiH",&ipkValIFGphiH,"ipkValIFGphiH/I");
  T->Branch("ipkValIFGtheV",&ipkValIFGtheV,"ipkValIFGtheV/I");
  T->Branch("ipkValIFGtheH",&ipkValIFGtheH,"ipkValIFGtheH/I");
  T->Branch("fpkValIFGV",&fpkValIFGV,"fpkValIFGV");
  T->Branch("fpkValIFGH",&fpkValIFGH,"fpkValIFGH");
  T->Branch("iPkbinIFGphiV",&iPkbinIFGphiV,"iPkbinIFGphiV/I");
  T->Branch("iPkbinIFGphiH",&iPkbinIFGphiH,"iPkbinIFGphiH/I");
  T->Branch("iPkbinIFGtheV",&iPkbinIFGtheV,"iPkbinIFGtheV/I");
  T->Branch("iPkbinIFGtheH",&iPkbinIFGtheH,"iPkbinIFGtheH/I");
  T->Branch("fPkValIFGV",&fPkValIFGV,"fPkValIFGV");
  T->Branch("fPkValIFGH",&fPkValIFGH,"fPkValIFGH");
  T->Branch("nhits",&nhits,"nhits/I");
  T->Branch("iEvtNo",&iEvtNo,"iEvtNo/I");
  T->Branch("iRunNo",&iRunNo,"iRunNo/I");
  T->Branch("chisqvtx",&chisqvtx,"chisqvtx");
  T->Branch("ChiSqTot",&ChiSqTot,"ChiSqTot/D");
  T->Branch("Tsec",&Tsec,"Tsec/D");
  T->Branch("Tus",&Tus,"Tus/D");
  T->Branch("trighour",&trighour,"trighour/I");
  T->Branch("trigmin",&trigmin,"trigmin/I");
  T->Branch("trigsec",&trigsec,"trigsec/I");
  T->Branch("trigday",&trigday,"trigday/I");
  T->Branch("jTemplate",&jTemplate,"jTemplate/I");
  T->Branch("NCHgt400MHz",&NCHgt400MHz,"NCHgt400MHz/I");
  T->Branch("NbindelNSigMax",&NbindelNSigMax,"NbindelNSigMax/I");
  T->Branch("NX0",&NX0,"NX0/I");
  T->Branch("dTT",&dTT,"dTT");
  T->Branch("xcvalMax",&xcvalMax,"xcvalMax");
  T->Branch("WfPT",&WfPT,"WfPT");
  T->Branch("WfPTS",&WfPTS,"WfPTS");
  T->Branch("WfPThresh",&WfPThresh,"WfPThresh");
  T->Branch("dzFirstHit",&dzFirstHit,"dzFirstHit");

  T->Branch("RxTOT",&RxTOT,"RxTOT");
  T->Branch("RxTOT0",&RxTOT0,"RxTOT0");
  T->Branch("RxHitTime",&RxHitTime,"RxHitTime[16]");
  T->Branch("RxdTPos",&RxdTPos,"RxdTPos[16]");
  T->Branch("SNRRxHitTime",&SNRRxHitTime,"SNRRxHitTime[16]");
  T->Branch("SNRRxMaxVCh",&SNRRxMaxVCh,"SNRRxMaxVCh[16]");
  T->Branch("MaxFreqFFTSNRch",&MaxFreqFFTSNRch,"MaxFreqFFTSNRch[16]");
  T->Branch("MaxFreqFFT",&MaxFreqFFT,"MaxFreqFFT[16]");
  T->Branch("fFFTMax",&fFFTMax,"fFFTMax[16]");
  T->Branch("AFFTMax",&AFFTMax,"AFFTMax[16]");
  T->Branch("WfPowerc",&WfPowerc,"WfPowerc[16]");

  T->Branch("nPat",&nPat,"nPat/I");
  T->Branch("iTrig",&iTrig,"iTrig/I");

  T->Branch("grVT00",&grVT00,"grVT00/F");
  T->Branch("showerX",&showerX,"showerX");  
  T->Branch("showerY",&showerY,"showerY");  
  T->Branch("showerZ",&showerZ,"showerZ");
  T->Branch("showerE",&showerE,"showerE");
  T->Branch("nuE",&nuE,"nuE");


  Int_t it0Match;
  TTree *TX=new TTree("TX","it X jt");
  TX->Branch("it0",&it0,"it0/I");
  TX->Branch("jt0",&jt0,"jt0/I");
  TX->Branch("grXCorT",&grXCorT,"grXCorT");
  TX->Branch("grXCorA",&grXCorA,"grXCorA");
  TX->Branch("deltaTSqSum",&deltaTSqSum,"deltaTSqSum");
  TX->Branch("ndeltaTSqSum",&ndeltaTSqSum,"ndeltaTSqSum/I");
  TX->Branch("it0Match",&it0Match,"it0Match/I");

  f.cd();

  //  Int_t iGraphWrite=0;

  Double_t TtrigVTXC[MAXPAT], TtrigUsVTXC[MAXPAT];
  Float_t WfPowerTemplate[MAXPAT], WfPowerSurfTemplate[MAXPAT];
  Int_t NPtsTot=0;
  Double_t *tch[MCH];
  Double_t *Vch[MCH];

  Float_t ff[MBINFREQ];
  Float_t Af[MBINFREQ];
  TGraph *grFFTNotchTemp;
  TGraph *gTemp[2];
  TGraph *grEnvelope[MCH];
  TGraph *gVtxc[MCH];
  TGraph *gVtxc0[MCH];
  TGraph *gVtxc0Fit[MCH];
  TGraph *gVTXC[MCH][MAXPAT];  
  TGraph *gVTXCAvg[MCH][MAXPAT];  
  TGraph *grVTXC[MAXPAT];
  TGraph *grFFTVTXC[MAXPAT];
  TGraph *gFFTVTXC[MCH][MAXPAT];  
  TGraph *grUnbiased[MCH][MAXUNB];
  TGraph *grMCVtFFT[MCH];
  TGraph *grUnbMCVt[MCH];
  TGraph *grMCVt[MCH];
  TGraph *grdNdOmeg;
  TH1 *hFFT;
  TH1 *hVTFFT0;
  TH1 *hTF1NuSig;
  TH1 *hFFT;
  TH1F *hSig32[MCH];
  TH2F *hfreqFilter=new TH2F("hfreqFilter","hfreqFilter",128,0,1.28,MCH,-0.5,MCH-0.5);
  TH1F *hdelTRFtrigs=new TH1F("hdelTRFtrigs","hdelTRFtrigs",10000,0,100);
  TH1I *hfmod10=new TH1I("hfmod10","hfmod10",10,-0.5,9.5);
  TH1I *hdEvNo=new TH1I("hdEvNo","hdEvNo",16,-0.5,15.5);
  TH2F *hVMaxRms=new TH2F("hVMaxRms","hVMaxRms",50,0,5,50,0,5);
  TH2F *hVMaxAvg=new TH2F("hVMaxAvg","hVMaxAvg",50,0,10,50,0,10);  
  TH2F *hMaxVNHit=new TH2F("hMaxVNHit","hMaxVNHit",MCH+1,-0.5,16.5,MTHRESH,0,MTHRESH);
  TH2F *hfMaxSig=new TH2F("hfMaxSig","hfMaxSig|trigCodeZ",100,0,0.4,3,-0.5,2.5);
  TH1D *hDeadTime=new TH1D("hDeadTime","hDeadTime",10,-0.5,9.5);
  TH2F *hdelXcNSig=new TH2F("hdelXcNSig","hdelXcNSig",200,-100,100,3,-0.5,2.5);
  TH1F *hWFPAsym=new TH1F("hWFPAsym","hWFPAsym",200,-100000,100000);
  TH1F *hMCXDAChVal=new TH1F("hMCXDAChVal","hMCXDAChVal",512,0,2048);
  TH1F *hMCXDAChBin=new TH1F("hMCXDAChBin","hMCXDAChBin",512,0,1024);
  TH1F *hMCXDAChVal1=new TH1F("hMCXDAChVal1","hMCXDAChVal1",512,0,2048);
  TH1F *hMCXDAChBin1=new TH1F("hMCXDAChBin1","hMCXDAChBin1",512,0,1024);
  TH1F *hMCUnbVtXDA=new TH1F("hMCUnbVtXDA","hMCUnbVtXDA",512,0,8192*4);
  TH1F *hMCVtFFTXDA=new TH1F("hMCVtFFTXDA","hMCVtFFTXDA",512,0,8192*4);
  TH1F *hUnbXDAChVal=new TH1F("hUnbXDAChVal","hUnbXDAChVal",512,0,512);
  TH1F *hUnbXDAChBin=new TH1F("hUnbXDAChBin","hUnbXDAChBin",512,0,1024);
  TH1F *hUnbXDA=new TH1F("hUnbXDA","hUnbXDA",512,0,8192*4);
  TH1F *hTemplWfPowerThreshServo=new TH1F("WfPowerThreshServo","WfPowerThreshServo",MAXPAT,-0.5,MAXPAT-0.5);
  TH2I *hNlen=new TH2I("hNlen","hNlen",MAXPAT,-0.5,MAXPAT-0.5,MCH,-0.5,MCH-0.5);
  TH2I *hNEvCuts=new TH2I("hNEvCuts","hNEvCuts",M_NEVCUTS,-0.5,M_NEVCUTS-0.5,3,-0.5,2.5);
  TH2I *hNThresh=new TH2I("hNThresh","hNThresh",MTHRESH,-0.5,MTHRESH-0.5,MTHRESH,-0.5,MTHRESH-0.5);
  TH2F *ht0=new TH2F("ht0","ht0",MAXPAT,-0.5,MAXPAT-0.5,MCH,-0.5,MCH-0.5);
  TH2F *hdt=new TH2F("hdt","hdt",MAXPAT,-0.5,MAXPAT-0.5,MCH,-0.5,MCH-0.5);
  TH1F *hVMaxV1=new TH1F("VMaxV","VMaxV",128,0,25.6);
  TH1F *hVMaxH1=new TH1F("VMaxH","VMaxH",128,0,25.6);
  TH2F *hVMaxV=new TH2F("hVMaxV","VMaxV",128,0,12.8,3,-0.5,2.5);
  TH2F *hVMaxH=new TH2F("hVMaxH","VMaxH",128,0,12.8,3,-0.5,2.5);
  TH2F *hnhitV=new TH2F("hnhitV","nhitV",12,-0.5,11.5,3,-0.5,2.5);
  TH2F *hnhitH=new TH2F("hnhitH","nhitH",12,-0.5,11.5,3,-0.5,2.5);
  TH2F *hnhitVH=new TH2F("hnhitVH","nhitVH",12,-0.5,11.5,12,-0.5,11.5);
  TH1F *hdelTT=new TH1F("hdelTT","log10(delTT)",1000,-2,5);
  TH2F *hlenArr=new TH2F("hlenArr","hlenArr",1024,0,1024,1024,0,1024);
  TH2F *hxcvalSum=new TH2F("hxcvalSum","xcvalSum|dTT",100,0,50000,25,0,50);
  TH2F *hXCtrigCode=new TH2F("hXCtrigCode","hxcvalSum|trigCode",500,0,50000,3,-0.5,2.5);
  TH2F *hxcvalMax=new TH2F("hxcvalMax","hxcvalMax|trigCode",500,0,50000,3,-0.5,2.5);
  TH2F *hxcValSum=new TH2F("hxcValSum","hxcValSum|grXCorA",100,0,50000,50,0,5000);
  TH1F *hdeltaT=new TH1F("hdeltaT","hdeltaT",100,0,25);
  TH1F *hdeltaA=new TH1F("hdeltaA","hdeltaA",200,0,0.25);
  TH1F *hdeltaTxc=new TH1F("hdeltaTxc","hdeltaTxc",100,0,25);
  TH2F *hdeltaTCh=new TH2F("hdeltaTCh","hdeltaT|Ch",200,0,200,16,-0.5,15.5);
  TH1F *hTemplateMatch=new TH1F("hTemplateMatch","hTemplateMatch",100,-0.5,99.5);
  TH2F *hdT2d=new TH2F("hdT2d","hdT2d",2000,0,2000,1000,-0.5,999.5);
  TH1F *hgrCorTp1Tp2=new TH1F("hgrCorTp1Tp2","hgrCorTp1Tp2",2000,-500,500);
  TH1F *hdT1d0=new TH1F("hdT1d0","hdT1d0",2000,-1000,1000); //values of XCor time shifts
  TH2F *hdTTemplates=new TH2F("hdTTemplates","hdTTemplates",MCH,-0.5,MCH-1,MCH,-0.5,MCH-1);
  TH1F *hdT1d=new TH1F("hdT1d","hdT1d",2000,0,2000); //values of XCor time shifts
  TH1F *hdT1d_rms=new TH1F("hdT1d_rms","hdT1d_rms",500,0,500);
  TH1F *hWfPowerTemplates=new TH1F("hWfPowerTemplates","hWfPowerTemplates",600,6.4,8.2);
  TH2F *hWfPower=new TH2F("hWfPower","log10(hWfPower)|trigCode",600,6.4,8.2,3,-0.5,2.5);
  TH1F *hWfPowerTF1MC=new TH1F("hWfPowerTF1MC","log10(hWfPower)",600,6.4,8.2);
  TH1F *hWfPowerMC0=new TH1F("hWfPowerMC0","log10(hWfPower1)",600,6.4,8.2);
  TH1F *hWfPowerUnbMC=new TH1F("hWfPowerUnbMC","log10(hWfPowerUnbMC)",600,6.4,8.2);
  TH1F *hWfPowerUnb0=new TH1F("hWfPowerUnb0","log10(hWfPowerUnb0)",600,6.4,8.2);
  TH1F *hVTXC[MAXPAT];
  TH1F *hFFTVTXC[MAXPAT];
  TH1F *hVtxc[MAXCALPULSERTEMPLATES];
  for(Int_t ic=0; ic<MAXCALPULSERTEMPLATES; ic++){
    sprintf(hName,"hVtxc%d",ic);
    hVtxc[ic]=new TH1F(hName,hName,MBINS,0,MBINS);
  }
  Int_t ifreq;
  for(ifreq=0; ifreq<MBINFREQ; ifreq++){
    ff[ifreq]=float(ifreq)/MBINFREQ;
    Af[ifreq]=0;
  }
  
  //  TGraph *gVt;

  //  string hitsFilestr=string("/home/dbesson/ctem/hitsFile_")+datestamp+string(".txt");
  //  ofstream hitsOut(hitsFilestr.c_str());

  if(AraRootv<=2)
    RawAraEvent *eventPtr=0; //should work on processed files as of 31jan11
  else
    RawIcrrStationEvent *eventPtr=0; //should work on processed files as of 31jan11
  UsefulIcrrStationEvent* calEvPtr=0;
  AraGeomTool *araGeom=AraGeomTool::Instance();
  AraAntennaInfo antennaInfo;
  AraAntennaInfo::Instance();
  AraEventCorrelator *theCorrelator= AraEventCorrelator::Instance();
  //  AraEventCorrelator::fDebugMode(1);
  AraCorrelatorType::EAraCorrelatorType corType;
  theCorrelator->setupDeltaTInfinity();
  corType=AraCorrelatorType::kPlaneWave;

  eventrecoMinuit->SetPrintLevel(0); //"quiet"
  eventrecoMinuit->SetMaxIterations(100);
  eventrecoMinuit->mnexcm("SET ERR", arglist, 1, ierflg);	// Parameter UP for both SIMPLEX and MIGRAD is set up here to 200

  Int_t nVPolCh, nHPolCh, nSurfCh;

  Int_t iCOGz=0;
  for(Int_t ich=0; ich<MCH; ich++){
    araGeom->fStationInfo[station].fAntInfo[ich].printAntennaInfo();

    Rx[ich].x=araGeom->fStationInfo[station].fAntInfo[ich].antLocation[0];
    Rx[ich].y=araGeom->fStationInfo[station].fAntInfo[ich].antLocation[1];
    Rx[ich].z=araGeom->fStationInfo[station].fAntInfo[ich].antLocation[2];
    if(Rx[ich].z<-10){
      RxCOGz+=Rx[ich].z;
      iCOGz++;
    }
    Rx[ich].iPol=-1;
    if(araGeom->fStationInfo[station].fAntInfo[ich].polType==AraAntPol::kVertical){
      Rx[ich].iPol=0;
      nVPolCh++;
    }
    if(araGeom->fStationInfo[station].fAntInfo[ich].polType==AraAntPol::kHorizontal){
      Rx[ich].iPol=1;
      nHPolCh++;
    }
    if(araGeom->fStationInfo[station].fAntInfo[ich].polType==AraAntPol::kSurface){
      Rx[ich].iPol=2;
      nSurfCh++;
    }
    sprintf(chname,"hSig32%d",ich);
    hSig32[ich]=new TH1F(chname,chname,MBINSIG,-0.5,MBINSIG-0.5);
  }
  RxCOGz/=iCOGz; 
  cout<<"NVPol/NHPol/NSurf="<<nVPolCh<<"/"<<nHPolCh<<"/"<<nSurfCh<<endl<<"RxCOGz="<<RxCOGz<<endl;

  if(iEV>=0){
    //http://ara.physics.wisc.edu/docs/0006/000611/001/ARA_%204th%20presentation%282%29.pdf
    Rx[0].dtNsCorr=15.5686;    Rx[1].dtNsCorr=11.459;    Rx[2].dtNsCorr=2.77805;    Rx[3].dtNsCorr=1.68749;    Rx[4].dtNsCorr=2.1316;    Rx[5].dtNsCorr=-7.8333;    Rx[6].dtNsCorr=3.84821;    Rx[7].dtNsCorr=0;    Rx[8].dtNsCorr=0.386255;    Rx[9].dtNsCorr=-16.2755;    Rx[10].dtNsCorr=-0.820987;    Rx[11].dtNsCorr=-12.4605;    Rx[12].dtNsCorr=18.1285;    Rx[13].dtNsCorr=0;    Rx[14].dtNsCorr=0;    Rx[15].dtNsCorr=0;
  }
  /*****  
  sprintf(cFileIn,"/home/dbesson/lists/%s.list",cListFileIn);
  ifstream InFileList;
  if(AraRootv<=2)
    InFileList.open("/home/dbesson/lists/0130_windy.list"); //good
  else
    InFileList.open(cFileIn);

  string path;
  TFile *fp;
  stringstream str;
  ******/
  memset(RxHitTimeTempl,0,sizeof(RxHitTimeTempl));
  memset(xcHitTimeTempl,0,sizeof(xcHitTimeTempl));
  //  memset(NEvCuts,0,sizeof(NEvCuts));

  Bool_t LExit=false;

  //MAIN EVENT LOOP!
  /*******
  while(!InFileList.eof() && (NEvTot<MAXEVT || MAXEVT<0) && (itemplate<MAXPAT) && !LExit ){
    InFileList>>path;     
    if(InFileList.eof()||!InFileList.good())
      break;
    str=path;
    filename = str.str();
    if(fp)
      delete fp;
    fp=new TFile(filename.c_str());  //Loading calEventFile                 
    int nchar=path.find("-Run");
  *****/
    string runString=path.substr(nchar+sizeof("-Run"),6);
    //   cout<<"Opening new file: "<<filename.c_str()<<" for run "<<atof(runString.c_str())<<endl;
 
  TFile *fp;
  if(fp) delete fp;
  fp = new TFile(inFileName);
  cout <<"Opening new file: "<< inFileName << "for run" << atof(runString.c_str())<<endl;
  fp.cd();
  while(!LExit && (NEvTot<MAXEVT || MAXEVT<0) && (itemplate<MAXPAT) ){
    Float_t hktemps[8];
    TTree *eventTree = (TTree*)fp->Get("eventTree");
    eventTree->SetBranchAddress("event",&eventPtr);
    eventTree->SetBranchAddress("run",&runNumber);
    eventTree->SetBranchAddress("head.eventNumber",&headerIevt);
    eventTree->SetBranchAddress("calevent",&calEvPtr);
    //    eventTree->SetBranchAddress("hk.temp",&hktemps);
    cout<<eventPtr->hk.temp[0]<<endl;
    Bool_t LLoop=true;
    //    NSigmaCut=4.5;
    //main event loop:
    for(int ievt=0; LLoop; ievt++){
      LLoop=(ievt<MAXEVT-1 || MAXEVT<0) && ievt<eventTree->GetEntries()-1 && (NEvTot<MAXEVT-1 || MAXEVT<0) && (itemplate<MAXPAT);

      hdT1d->Reset();
      hVMaxV1->Reset();
      hVMaxH1->Reset();
      Bool_t LPrintOut=fmod(NEvTot,MPRINTEV)==0;
      //      headerIevt0=headerIevt;
      eventTree->GetEntry(ievt);

      //      if(AraRootv<=2)UsefulAraEvent *usefulEvent=new UsefulAraEvent(eventPtr,AraCalType::kLatestCalib);
      if(AraRootv>2)UsefulIcrrStationEvent usefulEvent(eventPtr,AraCalType::kLatestCalib);
      iEvNoOld=iEvNoThis;
      iEvNoThis=calEvPtr->head.eventNumber;
      hdEvNo->Fill(iEvNoThis-iEvNoOld);
      TEvent=eventPtr->head.unixTime+micro*eventPtr->head.unixTimeUs;
      if(TFirstEvent==0){
	TFirstEvent=TEvent;
	TEvent0=TEvent;
      }
      if(TEvent-TEvent0>NSECLIVECALC){
	fLiveInst=float(nEvCalPulser0)/NSECLIVECALC;
	TEvent0=TEvent;
	nEvCalPulser0=0;
      }
      LCalPulser=(usefulEvent.isCalPulserEvent()==1 || eventPtr->head.unixTimeUs<TusCPMAX) && iEV>=0;
      LUnBiased=int(usefulEvent.trig.trigType)==68;
      itrigCodeZ=0;
      NEvTot0++;
      if(LCalPulser){
	nEvCalPulser++;
	nEvCalPulser0++;
	itrigCodeZ=1;
      }
      if(LUnBiased)
	itrigCodeZ=2;
      if(itrigCodeZ==0){
	TEventRF0=TEventRF;
	TEventRF=TEvent;
	hdelTRFtrigs->Fill(TEventRF-TEventRF0);
	TdT->Fill(TEventRF-TEventRF0,TEventRF);	  
      }
      NTrigs[itrigCodeZ]++;
      iprintx=0;
      iCutsLevel=-1;
      //      NEvCuts[iCutsLevel][itrigCodeZ]++;
      WfPowerSurf=0;
      WfPower=0;
      NChHitTrigger=0;
      iEvFailCode2=0;
      XCorSum=0;
      SkewSum=0;
      KurtSum=0;
      NComb32=0;
      NEvCutsFail=0;
      memset(NThresh,0,sizeof(NThresh));
      memset(RxHitTime,0,sizeof(RxHitTime));
      memset(RxdTPos,0,sizeof(RxdTPos));
      memset(fFFTMax,0,sizeof(fFFTMax));
      memset(AFFTMax,0,sizeof(AFFTMax));
      RxTOT=0;
      Float_t VSqSum0=0;
      VMaxVRms=0;
      VMaxHRms=0;
      memset(VMax,0,sizeof(VMax));
      memset(WfPowerc,0,sizeof(WfPowerc));
      NPtsTot=0;
      fdelTSqSum=999;
      igoodX=0;
      SNRMaxFreq=0;
      fSNRMaxFreq=0;
      Int_t NbinCalPulser=0;
      Float_t TNSig=0;
      Float_t TMaxV=0;
      nhitV=0;
      nhitH=0;
      WfPSig=0;
      SNRRxMaxVAvg=0;
      NbinDiffNSigMax[itemplate]=0;
      WfPowerMaxNeg[itemplate]=0;
      WfPowerMaxPos[itemplate]=0;
      NuSigChisquare[itemplate]=0;
      fKGTest32[itemplate]=0;
      fChi2Test32[itemplate]=0;
      fresidChi2_32[itemplate]=0;
      ZDot32[itemplate]=0;
      //      WfPowerCalPulserEv=0;
      Int_t igrVT=0;
      nhits=0;
      nhitsH=0;
      nhitsV=0;
      nhitsS=0;
      NX0=0;
      xCorVV=0;
      xCorHH=0;
      xCorVH=0;
      Int_t ichFirstHit=-1;
      Float_t T0event=9999;
      Int_t ibinNSig=0;
      Int_t ibinNSigLast=0; 
      Int_t ibinMax=0;
      Int_t NChPassMaxSig=0;
      LTemplate0=true;

      Int_t krx, mpts, iMCUnbEv, iMCUnbEvNo;
      Float_t aVtMC, aVMC;
      deadTime1_tot+=calEvPtr->trig.deadTime1;
      deadTime2_tot+=calEvPtr->trig.deadTime2;

      hfmod10->Fill(fmod(calEvPtr->head.eventNumber,MPRESCALE));
      Bool_t LAna=iEV!=0 || (iEV==0 && fmod(calEvPtr->head.eventNumber,MPRESCALE)==0);
      if(LCalPulser&&iCP==0&&nEvCalPulser>=MAXANACALPULSER)
	LAna=false;
      if(iCP==1&&!LCalPulser)
	LAna=false;
      if(iUB==1&&!LUnBiased)
	LAna=false;
      if(LAna){
	for(Int_t ich=0; ich<MCH; ich++){
	  //	  LFitRxCh[ich]=false;
	  WfPowerCh=0;
	  LUseCh[ich]=0;
	  LUseChT[ich][itemplate]=0;
	  Rx[ich].HitTime=RxHitTime00;
	  SNRRxHitTime[ich]=-1;
	  
	  if(iEV>=0){
	    NPtsTot+=usefulEvent.fNumPointsRF[ich];
	    gVtxc0[ich]=usefulEvent.getGraphFromRFChan(ich);
	  }
	  
	  if(LUnBiased&&NEvUnbias<MAXUNB-1 && iEV>=0)
	    grUnbiased[ich][NEvUnbias]=usefulEvent.getGraphFromRFChan(ich);
	  Double_t *VVtxc0=gVtxc0[ich]->GetY();
	  Double_t *tVVtxc0=gVtxc0[ich]->GetX();
	  Int_t Nbins=gVtxc0[ich]->GetN();
	  dtVT[ich]=tVVtxc0[1]-tVVtxc0[0];
	  t0VT[ich]=tVVtxc0[0];
	  if(dtVT[ich]<0)printf("dtVT[%d]<0!\n",ich);
	  if(LPrintx)printx("ch");
	  for(Int_t ib=1; ib<Nbins; ib++){
	    //	    if(!gVtxc0->CompareX(gVtxc0,ib-1,ib))cout<<"BINS NOT DESCENDING!"<<endl;
	    if(tVVtxc0[ib]<tVVtxc0[ib-1]){
	      printf("tVVtxc0[%d]<tVVtxc0[%d]\n",ib,ib-1);
	      tVVtxc0[ib]=tVVtxc0[ib-1]+dtVT[ich];
	      gVtxc0[ich]->SetPoint(ib,tVVtxc0[ib],VVtxc0[ib]);
	    }
	  }

	  TGraph *grFFTtemp=usefulEvent.getFFTForRFChan(ich);
	  MaxFreqFFTSNR[ich][itemplate]=grFFTtemp->GetHistogram()->GetMaximum()/grFFTtemp->GetRMS(2);
	  MaxFreqFFT[ich][itemplate]=grFFTtemp->GetHistogram()->GetMaximumBin()/grFFTtemp->GetRMS(2);
	  Float_t delfFFT=grFFTtemp->GetHistogram()->GetBinWidth(2);
	  if(MaxFreqFFTSNR[ich][itemplate]>SNRMaxFreq){
	    SNRMaxFreq=MaxFreqFFTSNR[ich][itemplate];
	    fSNRMaxFreq=MaxFreqFFT[ich][itemplate];
	    if(SNRMaxFreq>SNRMINFREQFILTER){
	      grFFTNotchTemp=FFTtools::simpleNotchFilter(gVtxc0[ich],fSNRMaxFreq-delfFFT,fSNRMaxFreq+delfFFT);
	      delete gVtxc0[ich];
	      gVtxc0[ich]=grFFTNotchTemp;
	      hfreqFilter->Fill(fSNRMaxFreq,ich);
	      //	      delete grFFTNotchTemp;
	    }
	  }
	  delete grFFTtemp;
	  
	  gVtxc0Fit[ich]=(TGraph*)gVtxc0[ich]->Clone();
	  if(false)grEnvelope[ich]=FFTtools::getSimplePowerEnvelopeGraph(gVtxc0[ich]);
	  if(false)grEnvelope[ich]=FFTtools::getHilbertEnvelope(gVtxc0[ich]);
	  /*	  c3->cd();
	  grEnvelope[ich]->Draw("AL");
	  sprintf(cName,"/home/dbesson/ntem/tem/FitCh%d_ev%d.png",ich,ievt);
	  c3->SaveAs(cName); */
	  Float_t VSqSum=FFTtools::sumVoltageSquared(gVtxc0[ich],0,gVtxc0[ich]->GetN());
	  VSqSum0+=VSqSum;
	  
	  TH1F *habsVt=new TH1F("habsVt","habsVt",gVtxc0[ich]->GetN(),0,gVtxc0[ich]->GetN());
	  Int_t ibinVposMax=0;
	  Float_t VposMax=0;
	  for(Int_t ib=0; ib<gVtxc0[ich]->GetN(); ib++){
	    grVT[igrVT]=VVtxc0[ib];
	    igrVT++;
	    if(fabs(VVtxc0[ib])>VMax[ich])
	      VMax[ich]=fabs(VVtxc0[ib]);
	    if(VVtxc0[ib]>VposMax){
	      VposMax=VVtxc0[ib];
	      ibinVposMax=ib;
	    }	      
	    
	    WfPowerCh+=VVtxc0[ib]*VVtxc0[ib];
	    WfPower+=VVtxc0[ib]*VVtxc0[ib];
	    habsVt->Fill(ib,fabs(VVtxc0[ib]));
	  }
	  
	  //	habsVt->Print("all");
	  ibinNSig=habsVt->FindFirstBinAbove(Rx[ich].Vrms*NSigmaCut);
	  ibinNSigLast=habsVt->FindLastBinAbove(Rx[ich].Vrms*NSigmaCut);
	  ibinMax=habsVt->GetMaximumBin();
	  fMaxSig[ich]=habsVt->Integral(ibinMax-3,ibinMax+3)/habsVt->Integral();
	  SNRRxMaxV[ich][itemplate]=habsVt->GetMaximum()/Rx[ich].Vrms;
	  if(SNRRxMaxV[ich][itemplate]>NSIGHIT)
	    NChHitTrigger++;
	  for(Int_t ithresh=0; ithresh<MTHRESH; ithresh++)
	    if(SNRRxMaxV[ich][itemplate]>ithresh)
	      NThresh[ithresh]++;
	  //		NThresh[ithresh][itrigCodeZ]++;
	  hfMaxSig->Fill(fMaxSig[ich],itrigCodeZ);
	  if(fMaxSig[ich]>FMAXSIGMIN)NChPassMaxSig++;
	  if(VMax[ich]==0)printf("VMax[%d]=0!",ich);
	  Float_t ftemp=0;
	  if(ibinNSig>=0 && VMax[ich]>0){
	    SNRRxMaxVAvg+=SNRRxMaxV[ich][itemplate];
	    Int_t imin32=iMAX(0,ibinNSig-MBINSIG/4);
	    for(int ib32=imin32; ib32<iMIN(ibinNSig+3*MBINSIG/4+2,habsVt->GetNbinsX()); ib32++){
	      //	      cout<<"ib32="<<ib32<<" "<<habsVt->GetBinContent(ib32)<<endl;
	      //	      Float_t yval;
	      //	      yval=MILLION*habsVt->GetBinContent(ib32)/VMax[ich];
	      //	      if(yval<=1){
	      //		printf("Ch %d/imin32=%d - habsVt[%d]=%g\n",ich,imin32,ib32,yval);
	      //		yval=0.5*(habsVt->GetBinContent(ib32-1)+habsVt->GetBinContent(ib32+1))/(2*VMax[ich]);
	      //	      }
	      hSig32[ich]->SetBinContent(ib32-imin32,1000*habsVt->GetBinContent(ib32)/VMax[ich]);
	      WfPSig+=habsVt->GetBinContent(ib32)*habsVt->GetBinContent(ib32);
	    }
	    SkewSum+=hSig32[ich]->GetSkewness();
	    KurtSum+=hSig32[ich]->GetKurtosis();
	    NbinDiffNSigMax[itemplate]+=ibinMax-ibinNSig;
	    WfPowerMaxNeg[itemplate]+=habsVt->Integral(ibinNSig-MBINSIG/2-IBINOFFSET,ibinNSig-IBINOFFSET);
	    WfPowerMaxPos[itemplate]+=habsVt->Integral(ibinNSig-1,ibinMax+MBINSIG/2-1);
	    LUseCh[ich]=true;
	    LUseChT[ich][itemplate]=true;
	    Rx[ich].HitTime=tVVtxc0[ibinNSig];        
	    //	  Rx[ich].HitTime=Rx[ich].tMC;
	    SNRRxHitTime[ich]=VVtxc0[ibinNSig]/Rx[ich].Vrms;
	    if(Rx[ich].HitTime<T0event){
	      T0event=Rx[ich].HitTime;
	      ichFirstHit=ich;
	      zFirstHit[itemplate]=Rx[ich].z;
	    }
	    RxTOT+=ibinNSigLast-ibinNSig;
	    nhits++;
	    if(Rx[ich].iPol==0)nhitsV++;
	    if(Rx[ich].iPol==1)nhitsH++;
	    if(Rx[ich].iPol==2)nhitsS++;
	  }
	  delete habsVt;

	  if(ich>=14)
	    WfPowerSurf+=VSqSum;
	    //Use static Bool_tCompareX(const TGraph* gr, Int_t left, Int_t right) to make sure TGraph entries ascending
	  
	  gVtxc[ich]=FFTtools::getInterpolatedGraph(gVtxc0[ich],fInterpScale);
//gVtxc=interpolated gVtxc0
	  
	  //ZZZ
	  //	gVtxc0[ich]->Print("all");
	  //	gVtxc[ich]->Print("all");
	  
	  if(LUnBiased)
	    Vrms68[ich]+=gVtxc0[ich]->GetRMS(2);
	  if(LCalPulser){
	    Double_t *VCalPulser=gVtxc0[ich]->GetY();
	    if(iTemplCalPulser<MAXCALPULSERTEMPLATES)
	      for(Int_t ib=0; ib<gVtxc0[ich]->GetN(); ib++)
		hVtxc[iTemplCalPulser]->Fill(ib+NbinCalPulser,VCalPulser[ib]);
	    NbinCalPulser+=gVtxc0[ich]->GetN();
	    
	    /*
	      sprintf(gName,"gVtxc0_ch%d",ich);
	      gVtxc0[ich]->SetName(gName);
	      gVtxc0[ich]->SetTitle(gName);
	      gDirectory->Append(gVtxc0[ich]);
	      //	  gDirectory->ls();
	      */
	  }
	  if(Rx[ich].iPol==0){
	    hVMaxV->Fill(VMax[ich]/Rx[ich].Vrms,itrigCodeZ);
	    hVMaxV1->Fill(VMax[ich]/Rx[ich].Vrms);
	    if(VMax[ich]>NSigmaCut*Rx[ich].Vrms)
	      nhitV++;
	  }
	  if(Rx[ich].iPol==1){
	    hVMaxH->Fill(VMax[ich]/Rx[ich].Vrms,itrigCodeZ);
	    hVMaxH1->Fill(VMax[ich]/Rx[ich].Vrms);
	    if(VMax[ich]>NSigmaCut*Rx[ich].Vrms)
	      nhitH++;
	  }
	  if(nhits>=NHITSMIN)
	    hWFPAsym->Fill((WfPowerMaxPos[itemplate]-WfPowerMaxNeg[itemplate])/nhits);
	} //end MCH 

	for(Int_t ithresh=0; ithresh<MTHRESH && itrigCodeZ==0; ithresh++)
	  hNThresh->Fill(ithresh,NThresh[ithresh]);
	//Now XPol all hPol hSig32 and Vpol hSig32

	//	Double_t *residChi2;

	for(Int_t ich=0; ich<MCH; ich++){
	  for(Int_t jch=ich+1; jch<MCH; jch++){
	    if(LUseCh[ich]&&LUseCh[jch]){
	      if(Rx[ich].iPol==Rx[jch].iPol){
		NComb32++;
		//		if(LChi2Test){
		hDot=(TH1F*)hSig32[ich]->Clone();
		hDot->Multiply(hSig32[jch]);
		ZDot32[itemplate]+=hDot->Integral();
		fKGTest32[itemplate]+=hSig32[ich]->KolmogorovTest(hSig32[jch],"M");
		Chi2TestOut=hSig32[ich]->Chi2Test(hSig32[jch],"CHI2/NDF",residChi2); //,"PWW",&residChi2);
		hSig32[ich]->Chi2TestX(hSig32[jch],Chi2X,ndfX,igoodX,"CHI2/NDF",residChi2); //,"PWW",&residChi2);
		//		cout<<"Chi2X/Chi2TestOut/aChi2="<<Chi2X<<"/"<<Chi2TestOut<<"/"<<aChi2TestOut<<endl;
		//		cout<<"igoodX="<<igoodX<<endl;
		//		if(igoodX==0)
		fChi2Test32[itemplate]+=Chi2X;
		//		if(Chi2TestOut==0){		  hSig32[ich]->Print("all");		  hSig32[jch]->Print("all");}
		for(Int_t iresid=0; iresid<MBINSIG; iresid++)
		  fresidChi2_32[itemplate]+=residChi2[iresid]*residChi2[iresid];
		delete hDot;
		//		}
	      }
	    }
	  }
	} //end ich
	if(NComb32>0){
	  fKGTest32[itemplate]/=NComb32;
	  fresidChi2_32[itemplate]/=NComb32;
	  fChi2Test32[itemplate]/=NComb32;
	}

	c1->Clear();
	c1->Divide(2,nhits/2);
	//	cout<<"nhits="<<nhits<<endl;
	Int_t ic1panel=0;
	for(Int_t ich=0; ich<MCH; ich++){
	  if(LUseCh[ich]){
	    c1->cd(ic1panel+1);
	    //	    TGraph *gVtxc0_temp=(TGraph*)gVtxc0[ich]->Clone();
	    //	    gVtxc0_temp->GetXaxis()->SetRangeUser(
	    gVtxc0Fit[ich]->Draw("AL");
	    //	    sprintf(cName,"/home/dbesson/ntem/tem/FitCh%d_ev%d.png",ich,ievt);
	    //	    c1->SaveAs(cName);

	    ic1panel++;
	    RxHitTime0[ich][itemplate]=Rx[ich].HitTime-T0event;
	    for(Int_t jch=ich-1; jch>=0; jch--){
	      if(LUseCh[jch]){
		RxHitTimeTempl[jch][ich][itemplate]=Rx[ich].HitTime-Rx[jch].HitTime;
		hdeltaTCh->Fill(fabs(RxHitTimeTempl[jch][ich][itemplate]),ich);
		hdeltaTCh->Fill(fabs(RxHitTimeTempl[jch][ich][itemplate]),jch);
		
		TGraph *grCor_ij=FFTtools::getCorrelationGraph(gVtxc[ich],gVtxc[jch]);
		Int_t peakBin_ij=FFTtools::getPeakBin(grCor_ij);
		Float_t peakVal_ij=FFTtools::getPeakVal(grCor_ij);
		if(Rx[ich].iPol==Rx[jch].iPol)
		  XCorSum+=peakVal_ij;
		Double_t *dtVals_ij=grCor_ij->GetX();
		Double_t *xcVals_ij=grCor_ij->GetY();
		Float_t dtWtAvg=(dtVals_ij[peakBin_ij]*xcVals_ij[peakBin_ij]+dtVals_ij[peakBin_ij-1]*xcVals_ij[peakBin_ij-1]+dtVals_ij[peakBin_ij+1]*xcVals_ij[peakBin_ij+1])/(xcVals_ij[peakBin_ij]+xcVals_ij[peakBin_ij-1]+xcVals_ij[peakBin_ij+1]);
		xcHitTimeTempl[jch][ich][itemplate]=dtWtAvg; 
		xcHitTimeTempl[jch][ich][itemplate]=dtVals_ij[peakBin_ij];

		hdelXcNSig->Fill(xcHitTimeTempl[jch][ich][itemplate]-RxHitTimeTempl[jch][ich][itemplate],itrigCodeZ);
		delete grCor_ij;
		
	      }
	    }
	  }
	} //end ich loop

	for(Int_t ich=0; ich<MCH; ich++){
	  delete gVtxc0[ich];
	  if(grEnvelope[ich])	  delete grEnvelope[ich];
	}

	NHITS->Fill(Rx[0].HitTime,Rx[0].tMC,Rx[1].HitTime,Rx[1].tMC,
		    Rx[2].HitTime,Rx[2].tMC,Rx[3].HitTime,Rx[3].tMC,
		    Rx[4].HitTime,Rx[4].tMC,Rx[14].HitTime,Rx[14].tMC,Rx[15].HitTime,Rx[15].tMC);
	nhits0[itemplate]=nhits;
	//      cout<<"WfPower="<<WfPower<<" RxTOT="<<RxTOT<<endl;
	VMaxVRms=hVMaxV1->GetRMS();
	VMaxHRms=hVMaxH1->GetRMS();
	VMaxVAvg=hVMaxV1->GetMean();
	VMaxHAvg=hVMaxH1->GetMean();
	
	IncrementCuts(true);
	IncrementCuts(nhits>=NHITSMIN);
	IncrementCuts(WfPower>WfPowerMinNewTemplate);	
	IncrementCuts(NComb32>=8);
	IncrementCuts(WfPowerMaxPos[itemplate]>RWFPMIN*WfPowerMaxNeg[itemplate]);
	IncrementCuts(RxTOT<nhits*RXTOTMAXPERDOF); 
	IncrementCuts(SNRMaxFreq<SNRMAXFREQMAX);   
	IncrementCuts(NbinDiffNSigMax[itemplate]<nhits*MBINDIFFNSIGMAX);
	IncrementCuts((nhitsV>1&&VMaxVRms<VMAXRMSMAX)||(nhitsH>1&&VMaxHRms<VMAXRMSMAX));	
	LTemplate0=LTemplate0 || iTM==1;
	
	if(LUnBiased && NEvUnbias<MAXUNB-1 && iEV>=0){
	  NEvUnbias++;
	  //	  if(NEvUnbias<MAXUNB-1)
	  iUnbiasEvNo[NEvUnbias]=calEvPtr->head.eventNumber;
	}
	hnhitV->Fill(nhitV,itrigCodeZ);
	hnhitH->Fill(nhitH,itrigCodeZ);
	hnhitVH->Fill(nhitV,nhitH);
	
	if(fabs(WfPower/VSqSum0-1)>0.01)
	  cout<<"itrigCodeZ="<<itrigCodeZ<<" WfPower="<<WfPower<<" WfPower/VSqSum0="<<WfPower<<"/"<<VSqSum0<<endl;
	hWfPower->Fill(log10(WfPower),itrigCodeZ);
	
	//was here....

	if(LTemplate0){
	  TH2D *histMapV=theCorrelator->getInterferometricMap(&usefulEvent,AraAntPol::kVertical);
	  sprintf(hName,"vmap%d",ievt);
	  histMapV->SetName(hName);
	  TH2D *histMapH=theCorrelator->getInterferometricMap(&usefulEvent,AraAntPol::kHorizontal);
	  sprintf(hName,"hmap%d",ievt);
	  histMapH->SetName(hName);
	  
	  Int_t ilocxMax, ilocyMax, iloczMax;
	  fpkValMapV[itemplate]=-1;
	  fpkValMapH[itemplate]=-1;

	  for (int i=0;i<histMapV->GetNbinsX();i++){
	    for (int j=0;j<histMapV->GetNbinsY();j++){
	      if(histMapV->GetBinContent(i,j)>fpkValMapV[itemplate]){
		fpkValMapV[itemplate]=histMapV->GetBinContent(i,j);
		ipkValTheV[itemplate]=histMapV->GetYaxis()->GetBinCenter(j);
		ipkValPhiV[itemplate]=histMapV->GetXaxis()->GetBinCenter(i);
	      }
	      if(histMapH->GetBinContent(i,j)>fpkValMapH[itemplate]){
		fpkValMapH[itemplate]=histMapH->GetBinContent(i,j);
		ipkValTheH[itemplate]=histMapH->GetYaxis()->GetBinCenter(j);
		ipkValPhiH[itemplate]=histMapH->GetXaxis()->GetBinCenter(i);
	      }
	    }
	  }
	  
	  fPkValMapV[itemplate]=histMapV->GetMaximum();
	  fPkValMapH[itemplate]=histMapH->GetMaximum();
	  histMapH->GetMaximumBin(ilocxMax,ilocyMax,iloczMax);
	  iPkbinPhiH[itemplate]=ilocxMax;
	  iPkbinTheH[itemplate]=ilocyMax;

	  histMapV->GetMaximumBin(ilocxMax,ilocyMax,iloczMax);
	  iPkbinPhiV[itemplate]=ilocxMax;
	  iPkbinTheV[itemplate]=ilocyMax;

	  if(ipkValPhiV[itemplate]!=ilocxMax)
	    printf("ipkValPhiV=%d != ilocxMax=%d\n",ipkValPhiV[itemplate],ilocxMax);
	  
	  delete histMapV;
	  delete histMapH;
	  
	  for(Int_t iRx=0; iRx<MCH; iRx++){
	    for(Int_t jRx=iRx+1; jRx<MCH; jRx++){
	      TGraph *gRx1Rx2=FFTtools::getCorrelationGraph(gVtxc[iRx],gVtxc[jRx]);
	      xcMax=FFTtools::getPeakVal(gRx1Rx2);
	      if(Rx[iRx].iPol==0&&Rx[jRx].iPol==0)
		xCorVV+=xcMax;
	      if(Rx[iRx].iPol==1&&Rx[jRx].iPol==1)
		xCorHH+=xcMax;
	      if((Rx[iRx].iPol==1&&Rx[jRx].iPol==0)||
		 (Rx[iRx].iPol==0&&Rx[jRx].iPol==1))
		xCorVH+=xcMax;
	      delete gRx1Rx2;
	    }
	  }
	}
	delTT=99999;
	if(LTemplate0 && !LCalPulser){
	  TTemplate1=TTemplate0;
	  TTemplate0=TEvent; //eventPtr->head.unixTime+micro*eventPtr->head.unixTimeUs;
	  delTT=TTemplate0-TTemplate1;
	  if(LDebug)
	    cout<<setprecision(14)<<"TTemplate0/TTemplate1/delTT="<<TTemplate0<<"/"<<TTemplate1<<"/"<<delTT<<endl;
	  if(delTT>0)
	    hdelTT->Fill(log10(delTT));
	}
	IncrementCuts(iEV<0||delTT>delTTMin); 
	IncrementCuts(xCorVV>XCORXXMIN||xCorHH>XCORXXMIN);	
	IncrementCuts(xCorVH<XCORVHMAX*(xCorVV+xCorHH));
	LTemplate0=LTemplate0 || iTM==1;
	if(LTemplate0){
	  for(Int_t ich=0; ich<MCH; ich++){
	    TMaxV=tVVtxc0[ibinMax];
	    Float_t TFitMax=TMaxV+MBINSIG/2;
	    //	    if(VMax[ich]>0 && TMath::Finite(VVtxc0[ibinMax])==1 && TMath::IsNaN(VVtxc0[ibinMax])==0 && LFitNuSig && TFitMax<tVVtxc0[Nbins-1]){
	    if(LFitNuSig && TFitMax<tVVtxc0[Nbins-1] && LUseCh[ich]){
	      TF1 *NuSig=new TF1("NuSig",DampExp,-256,256,5);

	      Float_t Aval=fMIN(VVtxc0[ibinMax],VMAXMV);
	      Aval=fMAX(VVtxc0[ibinMax],-VMAXMV);
	      NuSig->FixParameter(0,Aval);
	      NuSig->FixParameter(1,TMaxV);
	      NuSig->SetParameter(2,10.);
	      NuSig->SetParameter(3,1.);
	      NuSig->FixParameter(4.,0.);
	      NuSig->SetParLimits(0,-VMAXMV,VMAXMV);
	      NuSig->SetParLimits(2,1,20);
	      NuSig->SetParLimits(3,0.5,1.5);
	      NuSig->SetRange(TMaxV,TFitMax);
	      gVtxc0Fit[ich]->GetXaxis()->SetRangeUser(TMaxV,TFitMax);
	      gVtxc0Fit[ich]->Fit("NuSig","BRQ"," ",TMaxV,TFitMax); //http://root.cern.ch/root/html/TH1.html#TH1:Fit	  
	      NuSig->ReleaseParameter(0);
	      //	  NuSig->ReleaseParameter(3);
	      NuSig->ReleaseParameter(4);
	      //	  NuSig->FixParameter(3,NuSig->GetParameter(3));
	      //	  NuSig->FixParameter(4,NuSig->GetParameter(4));
	      //	  NuSig->ReleaseParameter(2);
	      //	      if(1==TMath::Finite(NuSig->GetParameter(0)) && 0==TMath::IsNaN(NuSig->GetParameter(0))){ 
	      gVtxc0Fit[ich]->Fit("NuSig","BRQ"," ",TMaxV,TFitMax);  
	      NUSIG->Fill(ich,NuSig->GetParameter(0),NuSig->GetParameter(1),NuSig->GetParameter(2),NuSig->GetParameter(3),NuSig->GetParameter(4),NuSig->GetChisquare(),itemplate,itrigCodeZ);
	      NuSigChisquare[itemplate]+=NuSig->GetChisquare();
		//	      }
		//	      delete fNuSig;
	      //	      LFitRxCh[ich]=true;
	      //	      NuSig->Draw();
	      delete NuSig;
	    }
	  }

	  if(nhits>=NHITSRECO){ // && LTemplate0 && (!LCalPulser||iMC==1)){
	    Vtx[0]=T0event; //Should be set to the earliest cable-delay corrected time
	    Vtx[1]=Rx[ichFirstHit].x;
	    Vtx[2]=Rx[ichFirstHit].y;
	    Vtx[3]=Rx[ichFirstHit].z;
	    cout<<"phi of first hit/event="<<atan2(Vtx[2],Vtx[1])<<" "<<atan2(showerYMC[itemplate],showerXMC[itemplate])<<endl;
	    eventrecoMinuit->mnparm(0,"t0",Vtx[0],step[0],MINUIT_TMIN,MINUIT_TMAX,ierflg);
	    eventrecoMinuit->mnparm(1,"x0",Vtx[1],step[1],MINUIT_XMIN,MINUIT_XMAX,ierflg);
	    eventrecoMinuit->mnparm(2,"y0",Vtx[2],step[2],MINUIT_YMIN,MINUIT_YMAX,ierflg);
	    eventrecoMinuit->mnparm(3,"z0",Vtx[3],step[3],MINUIT_ZMIN,MINUIT_ZMAX,ierflg);
	    eventrecoMinuit->SetFCN(calc_chi_square); //chi square routine
	    
	    memset(FitPoint,0,sizeof(FitPoint));
	    memset(FitPerror,0,sizeof(FitPerror));
	    
	    chisqvtx=0;
	    eventrecoMinuit->mnexcm("SIMPLEX", argsSimplexMigrad, 2, convergence[0]);	// SIMPLEX doesn't use UP parameter as soon as tolerance is specified
	    eventrecoMinuit->mnexcm("MIGRAD", argsSimplexMigrad, 2, convergence[1]);	// But MIGRAD somehow uses UP along with tolerance
	    
	    eventrecoMinuit->mnstat(chi_square, tmp_double, tmp_double, tmp_int, tmp_int, tmp_int);
	    eventrecoMinuit->GetParameter(0, FitPoint[0], FitPerror[0]);
	    eventrecoMinuit->GetParameter(1, FitPoint[1], FitPerror[1]);
	    eventrecoMinuit->GetParameter(2, FitPoint[2], FitPerror[2]);
	    eventrecoMinuit->GetParameter(3, FitPoint[3], FitPerror[3]);    
	    chisq0[itemplate]=chi_square;
	    r2vtx0[itemplate]=sqrt(FitPoint[1]*FitPoint[1]+
				   FitPoint[2]*FitPoint[2]+
				   FitPoint[3]*FitPoint[3]);
	    the0[itemplate]=acos(FitPoint[3]/r2vtx0[itemplate]);
	    phi0[itemplate]=atan2(FitPoint[2],FitPoint[1]);
	    NVTX->Fill(ipkValPhiV[itemplate],ipkValTheV[itemplate],ipkValPhiH[itemplate],ipkValTheH[itemplate],fpkValMapV[itemplate],fpkValMapH[itemplate],showerEMC[itemplate],showerXMC[itemplate],showerYMC[itemplate],showerZMC[itemplate],FitPoint[1],FitPoint[2],FitPoint[3],iEvFailCode2);
	  }
	}
	IncrementCuts(chisq0[itemplate]<CHISQPERHITMAX*nhits);	
	Int_t idelpkVH=abs(ipkValPhiV[itemplate]-ipkValPhiH[itemplate]);
	if(idelpkVH>180)idelpkVH=(360-idelpkVH);
	IncrementCuts(idelpkVH<DELIPKVALS);	

	//cross-correlate this event with other events:
	Float_t hdT1d_rmsVal, hdT1d_rmsValMaxCor;
	xcvalMax=-1;
	fdelTSqSumMin=999999;
	ndeltaTSqSumMin=-1;
	if(LTemplate0){
	  for(Int_t it=0; it<itemplate; it++){
	    Float_t xcvalSum=0;
	    ndeltaTSqSum=0;
	    deltaTSqSum=0;
	    deltaASqSum=0;
	    deltaTxcSqSum=0;
	    fdelTSqSum=999;
	    for(Int_t ich=0; ich<MCH; ich++){
	      gVtxc[ich]->Draw("AL");

	      //by cbora
	      // sprintf(cpngname,"/home/dbesson/ntem/tem/gVtxcEv%d_Ch%d.png",ievt,ich);
 sprintf(cpngname,"gVtxcEv%d_Ch%d.png",ievt,ich);
	      //	      c3->SaveAs(cpngname);
	      TGraph *grCor=FFTtools::getCorrelationGraph(gVtxc[ich],gVTXC[ich][it]);
	      Int_t peakBin=FFTtools::getPeakBin(grCor);
	      Float_t peakVal=FFTtools::getPeakVal(grCor);
	      Float_t fpeakBin=fMIN(abs(fmod(peakBin,512)),abs(fmod(512-peakBin,512)));
	      hdT1d0->Fill(fpeakBin,peakVal);
	      fpeakBin=fMIN(fpeakBin,abs(fmod(1024-peakBin,1024)));
	      hdT2d->Fill(fpeakBin,NEvTot,peakVal);
	      if(peakVal>500)hdT1d->Fill(fpeakBin,peakVal);
	      xcvalSum+=peakVal;
	      Double_t *dtVals=grCor->GetX();
	      Double_t *xcVals=grCor->GetY();
	      delete grCor;
	      if(LUseChT[ich][it] && LUseChT[ich][itemplate])
		deltaASqSum+=fabs(SNRRxMaxV[ich][itemplate]-SNRRxMaxV[ich][it])/(SNRRxMaxV[ich][itemplate]+SNRRxMaxV[ich][it]);
	      
	      for(Int_t jch=ich+1; jch<MCH; jch++)
		if(LUseChT[ich][it]&&LUseChT[jch][it]&&LUseChT[ich][itemplate]&&LUseChT[jch][itemplate]){
		  ndeltaTSqSum++;
		  deltaTSqSum+=pow(RxHitTimeTempl[ich][jch][itemplate]-RxHitTimeTempl[ich][jch][it],2);
		  deltaTxcSqSum+=pow(xcHitTimeTempl[ich][jch][itemplate]-xcHitTimeTempl[ich][jch][it],2);
		  //		if(iMC==1)printf("TimeRxMC[%d][template=%d]=%f / TimeRxMC[%d][template=%d]=%f\n",ich,itemplate,TimeRxMC[ich][itemplate],ich,it,TimeRxMC[ich][it]);
		}
	    }
	    if(ndeltaTSqSum>NDTSETMIN){
	      fdelTSqSum=sqrt(deltaTSqSum)/ndeltaTSqSum;
	      hxcvalSum->Fill(xcvalSum,sqrt(deltaTSqSum)/ndeltaTSqSum);
	      hdeltaT->Fill(sqrt(deltaTSqSum)/ndeltaTSqSum);
	      hdeltaTxc->Fill(sqrt(deltaTxcSqSum)/ndeltaTSqSum);
	      if(fdelTSqSum<fdelTSqSumMin){
		fdelTSqSumMin=fdelTSqSum/ndeltaTSqSum;
		ndeltaTSqSumMin=ndeltaTSqSum;
	      }
	    }
	    if(nhits>0)
	      hdeltaA->Fill(deltaASqSum/nhits);
	    hdT1d_rmsVal=hdT1d->GetRMS();
	    hdT1d_rms->Fill(hdT1d_rmsVal);
	    hXCtrigCode->Fill(xcvalSum,itrigCodeZ);
	    if(xcvalSum>xcvalMax){
	      xcvalMax=xcvalSum;
	      hdT1d_rmsValMaxCor=hdT1d_rmsVal;
	    } //SET MATCHING CRITERION HERE
	    Bool_t Lxcval=xcvalSum>xcvalMinMatchTemplate;
	    Bool_t LdelT=fdelTSqSum<dTSqSumMatch&&ndeltaTSqSum>=NDTSETMIN;
	    if(Lxcval)
	      cout<<"----Template="<<itemplate<<" NEvTemplate0="<<NEvTemplate0<<" matches to template "<<it<<" by chisqr criterion / xcvalSum="<<xcvalSum<<endl;
	    if(LdelT){
	      cout<<"----Template "<<itemplate<<" matches to "<<it<<" by delT criterion / fdelTSqSum="<<fdelTSqSum<<"/ndeltaTSqSum="<<ndeltaTSqSum<<endl;
	      if(LDebug)
		for(Int_t ich=0; ich<MCH; ich++)
		  cout<<"Found times: "<<RxHitTime0[ich][itemplate]<<" "<<RxHitTime0[ich][it]<<" Generated times: "<<TimeRxMC[ich][itemplate]<<" "<<TimeRxMC[ich][it]<<endl;
	    }
	    if(iEV>=0 && (Lxcval || LdelT)){
	      hTemplateMatch->Fill(it);
	      npat[itemplate]++;
 	      if(npat[itemplate]<MAXPAT){ 
		MatchGraphsOut<<iEvNoThis<<" "<<itrigCodeZ<<" "<<itemplate<<" "<<npat[itemplate]<<endl;
		for(Int_t ich=0; ich<MCH && npat[itemplate]<MAXPAT; ich++){
		  MatchGraphsOut<<ich<<" "<<gVtxc[ich]->GetN()<<endl;
		  Double_t* tgVtxc=gVtxc[ich]->GetX();
		  Double_t* VgVtxc=gVtxc[ich]->GetY();
		  for(Int_t ibg=0; ibg<gVtxc[ich]->GetN(); ibg++)
		    MatchGraphsOut<<tgVtxc[ibg]<<" "<<VgVtxc[ibg]<<endl;
		  if(!LMaxTemplates){
		    gTemp[0]=(TGraph*)gVtxc[ich]->Clone();
		    gTemp[1]=(TGraph*)gVTXCAvg[ich][it]->Clone();
		    delete gVTXCAvg[ich][it];
		    gVTXCAvg[ich][it]=FFTtools::correlateAndAverage(2,gTemp);
		    delete gTemp[0];
		    delete gTemp[1];
		  }
		}
		//		LTemplate0=false; //demote this template as matched
		//		LMatchedTemplate0[it]=false;
		//		LMatchedTemplate0[itemplate]=false;
		//		it=itemplate; 
	      }
	    }
	  }
	  if(LCalPulser){
	    if(xcvalMax>xcvalMinMatchTemplate)nCalPulserMatch++;
	    else nCalPulserPass0++;
	  }
	}
	//	IncrementCuts(true); //past template matching
	hxcvalMax->Fill(xcvalMax,itrigCodeZ);

	NMC1->Fill(nuEMC[itemplate],showerEMC[itemplate],showerXMC[itemplate],showerYMC[itemplate],showerZMC[itemplate],LTemplate0,NChHitTrigger,iEvFailCode2,WfPower,nhits,launchAngleMC[itemplate],showerPhiMC[itemplate],showerThetaMC[itemplate]);
	if(nhits>=NHITSMIN){
	  memset(xNtuple,0,sizeof(xNtuple));
	  xNtuple[0]=SNRMaxFreq;
	  xNtuple[1]=RxTOT/nhits;
	  xNtuple[2]=WfPower;
	  xNtuple[3]=WfPowerMaxPos[itemplate]/nhits;
	  xNtuple[4]=WfPowerMaxNeg[itemplate]/nhits;
	  xNtuple[5]=NbinDiffNSigMax[itemplate]/nhits;
	  xNtuple[6]=nhits;
	  xNtuple[7]=NComb32;
	  xNtuple[8]=xcvalMax;
	  xNtuple[9]=VMaxVRms;
	  xNtuple[10]=VMaxHRms;
	  xNtuple[11]=xCorVV;
	  xNtuple[12]=xCorHH;
	  xNtuple[13]=xCorVH;
	  xNtuple[14]=ChiSqTot/nhits;
	  xNtuple[15]=eventPtr->head.unixTimeUs;
	  xNtuple[16]=SkewSum/nhits;
	  xNtuple[17]=KurtSum/nhits;
	  xNtuple[18]=itemplate;
	  xNtuple[19]=itrigCodeZ;
	  if(NComb32>0){
	    xNtuple[20]=fKGTest32[itemplate]/NComb32; //SHOULD BE NORMED BY NCOMB32!
	    xNtuple[31]=ZDot32[itemplate]/NComb32;
	    xNtuple[32]=fChi2Test32[itemplate]/NComb32;
	  }
	  xNtuple[21]=sqrt(FitPoint[1]*FitPoint[1]+FitPoint[2]*FitPoint[2]+FitPoint[3]*FitPoint[3]);
	  if(xNtuple[21]>0){
	    xNtuple[22]=acos(FitPoint[3]/xNtuple[21]);
	    xNtuple[23]=atan2(FitPoint[2],FitPoint[1]);
	  }
	  xNtuple[24]=NuSigChisquare[itemplate]/nhits;
	  xNtuple[25]=ipkValPhiV[itemplate];
	  xNtuple[26]=ipkValTheV[itemplate];
	  xNtuple[27]=ipkValPhiH[itemplate];
	  xNtuple[28]=ipkValTheH[itemplate];
	  xNtuple[29]=fpkValMapV[itemplate];
	  xNtuple[30]=fpkValMapH[itemplate];
	  xNtuple[33]=delTT;
	  xNtuple[34]=nhitsV;
	  xNtuple[35]=nhitsH;
	  xNtuple[36]=showerEMC[itemplate];
	  xNtuple[37]=showerXMC[itemplate];
	  xNtuple[38]=showerYMC[itemplate];
	  xNtuple[39]=showerZMC[itemplate];
	  xNtuple[40]=showerThetaMC[itemplate];
	  xNtuple[41]=showerPhiMC[itemplate];
	  xNtuple[42]=WfPowerSurf;
	  xNtuple[43]=WfPSig/nhits;
	  xNtuple[44]=irunno[itemplate];
	  xNtuple[45]=ievtno[itemplate];
	  xNtuple[46]=SNRRxMaxVAvg/nhits;
	  xNtuple[47]=NThresh[5];
	  xNtuple[48]=NThresh[6];
	  xNtuple[49]=NThresh[7];
	  //	  cout<<NThresh[5]<<" "<<NThresh[6]<<" "<<NThresh[7]<<endl;
	  xNtuple[50]=fdelTSqSumMin;
	  xNtuple[51]=ndeltaTSqSumMin;
	  xNtuple[52]=NThresh[8];
	  xNtuple[53]=NThresh[9];
	  xNtuple[54]=NThresh[10];
	  xNtuple[55]=NThresh[2];
	  xNtuple[56]=NThresh[3];
	  xNtuple[57]=NThresh[4];
	  xNtuple[58]=fSNRMaxFreq;
	  N0CUTS->Fill(xNtuple);
	  //	  NIFG->Fill(ipkValPhiV[itemplate],ipkValTheV[itemplate],ipkValPhiH[itemplate],ipkValTheH[itemplate],fpkValMapV[itemplate],fpkValMapH[itemplate],itemplate,itrigCodeZ,hVMaxV1->GetRMS(),hVMaxH1->GetRMS(),hVMaxV1->GetMean(),hVMaxH1->GetMean(),xCorVV,xCorHH,xCorVH);
	  //	  NXC->Fill(ZDot32[itemplate],fChi2Test32[itemplate],fresidChi2_32[itemplate],fKGTest32[itemplate],NuSigChisquare[itemplate],XCorSum,nhitsH,nhitsV,nhits,igoodX,delTT,SkewSum,KurtSum);
	}
	IncrementCuts(fdelTSqSumMin>dTSqSumMatch);
	IncrementCuts(iEV<0||xcvalMax<xcvalMinMatchTemplate);
	if(LTemplate0){
	  for(Int_t ich=0; ich<MCH && !LMaxTemplates; ich++){
	    c2->cd(ich+1);
	    gVTXC[ich][itemplate]=(TGraph*)gVtxc[ich]->Clone();
	    gVTXCAvg[ich][itemplate]=(TGraph*)gVtxc[ich]->Clone();
	    gVTXC[ich][itemplate]->Draw("AL");
	  }

	  //by cbora
	  //	  sprintf(cpngname,"/home/dbesson/ntem/tem/gVTXC_%s_Ev%d_template%d_iEV%d_%s.png",cListFileIn,ievt,itemplate,iEV,datestamp);
	  sprintf(cpngname,"gVTXC_%s_Ev%d_template%d_iEV%d_%s.png",cListFileIn,ievt,itemplate,iEV,datestamp);
	  if(iEV==0 && itemplate<MPNGSAVE && !LMaxTemplates)c2->SaveAs(cpngname);
	  
	  ievtno[itemplate]=calEvPtr->head.eventNumber;
	  irunno[itemplate]=atof(runString.c_str());
	  TtrigVTXC[itemplate]=eventPtr->head.unixTime;
	  TtrigUsVTXC[itemplate]=eventPtr->head.unixTimeUs;
	  xcValMax[itemplate]=xcvalMax;
	  grVT0[itemplate]=grVT[0];
	  //	  hTtrig->Fill(eventPtr->head.unixTime,itemplate);
	  //	  hTustrig->Fill(eventPtr->head.unixTimeUs,itemplate);
	  //	    if(!LCalPulser){
	  RXTOT[itemplate]=RxTOT;
	  WfPowerTemplate[itemplate]=WfPower;
	  WfPowerSurfTemplate[itemplate]=WfPowerSurf;
	  hWfPowerTemplates->Fill(log10(WfPower));
	  itrigCodePat[itemplate]=itrigCodeZ;
	  NTMPL->Fill(xNtuple); //WfPower,WfPowerSurf,delTT,itrigCodeZ,eventPtr->head.unixTime,eventPtr->head.unixTimeUs,calEvPtr->head.eventNumber,atof(runString.c_str()),itemplate,npat[itemplate],SNRMaxFreq);
	  //	  T0->Fill();
	  TVt->Fill();
	  cout<<"Setting itemplate number "<<itemplate<<" xcvalMax="<<xcvalMax<<" WfPower="<<WfPower<<" itrigCodeZ="<<itrigCodeZ<<" iTemplCalPulser="<<iTemplCalPulser<<" nhits="<<nhits<<" NEvTot="<<NEvTot<<endl; //" evt no="<<ievtno[it]<<endl;	
	  //by cbora	
  //	  sprintf(cpngname,"/home/dbesson/ntem/tem/gVtxc0Ev_%s_%d_fit_template%d_iEV%d_%s.png",cListFileIn,ievt,itemplate,iEV,datestamp);
	sprintf(cpngname,"gVtxc0Ev_%s_%d_fit_template%d_iEV%d_%s.png",cListFileIn,ievt,itemplate,iEV,datestamp);
	  if(iEV==0 && itemplate<MPNGSAVE && LFitNuSig && !LMaxTemplates)c1->SaveAs(cpngname);
	  if(LCalPulser)iTemplCalPulser++;
	  if(!LCalPulser || iTemplCalPulser<MAXCALPULSERTEMPLATES){
	    itemplate++;
	    hEvCuts->Fill(M_NEVCUTS,itrigCodeZ);
	    if(iEV==0||iEV>=3){
	      TemplateOut<<iEvNoThis<<" "<<itrigCodeZ<<" "<<itemplate<<" "<<npat[itemplate]<<endl;
	      for(Int_t ich=0; ich<MCH; ich++){
		TemplateOut<<ich<<" "<<gVtxc[ich]->GetN()<<endl;
		Double_t* tgVtxc=gVtxc[ich]->GetX();
		Double_t* VgVtxc=gVtxc[ich]->GetY();
		for(Int_t ibg=0; ibg<gVtxc[ich]->GetN(); ibg++)
		  TemplateOut<<tgVtxc[ibg]<<" "<<VgVtxc[ibg]<<endl;
	      }
	    }
	  }
	  NEvTemplate0++;
	  //	  if(iEV<0 && itemplate>=MAXPAT)itemplate--;
	  if(iEV!=2 && itemplate>=MAXPAT){
	    LMaxTemplates=true;
	    itemplate--;
	  }
	}
	for(Int_t ich=0; ich<MCH; ich++){
	  delete gVtxc0Fit[ich];
	  if(gVtxc[ich])
	    delete gVtxc[ich];
	}	  
	
	if(LPrintOut){
	  PrintOutNEvCuts(); //0
	  for(Int_t ipat=0; ipat<itemplate; ipat++)
	    if(npat[ipat]>0)
	      cout<<"npat["<<ipat<<"]="<<npat[ipat]<<endl;
	  if(NEvUnbias>0){
	    for(Int_t kch=0; kch<MCH; kch++){
	      Rx[kch].Vrms68=Vrms68[kch]/NEvUnbias;
	      if(fabs(Rx[kch].Vrms68-Rx[kch].Vrms)/Rx[kch].Vrms>0.05)
		cout<<"Rx["<<kch<<"].Vrms="<<Rx[kch].Vrms<<" "<<"<Vrms68>="<<Rx[kch].Vrms68<<endl;
	    }
	  }
	}
	if(itrigCodeZ==0)
	  for(Int_t ithresh=0; ithresh<MTHRESH; ithresh++)
	    hMaxVNHit->Fill(ithresh,NThresh[ithresh]);

	for(Int_t icut=0; icut<M_NEVCUTS && !LCalPulser; icut++)
	  if(iEvFailCode2==pow(2,icut))
	    hFailX->Fill(icut);
	
	NEvTot++;
      } //if(iCP==0||LCalPulser)
    } //end main event loop
    NFileAna++;
    //    for(Int_t ich=0; ich<MCH; ich++)
    //      for(Int_t ithresh=0; ithresh<MTHRESH; ithresh++)
    //	hMaxVNHit->Fill(ich,ithresh,NThresh[ithresh][0]);
  } //end all events
  
  cout<<"ANALYZED "<<NEvTot<<" EVENTS "<<endl;
  
  f.cd();
  for(Int_t it=0; it<itemplate; it++){
    Int_t ib0=0;
    //    sprintf(hName,"gVTXC_FFT%d_ch%d",it,ich);
    //    hFFT[it]=new TH1F(hName,hName,MBINS,-0.5,MBINS-0.5);
    sprintf(hName,"hVTXC%d",it);
    hVTXC[it]=new TH1F(hName,hName,MBINS,-0.5,MBINS-0.5);

    sprintf(hName,"hFFTVTXC%d",it);
    hFFTVTXC[it]=new TH1F(hName,hName,MBINS,-0.5,MBINS-0.5);

    Int_t iglobin=0;
    Int_t NbinsVTXC=0;
    Int_t NbinsFFTVTXC=0;
    RxTOT0=0;

    for(Int_t ich=0; ich<MCH; ich++){

      sprintf(gName,"gVTXC%d_ch%d",it,ich);
      gVTXC[ich][it]->SetName(gName);
      gVTXC[ich][it]->SetTitle(gName);
      //      gDirectory->Append(gVTXC[ich][it]);

      sprintf(gName,"gVTXCAvg_npat%d_ch%d",it,ich);
      gVTXCAvg[ich][it]->SetName(gName);
      gVTXCAvg[ich][it]->SetTitle(gName);
      if(npat[it]>4)
	gDirectory->Append(gVTXCAvg[ich][it]);

      Double_t *V=gVTXC[ich][it]->GetY();
      Double_t *t=gVTXC[ich][it]->GetX();
      Int_t Nlen=gVTXC[ich][it]->GetN();
      hNlen->Fill(it,ich,Nlen);
      ht0->Fill(it,ich,t[0]);
      hdt->Fill(it,ich,t[1]-t[0]);
      for(Int_t igrbin=0; igrbin<Nlen; igrbin++){
	hVTXC[it]->Fill(igrbin+NbinsVTXC,V[igrbin]);
	WfPowerc[ich]+=V[igrbin]*V[igrbin];
      }

      TH1F *htemp=new TH1F("htemp","htemp",Nlen,t[0],t[Nlen-1]);
      htemp->FillN(Nlen,t,V);

      Float_t VThresh=NSigmaCut*Rx[ich].Vrms;
      RxdTPos[ich]=htemp->FindLastBinAbove(VThresh)-htemp->FindFirstBinAbove(VThresh);
      delete htemp;

      gFFTVTXC[ich][it]=FFTtools::makePowerSpectrum(gVTXC[ich][it]);
      sprintf(gName,"gFFTVTXC%d_ch%d",it,ich);
      gFFTVTXC[ich][it]->SetName(gName);
      gFFTVTXC[ich][it]->SetTitle(gName);
      if(LStoreAllChs)gDirectory->Append(gFFTVTXC[ich][it]);

      Double_t *VFFT=gFFTVTXC[ich][it]->GetY();
      Double_t *fFFT=gFFTVTXC[ich][it]->GetX();
      Int_t NlenFFT=gFFTVTXC[ich][it]->GetN();
      hlenArr->Fill(Nlen,NlenFFT);
      //      if(NlenFFT!=Nlen || NbinsVTXC!=NbinsFFTVTXC)cout<<"Ch "<<ich<<" NlenFFT/Nlen="<<NlenFFT<<"/"<<Nlen<<endl;

      Int_t iFFTMax=0;
      AFFTMax[ich]=FFTtools::getPeakVal(gFFTVTXC[ich][it])/sqrt(WfPowerc[ich]);
      Int_t iFFTMax=FFTtools::getPeakBin(gFFTVTXC[ich][it]);
      fFFTMax[ich]=fFFT[iFFTMax];
      for(Int_t igrbin=0; igrbin<NlenFFT; igrbin++)
	if(igrbin+NbinsFFTVTXC<MBINS)
	  hFFTVTXC[it]->Fill(igrbin+NbinsFFTVTXC,VFFT[igrbin]);
      
      iglobin+=Nlen;
      NbinsFFTVTXC+=NlenFFT;
      NbinsVTXC+=Nlen;
      //      cout<<Nlen<<" "<<NbinsVTXC<<endl;
      //      (TGraph*)grtemp->Clone();
      //      cout<<VFFT[0]<<endl;
      //      for(Int_t ib=ib0; ib<ib0+Nlen&&ib<MBINS,ib++)
      //      	hFFT[it]->Fill(ib,VFFT[ib-ib0]);
      //      ib0+=Nlen;

      for(Int_t itbin=0; itbin<gVTXC[ich][it]->GetN(); itbin++){
	if(itbin>0)
	  if(V[itbin]*V[itbin-1]<0)
	    NX0++;
	//      Int_t itbin=0;
	//      while(itbin<gVTXC[ich][it]->GetN() && Rx[ich].HitTime==RxHitTime00){
	//	  if(V[itbin]>NSigmaCut*Vrms68[ich]/NVrms68 && RxHitTime[ich]==-1){
	if(fabs(V[itbin])>VThresh){
	  RxTOT0++;
	  if(LDebug)cout<<"Ch "<<ich<<" pol="<<araGeom->fStationInfo[station].fAntInfo[ich].polType<<" V["<<t[itbin]<<"]="<<V[itbin]<<endl;
	}
      }
    } //careful: LUseCh ONLY defined for template it!

    grVTXC[it]=new TGraph(hVTXC[it]);
    sprintf(gName,"grVTXC%d",it);
    grVTXC[it]->SetName(gName);
    sprintf(gName,"grVTXC%d_theta%4.2f_phi%4.2f_r2vtx%g",it,the0[it],phi0[it],r2vtx0[it]);
    grVTXC[it]->SetTitle(gName);
    gDirectory->Append(grVTXC[it]);

    grFFTVTXC[it]=new TGraph(hFFTVTXC[it]);
    sprintf(gName,"grFFTVTXC%d",it);
    grFFTVTXC[it]->SetName(gName);
    grFFTVTXC[it]->SetTitle(gName);
    gDirectory->Append(grFFTVTXC[it]);

    //    cout<<"template "<<it<<" has nhits="<<nhits<<endl;
    //now match time differences:
    it0=it;
    it0Match=0;

    //Insert1

    Tsec=TtrigVTXC[it];
    Tus=TtrigUsVTXC[it];
    dTT=0;
    if(it>0){
      dTT=TtrigVTXC[it]+micro*TtrigUsVTXC[it]-TtrigVTXC[it-1]-micro*TtrigUsVTXC[it-1];
      //      cout<<TtrigVTXC[it]<<" "<<micro*TtrigUsVTXC[it]<<" "<<TtrigVTXC[it-1]<<" "<<micro*TtrigUsVTXC[it-1]<<" "<<dTT<<endl;
    }
    
    TTimeStamp trigTime((time_t)Tsec,(Int_t)1000*Tus);      
    trigTime.GetTime(true,0,&trighour,&trigmin,&trigsec);
    trigday=trigTime.GetDayOfYear(true,0);
    ChiSqTot=0;
    WfPT=WfPowerTemplate[it];
    WfPTS=WfPowerSurfTemplate[it];

    //    VTXx[it]=FitPoint[1];
    //    VTXy[it]=FitPoint[2];
    //    VTXz[it]=FitPoint[3];
    jTemplate=it;
    iEvtNo=ievtno[it];
    iRunNo=irunno[it];
    WfPThresh=WfPowerThreshServo[it];
    nPat=npat[it];
    iTrig=itrigCodePat[it];
    xcvalMax=xcValMax[it];
    grVT00=grVT0[it];
    the=the0[it];
    phi=phi0[it];
    r2vtx=r2vtx0[it];
    ipkValIFGphiV=ipkValPhiV[it];
    ipkValIFGphiH=ipkValPhiH[it];
    ipkValIFGtheV=ipkValTheV[it];
    ipkValIFGtheH=ipkValTheH[it];
    fpkValIFGV=fpkValMapV[it];
    fpkValIFGH=fpkValMapH[it];

    iPkbinIFGphiV=iPkbinPhiV[it];
    iPkbinIFGphiH=iPkbinPhiH[it];
    iPkbinIFGtheV=iPkbinTheV[it];
    iPkbinIFGtheH=iPkbinTheH[it];
    fPkValIFGV=fPkValMapV[it];
    fPkValIFGH=fPkValMapH[it];

    nhits=nhits0[it];
    showerX=showerXMC[it];
    showerY=showerYMC[it];
    showerZ=showerZMC[it];
    showerE=showerEMC[it];
    nuE=nuEMC[it];
    showerPhi=showerPhiMC[it];
    showerTheta=showerThetaMC[it];
    showerPhi=showerPhiMC[it];
    nSurfHit=nSurfHitMC[it];
    nSigktHits=nSigktHitsMC[it];
    chisqvtx=chisq0[it];
    dzFirstHit=zFirstHit[it]-RxCOGz;
    WFPMaxPos=-1;
    WFPMaxNeg=-1;
    NuSigChi2=-1;
    if(nhits>0){
      RxTOT=RXTOT[it]/nhits;
      WFPMaxPos=WfPowerMaxPos[it]/nhits;
      WFPMaxNeg=WfPowerMaxNeg[it]/nhits;
      NuSigChi2=NuSigChisquare[it]/nhits;
      NbindelNSigMax=NbinDiffNSigMax[it]/nhits;
    }
    fKGTest=fKGTest32[it];
    fChi2Test=fChi2Test32[it];
    fresidChi2Test=fresidChi2_32[it];
    ZDot=ZDot32[it];
    NCHgt400MHz=0;
    for(Int_t ich=0; ich<MCH; ich++){
      RxHitTime[ich]=RxHitTime0[ich][it];
      MaxFreqFFTSNRch[ich]=MaxFreqFFTSNR[ich][it];
      MaxFreqFFTch[ich]=MaxFreqFFT[ich][it];
      SNRRxMaxVCh[ich]=SNRRxMaxV[ich][it];
      if(fFFTMax[ich]>0.4)
	NCHgt400MHz++;
    }
    T->Fill();
  }


  //cross-correlate surviving templates with MC
  if(LDebug) gObjectTable->Print();
  if(LDebug)gDirectory->ls();
  for(Int_t it=0; it<itemplate; it++){
    if(!LMatchedTemplate0[it])
      NEvCuts[M_NEVCUTS-1][itrigCodeZ]++;
    else{
      sprintf(gName,"grVTXC%d_Mnpat%d",it,npat[it]);
      grVTXC[it]->SetTitle(gName);
      if(hVTXC[it]){
	sprintf(hName,"hVTXC%d_Mnpat%d",it,npat[it]);
	hVTXC[it]->SetTitle(hName);
	//	  gDirectory->Remove(hVTXC[it]);	  
	//	  gDirectory->Remove(hFFTVTXC[it]);	  
	//	  cout<<"Deleting histograms for it template= "<<it<<endl;
      }
    } 
  }

  f.cd();
  gDirectory->Append(hFailX);
  gDirectory->Append(hFailI);

  hDeadTime->Fill(0,deadTime1_tot);
  hDeadTime->Fill(1,deadTime2_tot);
  hDeadTime->Fill(2,TEvent-TFirstEvent);  
  hDeadTime->Fill(3,TFirstEvent);
  hDeadTime->Fill(4,TEvent);
  hDeadTime->Fill(5,deadTime_tot); //http://www.hep.ucl.ac.uk/uhen/ara/araroot/branches/3.3/_icrr_trigger_monitor_8cxx-source.shtml#l00058
  hDeadTime->Fill(6,fracLive);
  for(Int_t icut=0; icut<M_NEVCUTS; icut++){
    for(Int_t jtrigCodeZ=0; jtrigCodeZ<3; jtrigCodeZ++)
      hNEvCuts->Fill(icut,jtrigCodeZ,NEvCuts[icut][jtrigCodeZ]);
  }
  TRUN->Fill();
  T->Show(0);
  N0CUTS->Show(0);
  TRUN->Show(0);
  hEvCuts->Write();
  hFailX->Write();
  hFailI->Write();
  f.Write();
  hEvCuts->ProjectionX()->Print("all");
  PrintOutNEvCuts(); //1
  if(nCalPulserPass0>0)
    cout<<"Cal Pulser matching fraction="<<float(nCalPulserMatch)/(nCalPulserPass0+nCalPulserMatch)<<endl;
  //changed by cbora
  cout<<"\nClosing "<< outFileName <<" after analyzing "<<NEvTot<<" events/"<<"nEvCalPulser="<<nEvCalPulser<<" NEvUnbias="<<NEvUnbias<<" itemplate="<<itemplate<<" NEvTemplate0="<<NEvTemplate0<<endl;
  TemplateOut.close();

  abort();
  return 0;
}

void calc_chi_square(Int_t &npar, Double_t *gin, Double_t &chisquare, Double_t *par, Int_t iflag){
  //  const double sLightAir = 0.29970551819; //velocity of light in meters/ns
  //  const double nIce=1.78;
  chisquare=0.0;
  ndof=0;
  float m2km =0.001;
  for(int ich=0; ich<MCH; ich++){
    if(LUseCh[ich]){
      if(Rx[ich].HitTime==RxHitTime00)
	cout<<"PANIC IN DETROIT!"<<endl;
      ndof++;
      float DistanceSq=
	(par[1]-Rx[ich].x)*(par[1]-Rx[ich].x)+
	(par[2]-Rx[ich].y)*(par[2]-Rx[ich].y)+
	(par[3]-Rx[ich].z)*(par[3]-Rx[ich].z);

      Float_t TransitTimens=sqrt(DistanceSq)/(sLightAir/nIce);
      TransitTimens=ntimexp(m2km*par[3],m2km*Rx[ich].z,m2km*sqrt(DistanceSq));
      //      if(Rx[i].TimeDelayNsCorr!=0)cout<<"ERROR!!!"<<endl;
      Float_t delta=par[0]+TransitTimens-Rx[ich].HitTime; //+Rx[ich].dtNsCorr; //remove dtNsCorr
      //      cout<<DistanceSq<<" "<<Rx[ich].HitTime<<" "<<Rx[ich].x<<" "<<Rx[ich].y<<" "<<Rx[ich].z<<" "<<delta<<endl;
      Rx[ich].dtFitResid=delta;
      chisquare+=delta*delta;
      ChiSqTot=chisquare;
    }
  }
  return;
}


