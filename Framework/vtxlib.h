//NEED TO CHECK VX/VY/VZ DZB VS. JEFF'S!

int vertex(int numhits, double refrac, double rlvtx[3], double *a, double *t, int *j, double *volt, double *vrms, double& t01, double& t02, double& confit);

double avgrms;
ofstream zout("zout.txt",ios::app);

int vertexfinder(int numhits, double refrac, double vertx[3], double vtxp[6], double *a, double *t, int *j, double *volt, double *vrms, double& t0vtx, double& tresvtx, double& sumdif, double& conefit){
  cout<<"NChannl="<<NChannl<<endl;
  double antloc[NChannl][3];
  double times[NChannl];
  int ids[NChannl];
  double volts[NChannl];
  double rms[NChannl];
  double cntrln[3];
  double cntrlnmag=0;
  double AngNuVtx=0;
  double direcmag=0;
  double confit;
  const float DMaxCut = 4000.; //max distance cut
  const float delta12Cut = 0.1; //turn off - 07/02/02 - reinstate 08/14
  float Avgx = 9999., Avgy = 9999., Avgz = 9999.;
  float delta12;
  double t01, t02, t0avg;
  double tzero = 0;
  
  t0avg = 0;
  t0vtx = 0;

  tresvtx = 0;
  conefit = 0;
  cntrln[0]=cntrln[1]=cntrln[2]=0;
  stdv1x=0, stdv1y=0, stdv1z=0, stdv2x=0, stdv2y=0, stdv2z=0; //030809
  double avgx=0,avgy=0,avgz=0,avgpx=0,avgpy=0,avgpz=0,avg1x=0,avg1y=0,avg1z=0,avg2x=0,avg2y=0,avg2z=0;
  float avg1xZ=0,avg2xZ=0,avg1yZ=0,avg2yZ=0,avg1zZ=0,avg2zZ=0;
  float avg1xX=0,avg2xX=0,avg1yX=0,avg2yX=0,avg1zX=0,avg2zX=0;
  
  if(numhits>3.5){
    for (int cnt=0; cnt<numhits; cnt++){
      antloc[cnt][0]=*(a+3*cnt+0);
      antloc[cnt][1]=*(a+3*cnt+1);
      antloc[cnt][2]=*(a+3*cnt+2);
      
      cntrln[0]+=antloc[cnt][0];
      cntrln[1]+=antloc[cnt][1];
      cntrln[2]+=antloc[cnt][2];
      
      times[cnt]=*(t+cnt);
      ids[cnt]=*(j+cnt);
      volts[cnt]=*(volt+cnt);
      rms[cnt]=*(vrms+cnt);
    }
    
    if (numhits<4)return 0;
    cntrln[0]/=numhits;
    cntrln[1]/=numhits;
    cntrln[2]/=numhits;
    
    //int combcount=0;
    combcount=0;
    vxPosz=9999;
    vyPosz=9999;
    vzPosz=9999;
    vxNegz=9999;
    vyNegz=9999;
    vzNegz=9999;
    int combpos=0;
    int combneg=0;
    int combcount2=0;
    int nv1=0;
    float Avgxnv1=0.;
    float Avgynv1=0.;
    float Avgznv1=0.;
    int combcountZ=0;
    const int MaxComb=1028; 
    //    MaxComb=512;
    double vxavg=0.;
    double vyavg=0.; 
    double vzavg=0.;
    double vxavgPosz=0.;
    double vyavgPosz=0.; 
    double vzavgPosz=0.;
    double vxavgNegz=0.;
    double vyavgNegz=0.; 
    double vzavgNegz=0.;
    double vxavg2=0.;
    double vyavg2=0.; 
    double vzavg2=0.;
    //    double vxZ, vyZ, vzZ;
    double vertexs[MaxComb][3];
    double v1s[MaxComb][3];
    double v2s[MaxComb][3];
    
    double t0s[MaxComb];
    double directions[MaxComb][3];
    double vtxrms[MaxComb], avgant[3], totaldists=0,antd1,antd2,antd3,antd4;
    
    double antloc0[4][3],times0[4],volts0[4],vrms0[4];
    int js0[4];
    float t1,t2,t3,t4,tminhit;
    float avgx12=0, avgy12=0, avgz12=0;
    int Ndelta12=0;
    float xChannl[NChannl]={0};
    float yChannl[NChannl]={0};
    float zChannl[NChannl]={0};
    float NtotChannl[NChannl]={0};
    
    //loop over all 4-fold permutations:
    int ntotcomb=0;
    for(int ic=0; ic<NChannl; ic++)
      dtimes[ic]=0.;
    for (int cnt1=0; cnt1<numhits; cnt1++){
      t1=times[cnt1];
      for (int cnt2=cnt1+1; cnt2<numhits; cnt2++){
	t2=times[cnt2];
	for (int cnt3=cnt2+1; cnt3<numhits; cnt3++){
	  t3=times[cnt3];
	  for (int cnt4=cnt3+1; cnt4<numhits; cnt4++){
	    t4=times[cnt4];
	    tminhit=t4;
	    if(t1<t2&&t1<t3&&t1<t4)tminhit=t1;
	    if(t2<t1&&t2<t3&&t2<t4)tminhit=t2;
	    if(t3<t1&&t3<t2&&t3<t4)tminhit=t3;
	    
	    //fill local variables for use in vertex;
	    antloc0[0][0]=antloc[cnt1][0];
	    antloc0[0][1]=antloc[cnt1][1];
	    antloc0[0][2]=antloc[cnt1][2];
	    antloc0[1][0]=antloc[cnt2][0];
	    antloc0[1][1]=antloc[cnt2][1];
	    antloc0[1][2]=antloc[cnt2][2];
	    antloc0[2][0]=antloc[cnt3][0];
	    antloc0[2][1]=antloc[cnt3][1];
	    antloc0[2][2]=antloc[cnt3][2];
	    antloc0[3][0]=antloc[cnt4][0];
	    antloc0[3][1]=antloc[cnt4][1];
	    antloc0[3][2]=antloc[cnt4][2];
	    
	    times0[0]=times[cnt1];
	    times0[1]=times[cnt2];
	    times0[2]=times[cnt3];
	    times0[3]=times[cnt4];
	    js0[0]=ids[cnt1];
	    js0[1]=ids[cnt2];
	    js0[2]=ids[cnt3];
	    js0[3]=ids[cnt4];
	    volts0[0]=volt[cnt1];
	    volts0[1]=volt[cnt2];
	    volts0[2]=volt[cnt3];
	    volts0[3]=volt[cnt4];
	    vrms0[0]=rms[cnt1];
	    vrms0[1]=rms[cnt2];
	    vrms0[2]=rms[cnt3];
	    vrms0[3]=rms[cnt4];
	    
	    //average x-coordinate of antennas being used:	  
	    avgant[0]=(antloc0[0][0]+antloc0[1][0]+antloc0[2][0]+antloc0[3][0])/4;
	    avgant[1]=(antloc0[0][1]+antloc0[1][1]+antloc0[2][1]+antloc0[3][1])/4;
	    avgant[2]=(antloc0[0][2]+antloc0[1][2]+antloc0[2][2]+antloc0[3][2])/4;
	    //xyz- deviation between 1st antenna and x-centroid of all 4:
	    antd1=sqrt( (avgant[0]-antloc0[0][0])*(avgant[0]-antloc0[0][0]) + 
			(avgant[1]-antloc0[0][1])*(avgant[1]-antloc0[0][1]) + 
			(avgant[2]-antloc0[0][2])*(avgant[2]-antloc0[0][2]) );
	    
	    antd2=sqrt( (avgant[0]-antloc0[1][0])*(avgant[0]-antloc0[1][0]) + 
			(avgant[1]-antloc0[1][1])*(avgant[1]-antloc0[1][1]) + 
			(avgant[2]-antloc0[1][2])*(avgant[2]-antloc0[1][2]) );
	    
	    antd3=sqrt( (avgant[0]-antloc0[2][0])*(avgant[0]-antloc0[2][0]) + 
			(avgant[1]-antloc0[2][1])*(avgant[1]-antloc0[2][1]) + 
			(avgant[2]-antloc0[2][2])*(avgant[2]-antloc0[2][2]) );
	    
	    antd4=sqrt( (avgant[0]-antloc0[3][0])*(avgant[0]-antloc0[3][0]) + 
			(avgant[1]-antloc0[3][1])*(avgant[1]-antloc0[3][1]) + 
			(avgant[2]-antloc0[3][2])*(avgant[2]-antloc0[3][2]) );
	    
	    //confit is 4-fold fit to cherenkov cone; needs to synthesize multi-hits;
	    
	    vertex(4,refrac,vertx,&antloc0[0][0],&times0[0],&js0[0],&volts0[0],&vrms0[0],t01,t02,confit);
	    
	    Avgx=Avgy=Avgz=0.0001;
	    
	    float d1 = sqrt(pow(double(v1[0]-avgant[0]),double(2)) +
			    pow(double(v1[1]-avgant[1]),double(2)) +
			    pow(double(v1[2]-avgant[2]),double(2)));
	    
	    float d2 = sqrt(pow(double(v2[0]-avgant[0]),double(2)) +
			    pow(double(v2[1]-avgant[1]),double(2)) +
			    pow(double(v2[2]-avgant[2]),double(2)));
	    
	    combcount2++;

	    if(fabs(t01)<=fabs(t02)){
	      vxavg2+=v1[0];
	      vyavg2+=v1[1];
	      vzavg2+=v1[2];

	      if(v1[2]>0){
		vxavgPosz+=v1[0];
		vyavgPosz+=v1[1];
		vzavgPosz+=v1[2];
		combpos++;
	      }
	      else{
		vxavgNegz+=v1[0];
		vyavgNegz+=v1[1];
		vzavgNegz+=v1[2];
		combneg++;
	      }

	    }
	    else{
	      vxavg2+=v2[0];
	      vyavg2+=v2[1];
	      vzavg2+=v2[2];

	      if(v2[2]>0){
		vxavgPosz+=v2[0];
		vyavgPosz+=v2[1];
		vzavgPosz+=v2[2];
		combpos++;
	      }
	      else{
		vxavgNegz+=v2[0];
		vyavgNegz+=v2[1];
		vzavgNegz+=v2[2];
		combneg++;
	      }

	    }
	    if(t01<tminhit && t02>tminhit && t01!=0. && d1<DMaxCut) {
	      
	      float dresid1=0.;
	      for(int j4=0; j4<4; j4++){
		dresid1+=((times0[j4] - t01)*(2.998e8/1.78)) -
		  sqrt(pow(double(v1[0] - antloc0[j4][0]),double(2)) +
		       pow(double(v1[1] - antloc0[j4][1]),double(2)) +
		       pow(double(v1[2] - antloc0[j4][2]),double(2)));
	      }
	      //	    if(dresid1>10.)cout<<"VTXFINDER dresid1/vtx="<<dresid1<<'/'<<v1[0]<<'/'<<v1[1]<<'/'<<v1[2]<<endl;
	      
	      Avgx=v1[0];
	      Avgy=v1[1];
	      Avgz=v1[2];
	      tzero = t01;

	    }
	    if(t01>tminhit && t02<tminhit && t02!=0. && d2<DMaxCut) {
	      Avgx=v2[0];
	      Avgy=v2[1];
	      Avgz=v2[2];
	      tzero = t02;
	      cout<<"Z selected soln. 2: "<<v2[0]<<' '<<v2[1]<<' '<<v2[2]<<endl;

	    }
	    
	    //ambiguous cases - do these help or hurt? (07/02/02)
	    if(t01<tminhit && t02<tminhit && t01!=0. && t02!=0.){
	      
	      delta12 = sqrt(pow(double(v1[0]-v2[0]),double(2))+
			     pow(double(v1[1]-v2[1]),double(2))+
			     pow(double(v1[2]-v2[2]),double(2)))/(d1+d2);
	      
	      if(delta12<delta12Cut && d1<DMaxCut && d2<DMaxCut){
		if(t01==t02){
		  Avgx = (v1[0]+v2[0])/2.;
		  Avgy = (v1[1]+v2[1])/2.;
		  Avgz = (v1[2]+v2[2])/2.;
		  avgx12 += Avgx;
		  avgy12 += Avgy;
		  avgz12 += Avgz;
		  Ndelta12++;
		  tzero = t01;
		}
		if(t01<t02){
		  Avgx=v1[0];
		  Avgy=v1[1];
		  Avgz=v1[2];
		  tzero = t01;
		}
		if(t02<t01){
		  Avgx=v2[0];
		  Avgy=v2[1];
		  Avgz=v2[2];
		  tzero = t02;
		}
	      }
	    }
	    
	    // if((Avgx+0.00001)/(vx+0.00001)-1 > 0.001)cout<<" Avgx/y/z/(Z).ne.vx/vy/vz(JA): "<<Avgx<<' '<<Avgy<<' '<<Avgz<<' '<<vx<<' '<<vy<<' '<<vz<<endl;
	    
	    if(fabs(Avgx*Avgy*Avgz)<1.e-9){
	      if(d1<d2 && d1<DMaxCut){
		nv1++;
		Avgxnv1+=v1[0];
		Avgynv1+=v1[1];
		Avgznv1+=v1[2];
		Avgx = v1[0]; //=vx,vy,vz not so hot...
		Avgy = v1[1];
		Avgz = v1[2];
	      }
	      if(d2<d1 && d2<DMaxCut){
		nv1++;
		Avgxnv1+=v2[0];
		Avgynv1+=v2[1];
		Avgznv1+=v2[2];
		Avgx = v2[0]; //=vx,vy,vz not so hot...
		Avgy = v2[1];
		Avgz = v2[2];
	      }
	    }
	    
	    avg1xX+=v1[0];
	    avg1yX+=v1[1];
	    avg1zX+=v1[2];
	    avg2xX+=v2[0];
	    avg2yX+=v2[1];
	    avg2zX+=v2[2];
	    
	    if(fabs(Avgx*Avgy*Avgz)>1.e-9 && combcount<MaxComb){
	      
	      //weight by rms
	      vtxrms[combcount]= pow(double((antd1+antd2+antd3+antd4)/4),double(2)) ;
	      
	      totaldists+=vtxrms[combcount]; 
	      
	      vxavg += Avgx;
	      vyavg += Avgy;
	      vzavg += Avgz;
	      
	      t0s[combcount] = tzero;
	      t0avg += tzero;
	      
	      vertexs[combcount][0]=Avgx;
	      vertexs[combcount][1]=Avgy;
	      vertexs[combcount][2]=Avgz;
	      
	      v1s[combcount][0]=v1[0];
	      v1s[combcount][1]=v1[1];
	      v1s[combcount][2]=v1[2];
	      v2s[combcount][0]=v2[0];
	      v2s[combcount][1]=v2[1];
	      v2s[combcount][2]=v2[2];
	      
	      avg1xZ+=v1[0];
	      avg1yZ+=v1[1];
	      avg1zZ+=v1[2];
	      avg2xZ+=v2[0];
	      avg2yZ+=v2[1];
	      avg2zZ+=v2[2];
	      
	      directions[combcount][0]=px;
	      directions[combcount][1]=py;
	      directions[combcount][2]=pz;
	      if(combcount<MaxComb)combcount++;
	      if(combcount==MaxComb)cout<<"VTX combcount=MaxComb!"<<endl;
	      combcountZ++;
	      //zout <<" ON Chs/vtx: "<<cnt1<<'/'<<cnt2<<'/'<<cnt3<<'/'<<cnt4<<'/'<<Avgx<<'/'<<Avgy<<'/'<<Avgz<<endl;	    
	      //cout <<" ON Chs/vtx: "<<cnt1<<'/'<<cnt2<<'/'<<cnt3<<'/'<<cnt4<<'/'<<Avgx<<'/'<<Avgy<<'/'<<Avgz<<endl;
	      
	      NtotChannl[(int)ids[cnt1]]++;
	      NtotChannl[(int)ids[cnt2]]++;
	      NtotChannl[(int)ids[cnt3]]++;
	      NtotChannl[(int)ids[cnt4]]++;
	      xChannl[(int)ids[cnt1]]+=Avgx;
	      yChannl[(int)ids[cnt1]]+=Avgy;
	      zChannl[(int)ids[cnt1]]+=Avgz;
	      xChannl[(int)ids[cnt2]]+=Avgx;
	      yChannl[(int)ids[cnt2]]+=Avgy;
	      zChannl[(int)ids[cnt2]]+=Avgz;
	      xChannl[(int)ids[cnt3]]+=Avgx;
	      yChannl[(int)ids[cnt3]]+=Avgy;
	      zChannl[(int)ids[cnt3]]+=Avgz;
	      xChannl[(int)ids[cnt4]]+=Avgx;
	      yChannl[(int)ids[cnt4]]+=Avgy;
	      zChannl[(int)ids[cnt4]]+=Avgz;
	    }
	    conefit+=confit;
	    ntotcomb++;
	  }
	}
      }
    }

    if(combpos>0){
      vxPosz=vxavgPosz/combpos;
      vyPosz=vyavgPosz/combpos;
      vzPosz=vzavgPosz/combpos;
    }
    
    if(combneg>0){
      vxNegz=vxavgNegz/combneg;
      vyNegz=vyavgNegz/combneg;
      vzNegz=vzavgNegz/combneg;
    }
    
    //using all vertices:
    avg1xX/=combcount; //these were ntotcomb, and only filled arrays to combcnt
    avg1yX/=combcount;
    avg1zX/=combcount;
    avg2xX/=combcount;
    avg2yX/=combcount;
    avg2zX/=combcount;
    
    if(combcount==0)return 0;  
    avg1xZ/=combcount;
    avg1yZ/=combcount;
    avg1zZ/=combcount;
    avg2xZ/=combcount;
    avg2yZ/=combcount;
    avg2zZ/=combcount;
    
    t0avg/=combcount;
    
    cout<<"\ncombcount/nv1/Ndelta12: "<<combcount<<'/'<<nv1<<'/'<<Ndelta12<<endl;
    if(Ndelta12>0)cout<<"delta12 vertex:   "<<avgx12/Ndelta12<<'/'<<
		    avgy12/Ndelta12<<'/'<<avgz12/Ndelta12<<endl;
    cout.precision (4);
    if(nv1>0)fprintf(stdout," Avgxnv1/y/z: %f/%f/%f\n",
		     Avgxnv1/nv1,Avgynv1/nv1,Avgznv1/nv1);
    cout<<"vertexfinder: vx1:"<<avg1xZ<<'/'<<avg1yZ<<'/'<<avg1zZ<<endl;
    cout<<"vertexfinder: vx2:"<<avg2xZ<<'/'<<avg2yZ<<'/'<<avg2zZ<<endl;
    
    conefit/=combcount;
    vxavg/=combcount;
    vyavg/=combcount;
    vzavg/=combcount;
    
    if(combcountZ>MaxComb)cout<<"vtxlib combs: "<<combcountZ;
    
    //cout<<"\nvtxfinder: vxavg/vyavg/vzavg:"<<vxavg<<'/'<<vyavg<<'/'<<vzavg<<endl;
    
    for (int acnt1=0; acnt1<combcount; acnt1++){
      
      avgx+=vertexs[acnt1][0]*vtxrms[acnt1]/totaldists;
      avgy+=vertexs[acnt1][1]*vtxrms[acnt1]/totaldists;
      avgz+=vertexs[acnt1][2]*vtxrms[acnt1]/totaldists;
      avgpx+=directions[acnt1][0]*vtxrms[acnt1]/totaldists;
      avgpy+=directions[acnt1][1]*vtxrms[acnt1]/totaldists;
      avgpz+=directions[acnt1][2]*vtxrms[acnt1]/totaldists;
      
      avg1x+=v1s[acnt1][0]*vtxrms[acnt1]/totaldists;
      avg1y+=v1s[acnt1][1]*vtxrms[acnt1]/totaldists;
      avg1z+=v1s[acnt1][2]*vtxrms[acnt1]/totaldists;
      avg2x+=v2s[acnt1][0]*vtxrms[acnt1]/totaldists;
      avg2y+=v2s[acnt1][1]*vtxrms[acnt1]/totaldists;
      avg2z+=v2s[acnt1][2]*vtxrms[acnt1]/totaldists;
    }
    
    double pmag;
    pmag=sqrt(avgpx*avgpx+avgpy*avgpy+avgpz*avgpz);
    avgpx/=pmag;
    avgpy/=pmag;
    avgpz/=pmag;
    
    double stdv1x=0, stdv1y=0, stdv1z=0, stdv2x=0, stdv2y=0, stdv2z=0;
    stdvx=0, stdv1y=0, stdv1z=0, stdv2x=0, stdv2y=0, stdv2z=0;
    for (int scnt1=0; scnt1<combcount; scnt1++){
      
      //calculate standard deviation of this vertex wrt best avgx above;
      
      stdvx+=(vertexs[scnt1][0]-avgx)*(vertexs[scnt1][0]-avgx);
      stdvy+=(vertexs[scnt1][1]-avgy)*(vertexs[scnt1][1]-avgy);
      stdvz+=(vertexs[scnt1][2]-avgz)*(vertexs[scnt1][2]-avgz);
      stdpx+=(directions[scnt1][0]-avgpx)*(directions[scnt1][0]-avgpx);
      stdpy+=(directions[scnt1][1]-avgpy)*(directions[scnt1][1]-avgpy);
      stdpz+=(directions[scnt1][2]-avgpz)*(directions[scnt1][2]-avgpz);
      
      stdv1x+=(v1s[scnt1][0]-avg1x)*(v1s[scnt1][0]-avg1x);
      stdv1y+=(v1s[scnt1][1]-avg1y)*(v1s[scnt1][1]-avg1y);
      stdv1z+=(v1s[scnt1][2]-avg1z)*(v1s[scnt1][2]-avg1z);
      stdv2x+=(v2s[scnt1][0]-avg1x)*(v2s[scnt1][0]-avg1x);
      stdv2y+=(v2s[scnt1][1]-avg1y)*(v2s[scnt1][1]-avg1y);
      stdv2z+=(v2s[scnt1][2]-avg1z)*(v2s[scnt1][2]-avg1z);
      
    }
    stdvx/=combcount;
    stdvy/=combcount;
    stdvz/=combcount;
    stdpx/=combcount;
    stdpy/=combcount;
    stdpz/=combcount;
    
    stdv1x/=combcount;
    stdv1y/=combcount;
    stdv1z/=combcount;
    stdv2x/=combcount;
    stdv2y/=combcount;
    stdv2z/=combcount;
    
    stdvx=sqrt(stdvx);
    stdvy=sqrt(stdvy);
    stdvz=sqrt(stdvz);
    stdpx=sqrt(stdpx);
    stdpy=sqrt(stdpy);
    stdpz=sqrt(stdpz);	
    
    //why filled here to be overwritten later??;
    sumdif = sqrt(stdvx*stdvx + stdvy*stdvy + stdvz*stdvz); 
    
    stdv1x=sqrt(stdv1x);
    stdv1y=sqrt(stdv1y);
    stdv1z=sqrt(stdv1z);
    stdv2x=sqrt(stdv2x);
    stdv2y=sqrt(stdv2y);
    stdv2z=sqrt(stdv2z);
    
    //now have filled standard deviations and averages
    
    v1[0]=avg1x;
    v1[1]=avg1y;
    v1[2]=avg1z;
    v2[0]=avg2x;
    v2[1]=avg2y;
    v2[2]=avg2z;
    
    //int numvtxs=0;
    numvtxs=0;
    int goodvtx[MaxComb];
    //parameter controlling whether we use this hit;
    double stdcoeff=1.6;  //reduced from 3 on 020813 - if too large, smear vtx.
    
    avgrms=0;
    
    //3 sigma cut - note that this cut biases us to 'nearer' vertices since;
    //we tend to suppress more distant vertices, for a gaussian time smearin;
    
    totaldists=0;
    for (int acnt2=0; acnt2<combcount; acnt2++){
      
      float stdxct = stdvx*stdcoeff;
      float stdyct = stdvy*stdcoeff;
      float stdzct = stdvz*stdcoeff;
      
      //accept 95%;
      if(sqrt((vertexs[acnt2][0]-avgx)*(vertexs[acnt2][0]-avgx))>stdxct ||
	 sqrt((vertexs[acnt2][1]-avgy)*(vertexs[acnt2][1]-avgy))>stdyct ||
	 sqrt((vertexs[acnt2][2]-avgz)*(vertexs[acnt2][2]-avgz))>stdzct){
	goodvtx[acnt2]=0;
      }
      else{
	goodvtx[acnt2]=1;
	numvtxs++;
	totaldists+=vtxrms[acnt2];
	avgrms+=sqrt(vtxrms[acnt2]);
      }
    }
    
    cout << "vertices accepted: "<<numvtxs<<" out of: "<<combcount<<endl;
    if(numvtxs==0)return 0;
    avgrms/=numvtxs;
    
    avgx=avgy=avgz=avgpx=avgpy=avgpz=0;
    
    for (int acntZ=0; acntZ<combcount; acntZ++){
      if (goodvtx[acntZ]){
	avgx+=vertexs[acntZ][0]*vtxrms[acntZ]/totaldists;
	avgy+=vertexs[acntZ][1]*vtxrms[acntZ]/totaldists;
	avgz+=vertexs[acntZ][2]*vtxrms[acntZ]/totaldists;
	t0vtx+=t0s[acntZ]*vtxrms[acntZ]/totaldists;
      }
    }
    
    //Z: 010827: only add directions that "look" into array, within +/-1.2 rad
    //of centerline;
    
    float cntrlnx, cntrlny, cntrlnz;
    cntrlnx=cntrln[0]-avgx;
    cntrlny=cntrln[1]-avgy;
    cntrlnz=cntrln[2]-avgz;
    
    cntrlnmag = sqrt(pow(double(cntrlnx),double(2))+pow(double(cntrlny),double(2))+pow(double(cntrlnz),double(2)));
    
    for (int acnt3=0; acnt3<combcount; acnt3++){
      if (goodvtx[acnt3]){
	
	direcmag = sqrt(pow(double(directions[acnt3][0]),double(2)) + 
			pow(double(directions[acnt3][1]),double(2)) +   
			pow(double(directions[acnt3][2]),double(2)));
	
	AngNuVtx = acos((cntrlnx*directions[acnt3][0] +
			 cntrlny*directions[acnt3][1] +
			 cntrlnz*directions[acnt3][2])/(cntrlnmag*direcmag));
	
	if(fabs(AngNuVtx)<1.2){
	  avgpx+=directions[acnt3][0]*vtxrms[acnt3]/totaldists;
	  avgpy+=directions[acnt3][1]*vtxrms[acnt3]/totaldists;
	  avgpz+=directions[acnt3][2]*vtxrms[acnt3]/totaldists;
	}
      }
    }
    
    
    pmag=sqrt(avgpx*avgpx+avgpy*avgpy+avgpz*avgpz)+1.E-9;
    avgpx/=pmag;
    avgpy/=pmag;
    avgpz/=pmag;
    
    float vx2=avgx;
    float vy2=avgy;
    float vz2=avgz;
    
    //  cout<<"filtered vx/vy/vz:"<<avgx<<'/'<<avgy<<'/'<<avgz<<endl;
    float kotkin = 1 + pow(double(cntrlnmag/1000),double(2));
    if(kotkin>2)kotkin=2;
    float r=sqrt(avgx*avgx + avgy*avgy + avgz*avgz);
    float ctheta=avgz/r;
    cout<<"scaled vx/vy/vz/r/cz:"<<avgx*kotkin<<'/'<<avgy*kotkin<<'/'<<avgz*kotkin<<"//"<<r<<'/'<<ctheta<<endl;
    
    vx = avgx;
    vy = avgy;
    vz = avgz;
    
    if(combcount2>0)
      cout<<"Simple avg:"<<vxavg2/combcount2<<'/'<<vyavg2/combcount2<<'/'<<vzavg2/combcount2<<endl;
    
    px=avgpx;
    py=avgpy;
    pz=avgpz;
    for (int scnt2=0; scnt2<combcount; scnt2++){
      if (goodvtx[scnt2]){
	stdvx+=(vertexs[scnt2][0]-avgx)*(vertexs[scnt2][0]-avgx);
	stdvy+=(vertexs[scnt2][1]-avgy)*(vertexs[scnt2][1]-avgy);
	stdvz+=(vertexs[scnt2][2]-avgz)*(vertexs[scnt2][2]-avgz);
	stdpx+=(directions[scnt2][0]-avgpx)*(directions[scnt2][0]-avgpx);
	stdpy+=(directions[scnt2][1]-avgpy)*(directions[scnt2][1]-avgpy);
	stdpz+=(directions[scnt2][2]-avgpz)*(directions[scnt2][2]-avgpz);
      }
    }
    stdvx/=numvtxs;
    stdvy/=numvtxs;
    stdvz/=numvtxs;
    stdpx/=numvtxs;
    stdpy/=numvtxs;
    stdpz/=numvtxs;
    
    stdvx=sqrt(stdvx);
    stdvy=sqrt(stdvy);
    stdvz=sqrt(stdvz);
    stdpx=sqrt(stdpx);
    stdpy=sqrt(stdpy);
    stdpz=sqrt(stdpz);
    
    sumdif = sqrt(stdvx*stdvx + stdvy*stdvy + stdvz*stdvz); 
    
    for (int i=0; i<NChannl; i++){
      dvtxChnl[i][0] = -200;
      dvtxChnl[i][1] = -200;
      dvtxChnl[i][2] = -200;
      if(NtotChannl[i]>0){
	float xch = xChannl[i]/NtotChannl[i];
	float ych = yChannl[i]/NtotChannl[i];
	float zch = zChannl[i]/NtotChannl[i];
	//zout<<"Channel: "<<i<<" dx: "<< vx-xch << " dy: " << vy-ych <<" avgz: "<< vz-zch << endl;
	dvtxChnl[i][0] = vx - xch;
	dvtxChnl[i][1] = vy - ych;
	dvtxChnl[i][2] = vz - zch;
      }
    }
    //determine total time residual, all hits;
    
    tresvtx=0.;
    float TransitTime = 0;
    for(int jhits=0; jhits<numhits; jhits++){
      TransitTime = sqrt(pow(double(vx - antloc[jhits][0]),double(2)) +
			 pow(double(vy - antloc[jhits][1]),double(2)) +
			 pow(double(vz - antloc[jhits][2]),double(2)) )/(2.998e8/1.78);
      dtimes[(int)ids[jhits]]=1.e9*((times[jhits] - t0vtx) - TransitTime);
      tresvtx += fabs((times[jhits] - t0vtx) - TransitTime);
    }
  } //matches numhits check

  vertx[0]=vx;
  vertx[1]=vy;
  vertx[2]=vz;  

  vtxp[0]=avgpx;
  vtxp[1]=avgpy;
  vtxp[2]=avgpz;

  vtxp[3]=cntrlnmag;
  vtxp[4]=AngNuVtx;
  vtxp[5]=direcmag;

  return 0;
}




int vertex(int numhits, double refrac, double rlvtx[3], double *a, double *t, int *j, double *volt, double *vrms, double& t01, double& t02, double& confit)
{
  //#include <fstream.h>
  
#define NDIM	3
#define NORDER	3
#define KCOLUM	1
#define CVAC	2.998E8
#define BETA	1
  
  /*
    #ifdef V16CH
    #define NUMANT 16
    #elif V15CH
    #define NUMANT 15
    #else
    #define NUMANT 20
    #endif
  */
  
  double C;
  
  double VMOD(double vmA[], int vmLength);
  void VSUB(double vA[], double vB[], double* vC, int vLength);
  void QUAD(double a, double b, double c, double& xq1, double& xq2);
  
  void matrxvecmult(double* matrx, double* vect, double* dest);
  int linsystem(double* matrx, double* vect);
  int invert(double* matrx);
  double dotprod(double* a, double* b, int length);
  
  /*
  char *cfgDirName = getenv("RICE_ANALYSIS_CFGDIR");
  char antLocFileName[120];
#ifdef CCONE0
  sprintf(antLocFileName, "%s/antennaloc_debug.dat",cfgDirName);
#elif V1999
  sprintf(antLocFileName, "%s/antennaloc_1999.dat",cfgDirName);
#elif V2000
  sprintf(antLocFileName, "%s/antennaloc_2000.dat",cfgDirName);
#elif V2001
  sprintf(antLocFileName, "%s/antennaloc_2001.dat",cfgDirName);
#elif V2002
  sprintf(antLocFileName, "%s/antennaloc_2002.dat",cfgDirName);
#else
  sprintf(antLocFileName, "%s/antennaloc.dat",cfgDirName);
#endif
  
  ifstream infile(antLocFileName, ios::in);
  infile >> refrac;
  */  

  C=(CVAC/refrac);
  double CTHETC=1/(BETA*refrac);
  
  double a1z[3],a2z[3],a3z[3],a1zmag,a2zmag,a3zmag,amatrx[3][3],bvec[3],cx1mod,cx2mod,cx3mod,cx4mod;
  
  
  double antloc[NChannl][3];
  double times[NChannl];
  int ids[NChannl];
  double volts[NChannl];
  double rms[NChannl];
  
  for (int cnt=0; cnt<numhits; cnt++){
    antloc[cnt][0]=*(a+3*cnt+0);
    antloc[cnt][1]=*(a+3*cnt+1);
    antloc[cnt][2]=*(a+3*cnt+2);
    
    times[cnt]=*(t+cnt);
    ids[cnt]=*(j+cnt);
    volts[cnt]=*(volt+cnt);
    rms[cnt]=*(vrms+cnt);
  }
  
  ////////////////////////////////////////////////////
  //Jeffs Test Calculation
  ////////////////////////////////////////////////////
  double jx12,jx13,jx14,jy12,jy13,jy14,jz12,jz13,jz14,jt12,jt13,jt14,jt1sq,jt2sq,jt3sq,jt4sq,ja1sq,ja2sq,ja3sq,ja4sq,jdt12,jdt13,jdt14;
  double jvtx1[3],jvtx2[3],jvec[3],jdt[3],jtdif[3],jmatrix[3][3],jvecmult[3];
  double jray[3],jtev1[4],jtev2[4];
  double ja,jb,jc;
  double ja12[3],ja13[3],ja14[3];
  
  jx12=antloc[0][0]-antloc[1][0];
  jy12=antloc[0][1]-antloc[1][1];
  jz12=antloc[0][2]-antloc[1][2];
  jx13=antloc[0][0]-antloc[2][0];
  jy13=antloc[0][1]-antloc[2][1];
  jz13=antloc[0][2]-antloc[2][2];
  jx14=antloc[0][0]-antloc[3][0];
  jy14=antloc[0][1]-antloc[3][1];
  jz14=antloc[0][2]-antloc[3][2];
  jt12=times[0]-times[1];
  jt13=times[0]-times[2];
  jt14=times[0]-times[3];
  ja1sq=dotprod(&antloc[0][0],&antloc[0][0],3);
  ja2sq=dotprod(&antloc[1][0],&antloc[1][0],3);
  ja3sq=dotprod(&antloc[2][0],&antloc[2][0],3);
  ja4sq=dotprod(&antloc[3][0],&antloc[3][0],3);
  jt1sq=C*C*times[0]*times[0];
  jt2sq=C*C*times[1]*times[1];
  jt3sq=C*C*times[2]*times[2];
  jt4sq=C*C*times[3]*times[3];
  jdt12=(ja1sq-ja2sq)-(jt1sq-jt2sq);
  jdt13=(ja1sq-ja3sq)-(jt1sq-jt3sq);
  jdt14=(ja1sq-ja4sq)-(jt1sq-jt4sq);
  ja12[0]=jx12;
  ja12[1]=jy12;
  ja12[2]=jz12;
  ja13[0]=jx13;
  ja13[1]=jy13;
  ja13[2]=jz13;
  ja14[0]=jx14;
  ja14[1]=jy14;
  ja14[2]=jz14;
  
  
  jmatrix[0][0]=2*jx12;
  jmatrix[0][1]=2*jy12;
  jmatrix[0][2]=2*jz12;
  jmatrix[1][0]=2*jx13;
  jmatrix[1][1]=2*jy13;
  jmatrix[1][2]=2*jz13;
  jmatrix[2][0]=2*jx14;
  jmatrix[2][1]=2*jy14;
  jmatrix[2][2]=2*jz14;
  
  jdt[0]=jdt12;
  jdt[1]=jdt13;
  jdt[2]=jdt14;
  
  jtdif[0]=jt12;
  jtdif[1]=jt13;
  jtdif[2]=jt14;
  
  jvec[0]=jdt[0];
  jvec[1]=jdt[1];
  jvec[2]=jdt[2];
  
  linsystem(&jmatrix[0][0],&jvec[0]);
  
  jvecmult[0]=jmatrix[0][0]*jtdif[0]+jmatrix[0][1]*jtdif[1]+jmatrix[0][2]*jtdif[2];
  jvecmult[1]=jmatrix[1][0]*jtdif[0]+jmatrix[1][1]*jtdif[1]+jmatrix[1][2]*jtdif[2];
  jvecmult[2]=jmatrix[2][0]*jtdif[0]+jmatrix[2][1]*jtdif[1]+jmatrix[2][2]*jtdif[2];
  
  jray[0]=2*C*C*jvecmult[0];
  jray[1]=2*C*C*jvecmult[1];
  jray[2]=2*C*C*jvecmult[2];
  
  ja=dotprod(&jray[0],&jray[0],3)-C*C;
  jb=2*C*C*times[0]-2*dotprod(&jray[0],&antloc[0][0],3)+2*dotprod(&jray[0],&jvec[0],3);
  jc=dotprod(&jvec[0],&jvec[0],3)-2*dotprod(&jvec[0],&antloc[0][0],3)+dotprod(&antloc[0][0],&antloc[0][0],3)-C*C*times[0]*times[0];
  QUAD(ja,jb,jc,jtev1[0],jtev2[0]);
  
  ja=dotprod(&jray[0],&jray[0],3)-C*C;
  jb=2*C*C*times[1]-2*dotprod(&jray[0],&antloc[1][0],3)+2*dotprod(&jray[0],&jvec[0],3);
  jc=dotprod(&jvec[0],&jvec[0],3)-2*dotprod(&jvec[0],&antloc[1][0],3)+dotprod(&antloc[1][0],&antloc[1][0],3)-C*C*times[1]*times[1];
  QUAD(ja,jb,jc,jtev1[1],jtev2[1]);
  
  ja=dotprod(&jray[0],&jray[0],3)-C*C;
  jb=2*C*C*times[2]-2*dotprod(&jray[0],&antloc[2][0],3)+2*dotprod(&jray[0],&jvec[0],3);
  jc=dotprod(&jvec[0],&jvec[0],3)-2*dotprod(&jvec[0],&antloc[2][0],3)+dotprod(&antloc[2][0],&antloc[2][0],3)-C*C*times[2]*times[2];
  QUAD(ja,jb,jc,jtev1[2],jtev2[2]);
  
  ja=dotprod(&jray[0],&jray[0],3)-C*C;
  jb=2*C*C*times[3]-2*dotprod(&jray[0],&antloc[3][0],3)+2*dotprod(&jray[0],&jvec[0],3);
  jc=dotprod(&jvec[0],&jvec[0],3)-2*dotprod(&jvec[0],&antloc[3][0],3)+dotprod(&antloc[3][0],&antloc[3][0],3)-C*C*times[3]*times[3];
  QUAD(ja,jb,jc,jtev1[3],jtev2[3]);
  
  jvtx1[0]=jvec[0]+jtev1[0]*jray[0];
  jvtx1[1]=jvec[1]+jtev1[0]*jray[1];
  jvtx1[2]=jvec[2]+jtev1[0]*jray[2];
  jvtx2[0]=jvec[0]+jtev2[0]*jray[0];
  jvtx2[1]=jvec[1]+jtev2[0]*jray[1];
  jvtx2[2]=jvec[2]+jtev2[0]*jray[2];
  
  double jraymag;
  jraymag=sqrt(jray[0]*jray[0]+jray[1]*jray[1]+jray[2]*jray[2]);
  
  
/////////////////////////////////////////////////////
// End Jeff Test
////////////////////////////////////////////////////


  v1[0]=jvtx1[0];
  v1[1]=jvtx1[1];
  v1[2]=jvtx1[2];
  v2[0]=jvtx2[0];
  v2[1]=jvtx2[1];
  v2[2]=jvtx2[2];
  vx=v1[0];
  vy=v1[1];
  vz=v1[2];
  
  t01=jtev1[0];
  t02=jtev2[0];
  
  //////////////////////////////////////////////////////////////
  //Direction Calculation
  //////////////////////////////////////////////////////////////
  
  double vf[3];
  vf[0]=vx;
  vf[1]=vy;
  vf[2]=vz;
  
  VSUB(antloc[0],v1,&a1z[0],3);
  VSUB(antloc[1],v1,&a2z[0],3);
  VSUB(antloc[2],v1,&a3z[0],3);
  a1zmag=VMOD(a1z,3);
  a2zmag=VMOD(a2z,3);
  a3zmag=VMOD(a3z,3);
  amatrx[0][0]=a1z[0]/a1zmag;
  amatrx[0][1]=a1z[1]/a1zmag;
  amatrx[0][2]=a1z[2]/a1zmag;
  amatrx[1][0]=a2z[0]/a2zmag;
  amatrx[1][1]=a2z[1]/a2zmag;
  amatrx[1][2]=a2z[2]/a2zmag;
  amatrx[2][0]=a3z[0]/a3zmag;
  amatrx[2][1]=a3z[1]/a3zmag;
  amatrx[2][2]=a3z[2]/a3zmag;
  bvec[0]=bvec[1]=bvec[2]=CTHETC;
  
  
  
  int success=1;
  success=linsystem(&amatrx[0][0],&bvec[0]);
  
  cx1mod=VMOD(bvec,3)+1E-9;
  cx1[0]=bvec[0]/cx1mod;
  cx1[1]=bvec[1]/cx1mod;
  cx1[2]=bvec[2]/cx1mod;
  VSUB(antloc[0],v2,&a1z[0],3);
  VSUB(antloc[1],v2,&a2z[0],3);
  VSUB(antloc[2],v2,&a3z[0],3);
  a1zmag=VMOD(a1z,3);
  a2zmag=VMOD(a2z,3);
  a3zmag=VMOD(a3z,3);
  amatrx[0][0]=a1z[0]/a1zmag;
  amatrx[0][1]=a1z[1]/a1zmag;
  amatrx[0][2]=a1z[2]/a1zmag;
  amatrx[1][0]=a2z[0]/a2zmag;
  amatrx[1][1]=a2z[1]/a2zmag;
  amatrx[1][2]=a2z[2]/a2zmag;
  amatrx[2][0]=a3z[0]/a3zmag;
  amatrx[2][1]=a3z[1]/a3zmag;
  amatrx[2][2]=a3z[2]/a3zmag;
  bvec[0]=bvec[1]=bvec[2]=CTHETC;
  
  
  success=linsystem(&amatrx[0][0],&bvec[0]);
  
  
  cx2mod=VMOD(bvec,3)+1E-9;
  cx2[0]=bvec[0]/cx2mod;
  cx2[1]=bvec[1]/cx2mod;
  cx2[2]=bvec[2]/cx2mod;
  
  
  
  
  
  double axis[4][3];
  double Tc=56*3.1415926/180;
  
  
  
  ///////////////////////////////////////////////////////////////
  //AVERAGE OF V1
  ///////////////////////////////////////////////////////////////
  VSUB(antloc[0],v1,&a1z[0],3);
  VSUB(antloc[1],v1,&a2z[0],3);
  VSUB(antloc[2],v1,&a3z[0],3);
  
  a1zmag=VMOD(a1z,3);
  a2zmag=VMOD(a2z,3);
  a3zmag=VMOD(a3z,3);
  
  amatrx[0][0]=a1z[0]/a1zmag;
  amatrx[0][1]=a1z[1]/a1zmag;
  amatrx[0][2]=a1z[2]/a1zmag;
  amatrx[1][0]=a2z[0]/a2zmag;
  amatrx[1][1]=a2z[1]/a2zmag;
  amatrx[1][2]=a2z[2]/a2zmag;
  amatrx[2][0]=a3z[0]/a3zmag;
  amatrx[2][1]=a3z[1]/a3zmag;
  amatrx[2][2]=a3z[2]/a3zmag;
  bvec[0]=cos(Tc);
  bvec[1]=cos(Tc);
  bvec[2]=cos(Tc);
  
  success=linsystem(&amatrx[0][0],&bvec[0]);
  
  
  cx3mod=VMOD(bvec,3)+1.E-9;
  axis[0][0]=bvec[0]/cx3mod;
  axis[0][1]=bvec[1]/cx3mod;
  axis[0][2]=bvec[2]/cx3mod;
  
  
  VSUB(antloc[0],v1,&a1z[0],3);
  VSUB(antloc[1],v1,&a2z[0],3);
  VSUB(antloc[3],v1,&a3z[0],3);
  
  a1zmag=VMOD(a1z,3);
  a2zmag=VMOD(a2z,3);
  a3zmag=VMOD(a3z,3);
  
  amatrx[0][0]=a1z[0]/a1zmag;
  amatrx[0][1]=a1z[1]/a1zmag;
  amatrx[0][2]=a1z[2]/a1zmag;
  amatrx[1][0]=a2z[0]/a2zmag;
  amatrx[1][1]=a2z[1]/a2zmag;
  amatrx[1][2]=a2z[2]/a2zmag;
  amatrx[2][0]=a3z[0]/a3zmag;
  amatrx[2][1]=a3z[1]/a3zmag;
  amatrx[2][2]=a3z[2]/a3zmag;
  bvec[0]=cos(Tc);
  bvec[1]=cos(Tc);
  bvec[2]=cos(Tc);
  
  success=linsystem(&amatrx[0][0],&bvec[0]);
  
  
  cx3mod=VMOD(bvec,3)+1.E-9;
  axis[1][0]=bvec[0]/cx3mod;
  axis[1][1]=bvec[1]/cx3mod;
  axis[1][2]=bvec[2]/cx3mod;
  
  
  
  
  VSUB(antloc[0],v1,&a1z[0],3);
  VSUB(antloc[3],v1,&a2z[0],3);
  VSUB(antloc[2],v1,&a3z[0],3);
  
  a1zmag=VMOD(a1z,3);
  a2zmag=VMOD(a2z,3);
  a3zmag=VMOD(a3z,3);
  
  amatrx[0][0]=a1z[0]/a1zmag;
  amatrx[0][1]=a1z[1]/a1zmag;
  amatrx[0][2]=a1z[2]/a1zmag;
  amatrx[1][0]=a2z[0]/a2zmag;
  amatrx[1][1]=a2z[1]/a2zmag;
  amatrx[1][2]=a2z[2]/a2zmag;
  amatrx[2][0]=a3z[0]/a3zmag;
  amatrx[2][1]=a3z[1]/a3zmag;
  amatrx[2][2]=a3z[2]/a3zmag;
  bvec[0]=cos(Tc);
  bvec[1]=cos(Tc);
  bvec[2]=cos(Tc);
  
  success=linsystem(&amatrx[0][0],&bvec[0]);
  
  
  cx3mod=VMOD(bvec,3)+1.E-9;
  axis[2][0]=bvec[0]/cx3mod;
  axis[2][1]=bvec[1]/cx3mod;
  axis[2][2]=bvec[2]/cx3mod;

  VSUB(antloc[3],v1,&a1z[0],3);
  VSUB(antloc[1],v1,&a2z[0],3);
  VSUB(antloc[2],v1,&a3z[0],3);
  
  a1zmag=VMOD(a1z,3);
  a2zmag=VMOD(a2z,3);
  a3zmag=VMOD(a3z,3);
  
  amatrx[0][0]=a1z[0]/a1zmag;
  amatrx[0][1]=a1z[1]/a1zmag;
  amatrx[0][2]=a1z[2]/a1zmag;
  amatrx[1][0]=a2z[0]/a2zmag;
  amatrx[1][1]=a2z[1]/a2zmag;
  amatrx[1][2]=a2z[2]/a2zmag;
  amatrx[2][0]=a3z[0]/a3zmag;
  amatrx[2][1]=a3z[1]/a3zmag;
  amatrx[2][2]=a3z[2]/a3zmag;
  bvec[0]=cos(Tc);
  bvec[1]=cos(Tc);
  bvec[2]=cos(Tc);
  
  success=linsystem(&amatrx[0][0],&bvec[0]);
    
  cx3mod=VMOD(bvec,3)+1.E-9;
  if(cx3mod>0){
    axis[3][0]=bvec[0]/cx3mod;
    axis[3][1]=bvec[1]/cx3mod;
    axis[3][2]=bvec[2]/cx3mod;
  }
  else
    cout<<"cx3mod="<<cx3mod<<endl;
  
  cx3[0]=(axis[0][0]+axis[1][0]+axis[2][0]+axis[3][0])/4;
  cx3[1]=(axis[0][1]+axis[1][1]+axis[2][1]+axis[3][1])/4;
  cx3[2]=(axis[0][2]+axis[1][2]+axis[2][2]+axis[3][2])/4;
  //  cx3[0]=cx3[0]/sqrt(cx3[0]*cx3[0]+cx3[1]*cx3[1]+cx3[2]*cx3[2]); //gives divide by zero 3mar11
  //  cx3[1]=cx3[1]/sqrt(cx3[0]*cx3[0]+cx3[1]*cx3[1]+cx3[2]*cx3[2]);
  //  cx3[2]=cx3[2]/sqrt(cx3[0]*cx3[0]+cx3[1]*cx3[1]+cx3[2]*cx3[2]); 
  //////////////////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////////////////
  
  
  
  ///////////////////////////////////////////////////////////////
  //AVERAGE OF V2
  ///////////////////////////////////////////////////////////////
  VSUB(antloc[0],v2,&a1z[0],3);
  VSUB(antloc[1],v2,&a2z[0],3);
  VSUB(antloc[2],v2,&a3z[0],3);
  
  a1zmag=VMOD(a1z,3);
  a2zmag=VMOD(a2z,3);
  a3zmag=VMOD(a3z,3);
  
  amatrx[0][0]=a1z[0]/a1zmag;
  amatrx[0][1]=a1z[1]/a1zmag;
  amatrx[0][2]=a1z[2]/a1zmag;
  amatrx[1][0]=a2z[0]/a2zmag;
  amatrx[1][1]=a2z[1]/a2zmag;
  amatrx[1][2]=a2z[2]/a2zmag;
  amatrx[2][0]=a3z[0]/a3zmag;
  amatrx[2][1]=a3z[1]/a3zmag;
  amatrx[2][2]=a3z[2]/a3zmag;
  bvec[0]=cos(Tc);
  bvec[1]=cos(Tc);
  bvec[2]=cos(Tc);
  
  success=linsystem(&amatrx[0][0],&bvec[0]);
  
  
  cx4mod=VMOD(bvec,3)+1.E-9;
  axis[0][0]=bvec[0]/cx4mod;
  axis[0][1]=bvec[1]/cx4mod;
  axis[0][2]=bvec[2]/cx4mod;
  
  
  VSUB(antloc[0],v2,&a1z[0],3);
  VSUB(antloc[1],v2,&a2z[0],3);
  VSUB(antloc[3],v2,&a3z[0],3);
  
  a1zmag=VMOD(a1z,3);
  a2zmag=VMOD(a2z,3);
  a3zmag=VMOD(a3z,3);
  
  amatrx[0][0]=a1z[0]/a1zmag;
  amatrx[0][1]=a1z[1]/a1zmag;
  amatrx[0][2]=a1z[2]/a1zmag;
  amatrx[1][0]=a2z[0]/a2zmag;
  amatrx[1][1]=a2z[1]/a2zmag;
  amatrx[1][2]=a2z[2]/a2zmag;
  amatrx[2][0]=a3z[0]/a3zmag;
  amatrx[2][1]=a3z[1]/a3zmag;
  amatrx[2][2]=a3z[2]/a3zmag;
  bvec[0]=cos(Tc);
  bvec[1]=cos(Tc);
  bvec[2]=cos(Tc);
  
  success=linsystem(&amatrx[0][0],&bvec[0]);
  
  
  cx4mod=VMOD(bvec,3)+1.E-9;
  axis[1][0]=bvec[0]/cx4mod;
  axis[1][1]=bvec[1]/cx4mod;
  axis[1][2]=bvec[2]/cx4mod;

  VSUB(antloc[0],v2,&a1z[0],3);
  VSUB(antloc[3],v2,&a2z[0],3);
  VSUB(antloc[2],v2,&a3z[0],3);
  
  a1zmag=VMOD(a1z,3);
  a2zmag=VMOD(a2z,3);
  a3zmag=VMOD(a3z,3);
  
  amatrx[0][0]=a1z[0]/a1zmag;
  amatrx[0][1]=a1z[1]/a1zmag;
  amatrx[0][2]=a1z[2]/a1zmag;
  amatrx[1][0]=a2z[0]/a2zmag;
  amatrx[1][1]=a2z[1]/a2zmag;
  amatrx[1][2]=a2z[2]/a2zmag;
  amatrx[2][0]=a3z[0]/a3zmag;
  amatrx[2][1]=a3z[1]/a3zmag;
  amatrx[2][2]=a3z[2]/a3zmag;
  bvec[0]=cos(Tc);
  bvec[1]=cos(Tc);
  bvec[2]=cos(Tc);
  
  success=linsystem(&amatrx[0][0],&bvec[0]);
  
  
  cx4mod=VMOD(bvec,3)+1.E-9;
  axis[2][0]=bvec[0]/cx4mod;
  axis[2][1]=bvec[1]/cx4mod;
  axis[2][2]=bvec[2]/cx4mod;
  
  
  
  VSUB(antloc[3],v2,&a1z[0],3);
  VSUB(antloc[1],v2,&a2z[0],3);
  VSUB(antloc[2],v2,&a3z[0],3);
  
  a1zmag=VMOD(a1z,3);
  a2zmag=VMOD(a2z,3);
  a3zmag=VMOD(a3z,3);
  
  amatrx[0][0]=a1z[0]/a1zmag;
  amatrx[0][1]=a1z[1]/a1zmag;
  amatrx[0][2]=a1z[2]/a1zmag;
  amatrx[1][0]=a2z[0]/a2zmag;
  amatrx[1][1]=a2z[1]/a2zmag;
  amatrx[1][2]=a2z[2]/a2zmag;
  amatrx[2][0]=a3z[0]/a3zmag;
  amatrx[2][1]=a3z[1]/a3zmag;
  amatrx[2][2]=a3z[2]/a3zmag;
  bvec[0]=cos(Tc);
  bvec[1]=cos(Tc);
  bvec[2]=cos(Tc);
  
  success=linsystem(&amatrx[0][0],&bvec[0]);
  
  
  cx4mod=VMOD(bvec,3)+1.E-9;
  if(cx4mod==0)
    cout<<"cx4mod==0!"<<endl;
  else{
    axis[3][0]=bvec[0]/cx4mod;
    axis[3][1]=bvec[1]/cx4mod;
    axis[3][2]=bvec[2]/cx4mod;
  }
  
  cx4[0]=(axis[0][0]+axis[1][0]+axis[2][0]+axis[3][0])/4;
  cx4[1]=(axis[0][1]+axis[1][1]+axis[2][1]+axis[3][1])/4;
  cx4[2]=(axis[0][2]+axis[1][2]+axis[2][2]+axis[3][2])/4;
  float ftemp=sqrt(cx4[0]*cx4[0]+cx4[1]*cx4[1]+cx4[2]*cx4[2]);
  if(ftemp==0)
    cout<<"ftemp==0!"<<endl;
  else{
    cx4[0]=cx4[0]/sqrt(cx4[0]*cx4[0]+cx4[1]*cx4[1]+cx4[2]*cx4[2]);
    cx4[1]=cx4[1]/sqrt(cx4[0]*cx4[0]+cx4[1]*cx4[1]+cx4[2]*cx4[2]);
    cx4[2]=cx4[2]/sqrt(cx4[0]*cx4[0]+cx4[1]*cx4[1]+cx4[2]*cx4[2]);
    //////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////
  }
  
  
  
  double R1[3],R2[3],R3[3],R4[3],r1,r2,r3,r4;
  VSUB(antloc[0],vf,R1,3);
  VSUB(antloc[1],vf,R2,3);
  VSUB(antloc[2],vf,R3,3);
  VSUB(antloc[3],vf,R4,3);
  r1=VMOD(R1,3);
  r2=VMOD(R2,3);
  r3=VMOD(R3,3);
  r4=VMOD(R4,3);
  
  if (r1==0) r1=.001;
  if (r2==0) r2=.001;
  if (r3==0) r3=.001;
  if (r4==0) r4=.001;
  if (volts[0]==0) volts[0]=.001;
  if (volts[1]==0) volts[1]=.001;
  if (volts[2]==0) volts[2]=.001;
  if (volts[3]==0) volts[3]=.001;
  if (rms[0]==0) rms[0]=.001;
  if (rms[1]==0) rms[1]=.001;
  if (rms[2]==0) rms[2]=.001;
  if (rms[3]==0) rms[3]=.001;
  
  confit=((volts[1]/rms[1])/(volts[0]/rms[0])-r2/r1)*((volts[1]/rms[1])/(volts[0]/rms[0])-r2/r1) + ((volts[2]/rms[2])/(volts[0]/rms[0])-r3/r1)*((volts[2]/rms[2])/(volts[0]/rms[0])-r3/r1) + ((volts[3]/rms[3])/(volts[0]/rms[0])-r4/r1)*((volts[3]/rms[3])/(volts[0]/rms[0])-r4/r1);
	
  double error1,error2;
  
  if (rlvtx[0]==999 && rlvtx[1]==999 && rlvtx[2]==999){
    error1=(jray[0]/jraymag-cx3[0])*(jray[0]/jraymag-cx3[0])+(jray[1]/jraymag-cx3[1])*(jray[1]/jraymag-cx3[1])+(jray[2]/jraymag-cx3[2])*(jray[2]/jraymag-cx3[2]);
    error2=(jray[0]/jraymag-cx4[0])*(jray[0]/jraymag-cx4[0])+(jray[1]/jraymag-cx4[1])*(jray[1]/jraymag-cx4[1])+(jray[2]/jraymag-cx4[2])*(jray[2]/jraymag-cx4[2]);
  }
  else{
    error1=(rlvtx[0]-v1[0])*(rlvtx[0]-v1[0])+(rlvtx[1]-v1[1])*(rlvtx[1]-v1[1])+(rlvtx[2]-v1[2])*(rlvtx[2]-v1[2]);
    error2=(rlvtx[0]-v2[0])*(rlvtx[0]-v2[0])+(rlvtx[1]-v2[1])*(rlvtx[1]-v2[1])+(rlvtx[2]-v2[2])*(rlvtx[2]-v2[2]);
  }
  
  if (error1<error2){
    vx=v1[0];
    vy=v1[1];
    vz=v1[2];
    px=cx3[0];
    py=cx3[1];
    pz=cx3[2];
  }
  else if (error1>=error2){
    vx=v2[0];
    vy=v2[1];
    vz=v2[2];
    px=cx4[0];
    py=cx4[1];
    pz=cx4[2];
  }
  
  
  return 1;
}

/*
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
//5 HIT TEST
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////

int vertex5(int numhits, double *a, double *t, double *j, double *volt, double *vrms, double& t01, double& t02, double& confit)
{
	#include <fstream.h>
	
	#define NDIM	3
	#define NORDER	3
	#define KCOLUM	1
	#define CVAC	2.998E8
	#define BETA	1
#ifdef V16CH
#define NUMANT 16
#elif V15CH
#define NUMANT 15
#else
#define NUMANT 20
#endif

	double refrac, C;
	
	double VMOD(double vmA[], int vmLength);
	void VSUB(double vA[], double vB[], double* vC, int vLength);
	void QUAD(double a, double b, double c, double& xq1, double& xq2);

	void matrxvecmult(double* matrx, double* vect, double* dest);
	int linsystem(double* matrx, double* vect);
	int invert(double* matrx);
	double dotprod(double* a, double* b, int length);


	ifstream infile("antdata.txt");
	infile >> refrac;
		
	C=(CVAC/refrac);
	double CTHETC=1/(BETA*refrac);

	double a1z[3],a2z[3],a3z[3],a1zmag,a2zmag,a3zmag,amatrx[3][3],bvec[3],cx1mod,cx2mod,cx3mod,cx4mod;

	
	double antloc[NChannl][3];
	double times[NChannl];
	int ids[NChannl];
	double volts[NChannl];
	double rms[NChannl];

	for (int cnt=0; cnt<numhits; cnt++)
	{
		antloc[cnt][0]=*(a+3*cnt+0);
		antloc[cnt][1]=*(a+3*cnt+1);
		antloc[cnt][2]=*(a+3*cnt+2);

		times[cnt]=*(t+cnt);
		ids[cnt]=*(j+cnt);
		volts[cnt]=*(volt+cnt);
		rms[cnt]=*(vrms+cnt);
	}
	
////////////////////////////////////////////////////
//Jeffs Test Calculation
////////////////////////////////////////////////////
	double jx12,jx13,jx14,jy12,jy13,jy14,jz12,jz13,jz14,jt12,jt13,jt14,jt15,jt1sq,jt2sq,jt3sq,jt4sq,jt5sq,ja1sq,ja2sq,ja3sq,ja4sq,ja5sq,jdt12,jdt13,jdt14,jdt15;
	double jvtx1[3],jvtx2[3],jvec[3],jdt[3],jtdif[3],jmatrix[3][3],jvecmult[3];
	double jray[3],jtev,jtev1[4],jtev2[4];
	double ja,jb,jc;
	double ja12[3],ja13[3],ja14[3];

	jx12=antloc[0][0]-antloc[1][0];
	jy12=antloc[0][1]-antloc[1][1];
	jz12=antloc[0][2]-antloc[1][2];
	jx13=antloc[0][0]-antloc[2][0];
	jy13=antloc[0][1]-antloc[2][1];
	jz13=antloc[0][2]-antloc[2][2];
	jx14=antloc[0][0]-antloc[3][0];
	jy14=antloc[0][1]-antloc[3][1];
	jz14=antloc[0][2]-antloc[3][2];
	jt12=times[0]-times[1];
	jt13=times[0]-times[2];
	jt14=times[0]-times[3];
	jt15=times[0]-times[4];
	ja1sq=dotprod(&antloc[0][0],&antloc[0][0],3);
	ja2sq=dotprod(&antloc[1][0],&antloc[1][0],3);
	ja3sq=dotprod(&antloc[2][0],&antloc[2][0],3);
	ja4sq=dotprod(&antloc[3][0],&antloc[3][0],3);
	ja5sq=dotprod(&antloc[4][0],&antloc[4][0],3);
	jt1sq=C*C*times[0]*times[0];
	jt2sq=C*C*times[1]*times[1];
	jt3sq=C*C*times[2]*times[2];
	jt4sq=C*C*times[3]*times[3];
	jt5sq=C*C*times[4]*times[4];
	jdt12=(ja1sq-ja2sq)-(jt1sq-jt2sq);
	jdt13=(ja1sq-ja3sq)-(jt1sq-jt3sq);
	jdt14=(ja1sq-ja4sq)-(jt1sq-jt4sq);
	jdt15=(ja1sq-ja5sq)-(jt1sq-jt5sq);
	ja12[0]=jx12;
	ja12[1]=jy12;
	ja12[2]=jz12;
	ja13[0]=jx13;
	ja13[1]=jy13;
	ja13[2]=jz13;
	ja14[0]=jx14;
	ja14[1]=jy14;
	ja14[2]=jz14;


	jmatrix[0][0]=2*jx12;
	jmatrix[0][1]=2*jy12;
	jmatrix[0][2]=2*jz12;
	jmatrix[1][0]=2*jx13;
	jmatrix[1][1]=2*jy13;
	jmatrix[1][2]=2*jz13;
	jmatrix[2][0]=2*jx14;
	jmatrix[2][1]=2*jy14;
	jmatrix[2][2]=2*jz14;
	
	jdt[0]=jdt12;
	jdt[1]=jdt13;
	jdt[2]=jdt14;

	jtdif[0]=jt12;
	jtdif[1]=jt13;
	jtdif[2]=jt14;

	jvec[0]=jdt[0];
	jvec[1]=jdt[1];
	jvec[2]=jdt[2];

	linsystem(&jmatrix[0][0],&jvec[0]);

	jvecmult[0]=jmatrix[0][0]*jtdif[0]+jmatrix[0][1]*jtdif[1]+jmatrix[0][2]*jtdif[2];
	jvecmult[1]=jmatrix[1][0]*jtdif[0]+jmatrix[1][1]*jtdif[1]+jmatrix[1][2]*jtdif[2];
	jvecmult[2]=jmatrix[2][0]*jtdif[0]+jmatrix[2][1]*jtdif[1]+jmatrix[2][2]*jtdif[2];

	jray[0]=2*C*C*jvecmult[0];
	jray[1]=2*C*C*jvecmult[1];
	jray[2]=2*C*C*jvecmult[2];
	
	ja=dotprod(&jray[0],&jray[0],3)-C*C;
	jb=2*C*C*times[0]-2*dotprod(&jray[0],&antloc[0][0],3)+2*dotprod(&jray[0],&jvec[0],3);
	jc=dotprod(&jvec[0],&jvec[0],3)-2*dotprod(&jvec[0],&antloc[0][0],3)+dotprod(&antloc[0][0],&antloc[0][0],3)-C*C*times[0]*times[0];
	QUAD(ja,jb,jc,jtev1[0],jtev2[0]);
	
	ja=dotprod(&jray[0],&jray[0],3)-C*C;
	jb=2*C*C*times[1]-2*dotprod(&jray[0],&antloc[1][0],3)+2*dotprod(&jray[0],&jvec[0],3);
	jc=dotprod(&jvec[0],&jvec[0],3)-2*dotprod(&jvec[0],&antloc[1][0],3)+dotprod(&antloc[1][0],&antloc[1][0],3)-C*C*times[1]*times[1];
	QUAD(ja,jb,jc,jtev1[1],jtev2[1]);

	ja=dotprod(&jray[0],&jray[0],3)-C*C;
	jb=2*C*C*times[2]-2*dotprod(&jray[0],&antloc[2][0],3)+2*dotprod(&jray[0],&jvec[0],3);
	jc=dotprod(&jvec[0],&jvec[0],3)-2*dotprod(&jvec[0],&antloc[2][0],3)+dotprod(&antloc[2][0],&antloc[2][0],3)-C*C*times[2]*times[2];
	QUAD(ja,jb,jc,jtev1[2],jtev2[2]);

	ja=dotprod(&jray[0],&jray[0],3)-C*C;
	jb=2*C*C*times[3]-2*dotprod(&jray[0],&antloc[3][0],3)+2*dotprod(&jray[0],&jvec[0],3);
	jc=dotprod(&jvec[0],&jvec[0],3)-2*dotprod(&jvec[0],&antloc[3][0],3)+dotprod(&antloc[3][0],&antloc[3][0],3)-C*C*times[3]*times[3];
	QUAD(ja,jb,jc,jtev1[3],jtev2[3]);

	jvtx1[0]=jvec[0]+jtev1[0]*jray[0];
	jvtx1[1]=jvec[1]+jtev1[0]*jray[1];
	jvtx1[2]=jvec[2]+jtev1[0]*jray[2];
	jvtx2[0]=jvec[0]+jtev2[0]*jray[0];
	jvtx2[1]=jvec[1]+jtev2[0]*jray[1];
	jvtx2[2]=jvec[2]+jtev2[0]*jray[2];

	double jraymag;
	jraymag=sqrt(jray[0]*jray[0]+jray[1]*jray[1]+jray[2]*jray[2]);

	double a15[3],jvda15,jrda15;

	a15[0]=antloc[0][0]-antloc[4][0];
	a15[1]=antloc[0][1]-antloc[4][1];
	a15[2]=antloc[0][2]-antloc[4][2];

	jvda15=dotprod(&jvec[0],&a15[0],3);
	jrda15=dotprod(&jray[0],&a15[0],3);

	jtev=(-2*jvda15+jdt15)/(2*jrda15-2*C*C*jt15);



	ofstream jout("jout.txt");
	jout << "JRAY: " << jray[0] << "\t" << jray[1] << "\t" << jray[2] << endl;
	jout << "JVEC: " << jvec[0] << "\t" << jvec[1] << "\t" << jvec[2] << endl;
	jout << "JVX1: " << jvtx1[0] << "\t" << jvtx1[1] << "\t" << jvtx1[2] << endl;
	jout << "JVX2: " << jvtx2[0] << "\t" << jvtx2[1] << "\t" << jvtx2[2] << endl;
	jout << "TEV : " << jtev  << endl;
	jout << "TEV1: " << *jtev1 << endl;
	jout << "TEV2: " << *jtev2 << endl;






/////////////////////////////////////////////////////
// End Jeff Test
////////////////////////////////////////////////////


	v1[0]=jvtx1[0];
	v1[1]=jvtx1[1];
	v1[2]=jvtx1[2];
	v2[0]=jvtx2[0];
	v2[1]=jvtx2[1];
	v2[2]=jvtx2[2];
	vx=v1[0];
	vy=v1[1];
	vz=v1[2];

	t01=jtev1[0];
	t02=jtev2[0];

//////////////////////////////////////////////////////////////
//Direction Calculation
//////////////////////////////////////////////////////////////

	double vf[3];
	vf[0]=vx;
	vf[1]=vy;
	vf[2]=vz;

	VSUB(antloc[0],v1,&a1z[0],3);
	VSUB(antloc[1],v1,&a2z[0],3);
	VSUB(antloc[2],v1,&a3z[0],3);
	a1zmag=VMOD(a1z,3);
	a2zmag=VMOD(a2z,3);
	a3zmag=VMOD(a3z,3);
	amatrx[0][0]=a1z[0]/a1zmag;
	amatrx[0][1]=a1z[1]/a1zmag;
	amatrx[0][2]=a1z[2]/a1zmag;
	amatrx[1][0]=a2z[0]/a2zmag;
	amatrx[1][1]=a2z[1]/a2zmag;
	amatrx[1][2]=a2z[2]/a2zmag;
	amatrx[2][0]=a3z[0]/a3zmag;
	amatrx[2][1]=a3z[1]/a3zmag;
	amatrx[2][2]=a3z[2]/a3zmag;
	bvec[0]=bvec[1]=bvec[2]=CTHETC;
	

	
	int success=1;
	success=linsystem(&amatrx[0][0],&bvec[0]);

	cx1mod=VMOD(bvec,3)+1E-9;
	cx1[0]=bvec[0]/cx1mod;
	cx1[1]=bvec[1]/cx1mod;
	cx1[2]=bvec[2]/cx1mod;
	VSUB(antloc[0],v2,&a1z[0],3);
	VSUB(antloc[1],v2,&a2z[0],3);
	VSUB(antloc[2],v2,&a3z[0],3);
	a1zmag=VMOD(a1z,3);
	a2zmag=VMOD(a2z,3);
	a3zmag=VMOD(a3z,3);
	amatrx[0][0]=a1z[0]/a1zmag;
	amatrx[0][1]=a1z[1]/a1zmag;
	amatrx[0][2]=a1z[2]/a1zmag;
	amatrx[1][0]=a2z[0]/a2zmag;
	amatrx[1][1]=a2z[1]/a2zmag;
	amatrx[1][2]=a2z[2]/a2zmag;
	amatrx[2][0]=a3z[0]/a3zmag;
	amatrx[2][1]=a3z[1]/a3zmag;
	amatrx[2][2]=a3z[2]/a3zmag;
	bvec[0]=bvec[1]=bvec[2]=CTHETC;
	
	
	success=linsystem(&amatrx[0][0],&bvec[0]);


	cx2mod=VMOD(bvec,3)+1E-9;
	cx2[0]=bvec[0]/cx2mod;
	cx2[1]=bvec[1]/cx2mod;
	cx2[2]=bvec[2]/cx2mod;





	double axis[4][3];
	double Tc=56*3.1415926/180;

	

///////////////////////////////////////////////////////////////
//AVERAGE OF V1
///////////////////////////////////////////////////////////////
	VSUB(antloc[0],v1,&a1z[0],3);
	VSUB(antloc[1],v1,&a2z[0],3);
	VSUB(antloc[2],v1,&a3z[0],3);

	a1zmag=VMOD(a1z,3);
	a2zmag=VMOD(a2z,3);
	a3zmag=VMOD(a3z,3);

	amatrx[0][0]=a1z[0]/a1zmag;
	amatrx[0][1]=a1z[1]/a1zmag;
	amatrx[0][2]=a1z[2]/a1zmag;
	amatrx[1][0]=a2z[0]/a2zmag;
	amatrx[1][1]=a2z[1]/a2zmag;
	amatrx[1][2]=a2z[2]/a2zmag;
	amatrx[2][0]=a3z[0]/a3zmag;
	amatrx[2][1]=a3z[1]/a3zmag;
	amatrx[2][2]=a3z[2]/a3zmag;
	bvec[0]=cos(Tc);
	bvec[1]=cos(Tc);
	bvec[2]=cos(Tc);

	success=linsystem(&amatrx[0][0],&bvec[0]);


	cx3mod=VMOD(bvec,3)+1.E-9;
	axis[0][0]=bvec[0]/cx3mod;
	axis[0][1]=bvec[1]/cx3mod;
	axis[0][2]=bvec[2]/cx3mod;
	





	VSUB(antloc[0],v1,&a1z[0],3);
	VSUB(antloc[1],v1,&a2z[0],3);
	VSUB(antloc[3],v1,&a3z[0],3);

	a1zmag=VMOD(a1z,3);
	a2zmag=VMOD(a2z,3);
	a3zmag=VMOD(a3z,3);

	amatrx[0][0]=a1z[0]/a1zmag;
	amatrx[0][1]=a1z[1]/a1zmag;
	amatrx[0][2]=a1z[2]/a1zmag;
	amatrx[1][0]=a2z[0]/a2zmag;
	amatrx[1][1]=a2z[1]/a2zmag;
	amatrx[1][2]=a2z[2]/a2zmag;
	amatrx[2][0]=a3z[0]/a3zmag;
	amatrx[2][1]=a3z[1]/a3zmag;
	amatrx[2][2]=a3z[2]/a3zmag;
	bvec[0]=cos(Tc);
	bvec[1]=cos(Tc);
	bvec[2]=cos(Tc);

	success=linsystem(&amatrx[0][0],&bvec[0]);


	cx3mod=VMOD(bvec,3)+1.E-9;
	axis[1][0]=bvec[0]/cx3mod;
	axis[1][1]=bvec[1]/cx3mod;
	axis[1][2]=bvec[2]/cx3mod;




	VSUB(antloc[0],v1,&a1z[0],3);
	VSUB(antloc[3],v1,&a2z[0],3);
	VSUB(antloc[2],v1,&a3z[0],3);

	a1zmag=VMOD(a1z,3);
	a2zmag=VMOD(a2z,3);
	a3zmag=VMOD(a3z,3);

	amatrx[0][0]=a1z[0]/a1zmag;
	amatrx[0][1]=a1z[1]/a1zmag;
	amatrx[0][2]=a1z[2]/a1zmag;
	amatrx[1][0]=a2z[0]/a2zmag;
	amatrx[1][1]=a2z[1]/a2zmag;
	amatrx[1][2]=a2z[2]/a2zmag;
	amatrx[2][0]=a3z[0]/a3zmag;
	amatrx[2][1]=a3z[1]/a3zmag;
	amatrx[2][2]=a3z[2]/a3zmag;
	bvec[0]=cos(Tc);
	bvec[1]=cos(Tc);
	bvec[2]=cos(Tc);

	success=linsystem(&amatrx[0][0],&bvec[0]);


	cx3mod=VMOD(bvec,3)+1.E-9;
	axis[2][0]=bvec[0]/cx3mod;
	axis[2][1]=bvec[1]/cx3mod;
	axis[2][2]=bvec[2]/cx3mod;



	VSUB(antloc[3],v1,&a1z[0],3);
	VSUB(antloc[1],v1,&a2z[0],3);
	VSUB(antloc[2],v1,&a3z[0],3);

	a1zmag=VMOD(a1z,3);
	a2zmag=VMOD(a2z,3);
	a3zmag=VMOD(a3z,3);

	amatrx[0][0]=a1z[0]/a1zmag;
	amatrx[0][1]=a1z[1]/a1zmag;
	amatrx[0][2]=a1z[2]/a1zmag;
	amatrx[1][0]=a2z[0]/a2zmag;
	amatrx[1][1]=a2z[1]/a2zmag;
	amatrx[1][2]=a2z[2]/a2zmag;
	amatrx[2][0]=a3z[0]/a3zmag;
	amatrx[2][1]=a3z[1]/a3zmag;
	amatrx[2][2]=a3z[2]/a3zmag;
	bvec[0]=cos(Tc);
	bvec[1]=cos(Tc);
	bvec[2]=cos(Tc);

	success=linsystem(&amatrx[0][0],&bvec[0]);


	cx3mod=VMOD(bvec,3)+1.E-9;
	axis[3][0]=bvec[0]/cx3mod;
	axis[3][1]=bvec[1]/cx3mod;
	axis[3][2]=bvec[2]/cx3mod;


	cx3[0]=(axis[0][0]+axis[1][0]+axis[2][0]+axis[3][0])/4;
	cx3[1]=(axis[0][1]+axis[1][1]+axis[2][1]+axis[3][1])/4;
	cx3[2]=(axis[0][2]+axis[1][2]+axis[2][2]+axis[3][2])/4;
	cx3[0]=cx3[0]/sqrt(cx3[0]*cx3[0]+cx3[1]*cx3[1]+cx3[2]*cx3[2]);
	cx3[1]=cx3[1]/sqrt(cx3[0]*cx3[0]+cx3[1]*cx3[1]+cx3[2]*cx3[2]);
	cx3[2]=cx3[2]/sqrt(cx3[0]*cx3[0]+cx3[1]*cx3[1]+cx3[2]*cx3[2]);
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////



///////////////////////////////////////////////////////////////
//AVERAGE OF V2
///////////////////////////////////////////////////////////////
	VSUB(antloc[0],v2,&a1z[0],3);
	VSUB(antloc[1],v2,&a2z[0],3);
	VSUB(antloc[2],v2,&a3z[0],3);

	a1zmag=VMOD(a1z,3);
	a2zmag=VMOD(a2z,3);
	a3zmag=VMOD(a3z,3);

	amatrx[0][0]=a1z[0]/a1zmag;
	amatrx[0][1]=a1z[1]/a1zmag;
	amatrx[0][2]=a1z[2]/a1zmag;
	amatrx[1][0]=a2z[0]/a2zmag;
	amatrx[1][1]=a2z[1]/a2zmag;
	amatrx[1][2]=a2z[2]/a2zmag;
	amatrx[2][0]=a3z[0]/a3zmag;
	amatrx[2][1]=a3z[1]/a3zmag;
	amatrx[2][2]=a3z[2]/a3zmag;
	bvec[0]=cos(Tc);
	bvec[1]=cos(Tc);
	bvec[2]=cos(Tc);

	success=linsystem(&amatrx[0][0],&bvec[0]);


	cx4mod=VMOD(bvec,3)+1.E-9;
	axis[0][0]=bvec[0]/cx4mod;
	axis[0][1]=bvec[1]/cx4mod;
	axis[0][2]=bvec[2]/cx4mod;
	





	VSUB(antloc[0],v2,&a1z[0],3);
	VSUB(antloc[1],v2,&a2z[0],3);
	VSUB(antloc[3],v2,&a3z[0],3);

	a1zmag=VMOD(a1z,3);
	a2zmag=VMOD(a2z,3);
	a3zmag=VMOD(a3z,3);

	amatrx[0][0]=a1z[0]/a1zmag;
	amatrx[0][1]=a1z[1]/a1zmag;
	amatrx[0][2]=a1z[2]/a1zmag;
	amatrx[1][0]=a2z[0]/a2zmag;
	amatrx[1][1]=a2z[1]/a2zmag;
	amatrx[1][2]=a2z[2]/a2zmag;
	amatrx[2][0]=a3z[0]/a3zmag;
	amatrx[2][1]=a3z[1]/a3zmag;
	amatrx[2][2]=a3z[2]/a3zmag;
	bvec[0]=cos(Tc);
	bvec[1]=cos(Tc);
	bvec[2]=cos(Tc);

	success=linsystem(&amatrx[0][0],&bvec[0]);


	cx4mod=VMOD(bvec,3)+1.E-9;
	axis[1][0]=bvec[0]/cx4mod;
	axis[1][1]=bvec[1]/cx4mod;
	axis[1][2]=bvec[2]/cx4mod;




	VSUB(antloc[0],v2,&a1z[0],3);
	VSUB(antloc[3],v2,&a2z[0],3);
	VSUB(antloc[2],v2,&a3z[0],3);

	a1zmag=VMOD(a1z,3);
	a2zmag=VMOD(a2z,3);
	a3zmag=VMOD(a3z,3);

	amatrx[0][0]=a1z[0]/a1zmag;
	amatrx[0][1]=a1z[1]/a1zmag;
	amatrx[0][2]=a1z[2]/a1zmag;
	amatrx[1][0]=a2z[0]/a2zmag;
	amatrx[1][1]=a2z[1]/a2zmag;
	amatrx[1][2]=a2z[2]/a2zmag;
	amatrx[2][0]=a3z[0]/a3zmag;
	amatrx[2][1]=a3z[1]/a3zmag;
	amatrx[2][2]=a3z[2]/a3zmag;
	bvec[0]=cos(Tc);
	bvec[1]=cos(Tc);
	bvec[2]=cos(Tc);

	success=linsystem(&amatrx[0][0],&bvec[0]);


	cx4mod=VMOD(bvec,3)+1.E-9;
	axis[2][0]=bvec[0]/cx4mod;
	axis[2][1]=bvec[1]/cx4mod;
	axis[2][2]=bvec[2]/cx4mod;



	VSUB(antloc[3],v2,&a1z[0],3);
	VSUB(antloc[1],v2,&a2z[0],3);
	VSUB(antloc[2],v2,&a3z[0],3);

	a1zmag=VMOD(a1z,3);
	a2zmag=VMOD(a2z,3);
	a3zmag=VMOD(a3z,3);

	amatrx[0][0]=a1z[0]/a1zmag;
	amatrx[0][1]=a1z[1]/a1zmag;
	amatrx[0][2]=a1z[2]/a1zmag;
	amatrx[1][0]=a2z[0]/a2zmag;
	amatrx[1][1]=a2z[1]/a2zmag;
	amatrx[1][2]=a2z[2]/a2zmag;
	amatrx[2][0]=a3z[0]/a3zmag;
	amatrx[2][1]=a3z[1]/a3zmag;
	amatrx[2][2]=a3z[2]/a3zmag;
	bvec[0]=cos(Tc);
	bvec[1]=cos(Tc);
	bvec[2]=cos(Tc);

	success=linsystem(&amatrx[0][0],&bvec[0]);


	cx4mod=VMOD(bvec,3)+1.E-9;
	axis[3][0]=bvec[0]/cx4mod;
	axis[3][1]=bvec[1]/cx4mod;
	axis[3][2]=bvec[2]/cx4mod;


	cx4[0]=(axis[0][0]+axis[1][0]+axis[2][0]+axis[3][0])/4;
	cx4[1]=(axis[0][1]+axis[1][1]+axis[2][1]+axis[3][1])/4;
	cx4[2]=(axis[0][2]+axis[1][2]+axis[2][2]+axis[3][2])/4;
	cx4[0]=cx4[0]/sqrt(cx4[0]*cx4[0]+cx4[1]*cx4[1]+cx4[2]*cx4[2]);
	cx4[1]=cx4[1]/sqrt(cx4[0]*cx4[0]+cx4[1]*cx4[1]+cx4[2]*cx4[2]);
	cx4[2]=cx4[2]/sqrt(cx4[0]*cx4[0]+cx4[1]*cx4[1]+cx4[2]*cx4[2]);
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////




	double R1[3],R2[3],R3[3],R4[3],r1,r2,r3,r4;
	VSUB(antloc[0],vf,R1,3);
	VSUB(antloc[1],vf,R2,3);
	VSUB(antloc[2],vf,R3,3);
	VSUB(antloc[3],vf,R4,3);
	r1=VMOD(R1,3);
	r2=VMOD(R2,3);
	r3=VMOD(R3,3);
	r4=VMOD(R4,3);
	
	if (r1==0) r1=.001;
	if (r2==0) r2=.001;
	if (r3==0) r3=.001;
	if (r4==0) r4=.001;
	if (volts[0]==0) volts[0]=.001;
	if (volts[1]==0) volts[1]=.001;
	if (volts[2]==0) volts[2]=.001;
	if (volts[3]==0) volts[3]=.001;
	if (rms[0]==0) rms[0]=.001;
	if (rms[1]==0) rms[1]=.001;
	if (rms[2]==0) rms[2]=.001;
	if (rms[3]==0) rms[3]=.001;

	confit=((volts[1]/rms[1])/(volts[0]/rms[0])-r2/r1)*((volts[1]/rms[1])/(volts[0]/rms[0])-r2/r1) + ((volts[2]/rms[2])/(volts[0]/rms[0])-r3/r1)*((volts[2]/rms[2])/(volts[0]/rms[0])-r3/r1) + ((volts[3]/rms[3])/(volts[0]/rms[0])-r4/r1)*((volts[3]/rms[3])/(volts[0]/rms[0])-r4/r1);
	
	double error1,error2;
	error1=(jray[0]/jraymag-cx3[0])*(jray[0]/jraymag-cx3[0])+(jray[1]/jraymag-cx3[1])*(jray[1]/jraymag-cx3[1])+(jray[2]/jraymag-cx3[2])*(jray[2]/jraymag-cx3[2]);
	error2=(jray[0]/jraymag-cx4[0])*(jray[0]/jraymag-cx4[0])+(jray[1]/jraymag-cx4[1])*(jray[1]/jraymag-cx4[1])+(jray[2]/jraymag-cx4[2])*(jray[2]/jraymag-cx4[2]);
	if (error1<error2)
	{
		vx=v1[0];
		vy=v1[1];
		vz=v1[2];
		px=cx3[0];
		py=cx3[1];
		pz=cx3[2];
	}
	else if (error1>=error2)
	{
		vx=v2[0];
		vy=v2[1];
		vz=v2[2];
		px=cx4[0];
		py=cx4[1];
		pz=cx4[2];
	}

	
	return 1;
}



//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
//END 5 HIT TEST
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
*/



void QUAD(double a, double b, double c, double& xq1, double& xq2){
  double arg;
  xq1=xq2=0;
  arg=b*b-4*a*c;
  if (arg<0){
    arg=0;
  }
  xq1=(-b+sqrt(arg))/(2*a);
  xq2=(-b-sqrt(arg))/(2*a);
  if (a > 1E20 || b > 1E20 || c > 1E20){
    xq1=xq2=0;
  }
}


void VSUB(double vA[3], double vB[3], double* vC, int vLength){
  for (int vCnt=0; vCnt<vLength; vCnt++)
    *(vC+vCnt)=*(vA+vCnt)-*(vB+vCnt);
}


double VMOD(double vmA[], int vmLength){
  double vmSum=0;
  for (int vmCnt=0; vmCnt<vmLength; vmCnt++)
    vmSum+=vmA[vmCnt]*vmA[vmCnt];
  return sqrt(vmSum);
}


int linsystem(double* matrx, double* vect){
  double destvec[3]={0,0,0};
  int determ;
  void matrxvecmult(double* matrx, double* vect, double* dest);
  int invert(double* matrx);
  
  determ=invert(matrx);
  if (determ==1){
    matrxvecmult(matrx, vect, &destvec[0]);
    *vect=destvec[0];
    *(vect+1)=destvec[1];
    *(vect+2)=destvec[2];
    return 1;
  }
  else
    return 0;
}

void matrxvecmult(double* matrx, double* vect, double* dest){
  for (int cnt=0; cnt<3; cnt++)
    for (int cnt2=0; cnt2<3; cnt2++) 
      *(dest+cnt)+=*(matrx+3*cnt+cnt2)*(*(vect+cnt2));
}

int invert(double* matrx){
  double adj[3][3], adjt[3][3], det;
  adj[0][0]=(*(matrx+4)*(*(matrx+8)))-(*(matrx+5)*(*(matrx+7)));
  adj[0][1]=-((*(matrx+3)*(*(matrx+8)))-(*(matrx+5)*(*(matrx+6))));
  adj[0][2]=(*(matrx+3)*(*(matrx+7)))-(*(matrx+4)*(*(matrx+6)));
  adj[1][0]=-((*(matrx+1)*(*(matrx+8)))-(*(matrx+2)*(*(matrx+7))));
  adj[1][1]=(*(matrx)*(*(matrx+8)))-(*(matrx+2)*(*(matrx+6)));
  adj[1][2]=-((*(matrx)*(*(matrx+7)))-(*(matrx+1)*(*(matrx+6))));
  adj[2][0]=(*(matrx+1)*(*(matrx+5)))-(*(matrx+2)*(*(matrx+4)));
  adj[2][1]=-((*(matrx)*(*(matrx+5)))-(*(matrx+2)*(*(matrx+3))));
  adj[2][2]=(*(matrx)*(*(matrx+4)))-(*(matrx+1)*(*(matrx+3)));
  
  adjt[0][0]=adj[0][0];
  adjt[0][1]=adj[1][0];
  adjt[0][2]=adj[2][0];
  adjt[1][0]=adj[0][1];
  adjt[1][1]=adj[1][1];
  adjt[1][2]=adj[2][1];
  adjt[2][0]=adj[0][2];
  adjt[2][1]=adj[1][2];
  adjt[2][2]=adj[2][2];
  
  det=*(matrx)*adj[0][0]+*(matrx+1)*adj[0][1]+*(matrx+2)*adj[0][2];
  
  if (det!=0){
    for (int cnt=0; cnt<3; cnt++){
      for (int cnt2=0; cnt2<3; cnt2++){
	adjt[cnt][cnt2]*=1/det;
	*(matrx+3*cnt+cnt2)=adjt[cnt][cnt2];
      }
    }
    return 1;
  }
  else
    return 0;
}


double dotprod(double* a, double* b, int length){
  double sum=0;
  for (int cnt=0; cnt<length; cnt++)
    sum+=(*(a+cnt))*(*(b+cnt));
	
  return sum;
}
