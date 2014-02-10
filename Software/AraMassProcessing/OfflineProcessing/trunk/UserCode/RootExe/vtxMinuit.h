  // fitting data
const double sLightAir = 0.29970551819; //velocity of light in meters/ns
const double nIce=1.78;
Int_t ndof=0;
Int_t ierflg=0;
Float_t Vtx[4];
Bool_t LUseCh[MCH];
Double_t chi_square;
Double_t ChiSqTot;
Double_t tmp_double=0;
Double_t tmp_int=0;
//Float_t RxHitTime[MCH]={0};
//Float_t RxdtFitResid[MCH]={0};
Double_t FitPoint[4], FitPointxc[4], FitPerror[4], FitPerrorxc[4]; //, FitPoint[4], FitPerror[4];
Double_t arglist[2]={200,1}; //,10,10};	// UP = 200, 1 - is not used
Double_t argsSimplexMigrad[2]={1000, 1};	// MaxCalls = 1000, tolerance = 1
Double_t step[4]={20,4,4,4};
const Float_t MINUIT_TMIN=-512;
const Float_t MINUIT_TMAX=0;
const Float_t MINUIT_XMIN=-100;
const Float_t MINUIT_XMAX=100;
const Float_t MINUIT_YMIN=-100;
const Float_t MINUIT_YMAX=100;
const Float_t MINUIT_ZMIN=-100;
const Float_t MINUIT_ZMAX=100;
Float_t chisqvtx, tresidMax;
Float_t NSigmaCut=4.0;
Int_t nhits=0;
Int_t nhitsH=0;
Int_t nhitsS=0;
Int_t nhitsV=0;
Int_t NEvTot=0;
Int_t NEvTot0=0;
const Int_t M_NEVCUTS=16;
Int_t NEvCuts[M_NEVCUTS][3];

const Int_t convergence[2]={0,0};	// Convergence flags for SIMPLEX and MIGRAD (0 - conv, 4 - failed)
TMinuit* eventrecoMinuit=new TMinuit(4);

Int_t IPrintLevel=0; //1=verbose; 0=>'normal', -1=>'quiet'  

struct TBRxChs{
  char cID1[4], cID2[4], cLABID1[2], cLABID2[2], cHV[2], cRxType[9], cFullName[18], cPol, cLocation[3], cdum10[10], cdum11[11];
  int LPF, HPF, iLogCh, iRxno, ichno, iPol;
  float NF, x, y, z, cableDelayNs, HitTime, HitTime0, dtNsCorr, hitweight, dtFitResid, dtResidSum, Vrms, Vrms68, xMC, yMC, zMC, dtcMC, tMC;
  //  string commentstring;
};
TBRxChs Rx[MCH];
Rx[0].Vrms=42.1277;  Rx[1].Vrms=54.7289;  Rx[2].Vrms=64.3113;  Rx[3].Vrms=48.6095;  Rx[4].Vrms=35.9902;  Rx[5].Vrms=54.4125;  Rx[6].Vrms=44.6934;  Rx[7].Vrms=40.437;  Rx[8].Vrms=54.5959;  Rx[9].Vrms=52.4158;  Rx[10].Vrms=67.1405;  Rx[11].Vrms=52.4276;  Rx[12].Vrms=34.7929;  Rx[13].Vrms=52.979;  Rx[14].Vrms=41.4998;  Rx[15].Vrms=43.490;

