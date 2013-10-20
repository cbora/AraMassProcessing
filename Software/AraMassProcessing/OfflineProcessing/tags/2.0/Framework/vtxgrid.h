ofstream GridVtxPm3d("GridVtx.pm3dat");
ofstream GridVtxOut;

const int MAXHITS=20;
int iANTHits[MAXHITS];
float GridResid[MAXHITS], GridResid0[MAXHITS];
double TDRMhit[MAXHITS];
double xyzGrid[MAXHITS][3], xyzGridD[MAXHITS][3];
double TGrid[MAXHITS], TGrids[MAXHITS], VGrid[MAXHITS];
float QGrid[MAXHITS];
float ResidTestPt[MAXHITS];
bool LuseHit[MAXHITS];
int iuseHit[MAXHITS];
double vtxtime[MAXHITS], vtxresid;
int GridVtx[MAXHITS][3];
float ChGridResid[MAXHITS][MAXHITS];
float fracResid[MAXHITS][MAXHITS];
float GridMinResid[MAXHITS];
float vertexfinder_vtx[3];
int nExcludeHits=0;
int Grid0[3]={0,0,0}; //={0,0,-1000};	
int dGrid[3]={50,50,50}; //={2500,2500,1200};
int GridTestPt[3]={0,0,0}; //{300,120,0};
int GridSpacing[3]={2,2,2}; //={40,40,20};
int ichID[MAXHITS];
bool Linit=true;

void VtxCheck(double xyzGrid[][3], float Vtx0[3], double TVtx[4]);
void VtxCheck(double xyzGrid[][3], float Vtx0[3], double TVtx[4]){
  for(int ihit=0; ihit<4; ihit++){
    float D2Vtxsq=0.;
    for(int ijk=0; ijk<3; ijk++)
      D2Vtxsq+=(xyzGrid[ihit][ijk]-Vtx0[ijk])*(xyzGrid[ihit][ijk]-Vtx0[ijk]);
    TVtx[ihit]=sqrt(D2Vtxsq)*1.782/2.998e8;
    //    if(ihit>0)
    //      TVtx[ihit]=TVtx[ihit]-TVtx[0];
  }
}

void InitVtxGrid();
void InitVtxGrid(){
  char GridVtxOutFileName[128];
  sprintf(GridVtxOutFileName,"/net/user/dbesson/GridVtxOut-%s.txt",datestamp);
  GridVtxOut.open(GridVtxOutFileName);
  GridVtxOut<<"iMDAQhit:ichID:Xhit:Yhit:Zhit:GridX:GridY:GridZ:chisqr:TGrid:VGrid:chisqrT"<<endl;
}



void VtxGrid(int irunID, int ievtID, int NCHGrid, int ichID[], int iEvWrite, int GridSpacing[3], int iuseHit[], double TGrid[], double VGrid[], float QGrid[], double xyzGrid[][3], int Grid0[3], int GridTestPt[3], int dGrid[3], int GridVtx[][3], float ChGridResid[][MAXHITS], float fracResid[][MAXHITS], float ResidTestPt[], float GridMinResid[], float vertexfinder_vtx[]);

void VtxGrid(int irunID, int ievtID, int NCHGrid, int ichID[], int iEvWrite, int GridSpacing[3], int iuseHit[], double TGrid[], double VGrid[], float QGrid[], double xyzGrid[][3], int Grid0[3], int GridTestPt[3], int dGrid[3], int GridVtx[][3], float ChGridResid[][MAXHITS], float fracResid[][MAXHITS], float ResidTestPt[], float GridMinResid[], float vertexfinder_vtx[]){
   bool LDebug_pm3d=LDebug&&iEvWrite<2;
   double delTtransitSumSq[MAXHITS];
   float delTtransitMin[MAXHITS]={1.e12};
   //   float GridVtx[MAXHITS][3];
   //sort VGrid and determine maximum amplitude channel:
   //   int iMaxV=-1;
   //   float VMaxV=-1.;
   float TrigT0Grid[MAXHITS];
   double delT0[MAXHITS];
   double delTiV0[MAXHITS], delTnziV[MAXHITS];
   for(int i=0; i<MAXHITS; i++){
     LuseHit[i]=(iuseHit[i]==1);
     delTtransitMin[i]=1.e12;
     GridMinResid[i]=1.e12;
     GridVtx[i][0]=9999;
     GridVtx[i][1]=9999;
     GridVtx[i][2]=9999;
   }
   //to determine T0: measured time = transit_time + trigger_offset
   if(LDebug_pm3d)
     GridVtxPm3d<<"#"<<iEvWrite<<endl;
   float GridResid00[MAXHITS][MAXHITS]={{0.}};
   for(int xg=Grid0[0]-dGrid[0]; xg<Grid0[0]+dGrid[0]; xg+=GridSpacing[0]){
     for(int yg=Grid0[1]-dGrid[1]; yg<Grid0[1]+dGrid[1]; yg+=GridSpacing[1]){
       for(int zg=Grid0[2]-dGrid[2]; zg<Grid0[2]+dGrid[2]; zg+=GridSpacing[2]){
	 for(int iV=0; iV<NCHGrid; iV++){
	   delTtransitSumSq[iV]=0.;
	   double d2source=sqrt((xyzGrid[iV][0]-xg)*(xyzGrid[iV][0]-xg)+
			       (xyzGrid[iV][1]-yg)*(xyzGrid[iV][1]-yg)+
				(xyzGrid[iV][2]-zg)*(xyzGrid[iV][2]-zg));
	   double t2source=d2source*rice/c;
	   double deltX=(TGrid[iV]-TGrid[0]);
	   delTiV0[iV]=t2source - deltX/NS2S;
	   delTnziV[iV]=ntimexp(zg*0.001,xyzGrid[iV][2]*0.001,d2source*0.001) - deltX/NS2S; //ntimexp takes distances in km!
	 }
	 for(int iV=0; iV<NCHGrid; iV++){ 
	   if(LuseHit[iV]){
	     for(int jV=0; jV<NCHGrid; jV++){
	       if(iV!=jV && LuseHit[jV]){
		 delT0[iV]=delTnziV[iV]-delTnziV[jV];
		 GridResid00[iV][jV]=delT0[iV];
		 delTtransitSumSq[iV]+=delT0[iV]*delT0[iV];
	       }
	     }
	     if(fabs(GridTestPt[0]-xg)<GridSpacing[0] && fabs(GridTestPt[1]-yg)<GridSpacing[1] && fabs(GridTestPt[2]-zg)<GridSpacing[2])
	       ResidTestPt[iV]=delTtransitSumSq[iV];
	     if(delTtransitSumSq[iV]<delTtransitMin[iV]){
	       TrigT0Grid[iV]=delTnziV[iV]*NS2S+TGrid[0];
	       delTtransitMin[iV]=delTtransitSumSq[iV];
	       GridMinResid[iV]=delTtransitMin[iV];
	       GridVtx[iV][0]=xg;
	       GridVtx[iV][1]=yg;
	       GridVtx[iV][2]=zg;
	       for(int jV=0; jV<NCHGrid; jV++){
		 ChGridResid[iV][jV]=GridResid00[iV][jV];
		 if(GridMinResid[iV]>0)
		   fracResid[iV][jV]=GridResid00[iV][jV]*GridResid00[iV][jV]/GridMinResid[iV]; //fractional residual contributed by this channel
	       }
	     }
	   }
	 }
       }
       if(LDebug_pm3d){
	 GridVtxPm3d<<xg<<" "<<yg<<" ";
	 for(int iC1=0; iC1<NCHGrid; iC1++)
	   GridVtxPm3d<<delTtransitSumSq[iC1]<<" ";
	 GridVtxPm3d<<endl;
       }
     }
     if(LDebug_pm3d)
       GridVtxPm3d<<endl;
   }
   int NGoodHit=0;
   for(int i=0; i<NCHGrid; i++)
     if(LuseHit[i])
       NGoodHit++;
   GridVtxOut<<irunID<<" "<<ievtID<<" "<<NGoodHit<<" "<<vertexfinder_vtx[0]<<" "<<vertexfinder_vtx[1]<<" "<<vertexfinder_vtx[2]<<endl;
   for(int i=0; i<NCHGrid; i++){
     //     if(i==0)  
     nCombEvGrid++;
     AvgGridVtx[0]+=GridVtx[i][0]; 
     AvgGridVtx[1]+=GridVtx[i][1]; 
     AvgGridVtx[2]+=GridVtx[i][2];
     if(iuseHit[i]==1){
       GridVtxOut<<ichID[i]<<" "<<xyzGrid[i][0]<<" "<<xyzGrid[i][1]<<" "<<xyzGrid[i][2]<<" "<<GridVtx[i][0]<<" "<<GridVtx[i][1]<<" "<<GridVtx[i][2]<<" "<<GridMinResid[i]<<" "<<(TGrid[i]-TGrid[0])/NS2S<<" "<<VGrid[i]<<" "<<QGrid[i]<<" "<<ResidTestPt[i]<<" "<<endl; //30Jun10: Careful! NCHGrid changed to NGoodHit!!!
       //       GridVtxOut<<NCHGrid<<" "<<ichID[i]<<" "<<xyzGrid[i][0]<<" "<<xyzGrid[i][1]<<" "<<xyzGrid[i][2]<<" "<<GridVtx[i][0]<<" "<<GridVtx[i][1]<<" "<<GridVtx[i][2]<<" "<<GridMinResid[i]<<" "<<(TGrid[i]-TGrid[0])/NS2S<<" "<<VGrid[i]<<" "<<QGrid[i]<<" "<<ResidTestPt[i]<<" "<<endl; //30Jun10: Careful! NCHGrid changed to NGoodHit!!!
       cout<<"VtxGrid: Ch "<<i<<" x/y/z vtx="<<GridVtx[i][0]<<"/"<<GridVtx[i][1]<<"/"<<GridVtx[i][2]<<" chsqmin="<<GridMinResid[i]<<endl;
     }
   }
   if(LDebug_pm3d)
     GridVtxPm3d<<endl<<endl;
 } 
