{
  /****************************************************
   * In case the analysis executable need to be
   * compiled, compile the executable should be done 
   * in here, simply do:
   *    gRoot->ProcessLine(".L YOURexecutableName.C+");
   * this should come after the execution of rootlogon
   *****************************************************/

  gROOT->ProcessLine(".x /home/cbora/rootlogon.C");
  // gROOT->ProcessLine(".L myL2vrtx_compile.C+");
}
