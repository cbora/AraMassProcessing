#include "locale.h"
#include <ctime>
using namespace std;

char* datestamp;
float AvgGridVtx[3];
int nCombEvGrid;
#ifdef TUNKA
const float rice=1.0;
#else
const float rice=1.782;
#endif
const float c=2.998e-1; //0.2998 m per nanosecond 
const float cmpns=2.998e-1; 
const double NS2S=1.e-9;
const float km2m=1e3;
bool LDebug=false;

inline bool LSpacePts3D(double xyz[][3]){
  bool Ltest=true;
  for(int i=0; i<4; i++)
    for(int j=i+1; j<4; j++)
      if(xyz[i][0]==xyz[j][0] || xyz[i][1]==xyz[j][1] || xyz[i][2]==xyz[j][2])
	Ltest=false;
  return Ltest;
}

inline float fMAX(float float1, float float2){ if(float1>float2) return float1; else return float2;}
inline float fMIN(float float1, float float2){ if(float1<float2) return float1; else return float2;}

inline int iMAX(int i1, int i2){ if(i1>i2) return i1; else return i2;}
inline int iMIN(int i1, int i2){ if(i1<i2) return i1; else return i2;}

//input in km
inline float ntimexp(float z1, float z2, float dtot){
  const float A=1.78;
  const float B=A*-0.24;
  const float C=16.; //was 16. but must be shifted since units are km
  float zmax=z2;
  float zmin=z1;
  if(zmin>zmax){
    zmin=z2;
    zmax=z1;
  }
  float ctheta;
  if(dtot>0)
    ctheta=fabs(z2-z1)/dtot;
  else
    return 0;
  float ftimexp=dtot/c; //was =0
  if(ctheta!=0){
    if(zmax<0. && zmin<0.)
      ftimexp=(A*(zmax-zmin)+(B/C)*(exp(C*zmax)-exp(C*zmin)))/(c * ctheta);
    if(zmax>=0. && zmin<0.)
      ftimexp=(A*(0.-zmin)+(B/C)*(exp(C*0.)-exp(C*zmin))+zmax)/(c * ctheta);
    if(zmax>=0. && zmin>=0.)
      ftimexp=(zmax-zmin)/(c * ctheta);
    if(zmax<0. && zmin>0.)
      cout<<" CONDITION NOT COVERED IN NTIMEZ!!!"<<endl;
  }
  return ftimexp*km2m; //km->meters
}

inline char* datechar(){
  setlocale (LC_ALL, "");
  time_t tstartrun;
  tstartrun = time(NULL);
  struct tm *tp = localtime(&tstartrun);
  datestamp=(char*)malloc(16);
  strftime(datestamp,16,"%j%H%M%S", tp);
  cout<<"datestamp="<<datestamp<<endl;
  return datestamp;
}

void unixtime2realtime(double gmtUnixTime, int iyrevt, int imoevt, int idayevt, int ihourevt, int iminevt, int isecevt){
  time_t tim=gmtUnixTime;
  tm *gmtm;
  gmtm=localtime(&tim); //ARA time is already gmt!
  iyrevt=gmtm->tm_year+1900;
  imoevt=gmtm->tm_mon+1;
  idayevt=gmtm->tm_mday;
  ihourevt=gmtm->tm_hour;
  iminevt=gmtm->tm_min;
  isecevt=gmtm->tm_sec;
}

inline int signof(float a) { return (a == 0) ? 0 : (a<0 ? -1 : 1); }

inline float T1T2sz(float T1, float x1, float y1, float z1, float T2, float x2,
	     float y2, float z2, float rindex){
  //cable-delay corrected times in ns, distances in meters
  if(T1==0 || T2==0)
    return -9999;
  else{
    float distance=sqrt((x1-x2)*(x1-x2)+(y1-y2)*(y1-y2)+(z1-z2)*(z1-z2));
    float tdelay=T1-T2;
    if(distance==0)
      return -999;
    else
      return (cmpns*tdelay/rindex)/distance; //should have signof coords relative to line joining antennas
    //      return signof(z1-z2)*(c*tdelay/rindex)/distance;
    //      return (c*tdelay/rindex)/distance;                                  
  }
}


inline Float_t Map2pm180deg(Float_t phiAngleInDeg){
  Float_t phiAngleInDeg0=phiAngleInDeg;
  while(phiAngleInDeg0>=180)
    phiAngleInDeg0-=360;
  while(phiAngleInDeg0<-180)
    phiAngleInDeg0+=360;
  return phiAngleInDeg0;
  if(phiAngleInDeg0<-180 || phiAngleInDeg0>180)
    cout<<" phiAngleInDeg0 OOB="<<phiAngleInDeg0;
  //    return phiAngleInDeg-360*phiAngleInDeg/fabs(phiAngleInDeg);                                                   
}

inline Int_t Map2_0_360deg(Float_t phiAngleInDeg){
  Int_t phiAngleInDeg0=int(phiAngleInDeg);
  while(phiAngleInDeg0>=360)
    phiAngleInDeg0-=360;
  while(phiAngleInDeg0<0)
    phiAngleInDeg0+=360;
  return phiAngleInDeg0;
  //    return phiAngleInDeg-360*phiAngleInDeg/fabs(phiAngleInDeg);                                                   
}

inline Int_t Map2_0_180deg_theta(Float_t theAngleInDeg){
  Int_t theAngleInDeg0=int(theAngleInDeg);
  while(theAngleInDeg0<0)
    theAngleInDeg0+=90;
  while(theAngleInDeg0>180)
    theAngleInDeg0=360-theAngleInDeg0;
  return theAngleInDeg0;
}

