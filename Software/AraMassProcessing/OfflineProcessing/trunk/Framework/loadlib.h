void loadlib(){
  //gSystem->AddIncludePath("-I${ARA_UTIL_INSTALL_DIR}/include");
  gSystem->AddIncludePath("-I/net/user/cweaver/tools/${OS_ARCH}/include");
  gSystem->AddIncludePath("-I/net/user/cweaver/tools/${OS_ARCH}/include/AraRoot");
  gSystem->Load("libfftw3.so"); //all in /net/user/cweaver/tools/${OS_ARCH}/lib/
  cout<<"loading libfftw3.so"<<endl;
  gSystem->Load("libgsl.so");
  cout<<"loading libgsl.so"<<endl;
  gSystem->Load("libMathMore.so");
  cout<<"loading libMathMore.so"<<endl;
  gSystem->Load("libGeom.so");
  cout<<"loading libGeom.so"<<endl;
  gSystem->Load("libGraf3d.so");
  cout<<"loading libGraf3d.so"<<endl;
  gSystem->Load("libPhysics.so");  
  cout<<"loading libPhysics.so"<<endl;
  gSystem->Load("libRootFftwWrapper.so");     	  
  cout<<"loading libRootFftwWrapper.so"<<endl;
  gSystem->Load("libAraEvent.so");
  cout<<"loading libAraEvent.so"<<endl;
  gSystem->Load("libAraDisplay.so");
  cout<<"loading libAraDisplay.so"<<endl;
  gSystem->Load("libAraCorrelator.so");
  cout<<"loading libAraCorrelator.so"<<endl;
}

