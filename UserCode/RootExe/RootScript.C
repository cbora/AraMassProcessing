void RootScript(char * inFileName, char * outFileName, char *nonRootFile)
{
  //TFile f(inFileName);
  //f.Print();
  //f.ls();
  TFile *oldfile = new TFile(inFileName);
  TTree *oldtree = (TTree*)oldfile->Get("eventTree");

  //Create a new file + a clone of old tree header.
  TFile *newfile = new TFile(outFileName,"recreate");
  TTree *newtree = oldtree->CloneTree(0);

  //Divert branch fH to a separate file and copy all events
  newtree->CopyEntries(oldtree);
  newtree->Print();
  newfile->Write();
  delete oldfile;
  delete newfile;
//TFile f("/data/exp/ARA/2011/filtered/L0/0610/run003544.root");
//.ls
//.q
}
