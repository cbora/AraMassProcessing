{
  //    gSystem->SetIncludePath(" -I/home/hep/savdeev/AraRoot/include/ -I/home/hep/savdeev/RootFftwWrapper/include/ ");
  gSystem->SetIncludePath(" -I${ARA_ROOT_DIR}/include/ -I${ARA_UTIL_INSTALL_DIR}/include ");
  gSystem->Load("libAraEvent");
}
