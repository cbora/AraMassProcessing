//void mergeTree();
//void start();

int mergeTree(char *input, int jobNum, int level, char *outputPath){
//void mergeTree(char *input, int jobNum){
  
  int failure = 1;
  cout <<"input " <<input <<" job Num "<< jobNum << endl;

  ifstream inFile(input);
  TList l1;
  TList l2;
  TList l3;
  string path;
  int totalEvents;
 
  char outRootFile[200];
  sprintf(outRootFile, "mergeOutput_%i_%i.root",level, jobNum);
  string path2 = "default";
   cout << "outRootFile " << outRootFile << endl;

  while(!inFile.eof()){

    if(inFile.eof() || !inFile.good()) break;
    inFile >>path;
    if(path2 == path) break;

    cout << "path : "<< path << endl;
    TFile *f= new TFile(path.c_str());
    TTree *tree =(TTree*) f->Get("eventTree");
    TTree *tree2 = (TTree*) f->Get("AraTree");
    TTree *tree3 = (TTree*) f->Get("AraTree2");

    l1.Add(tree);
    l2.Add(tree2);
    l3.Add(tree3);
    path2 = path;
  
    totalEvents =(Int_t)tree->GetEntries();
  }
  
  TFile *out=new TFile(outRootFile, "RECREATE");
  TTree::MergeTrees(&l1);
  TTree::MergeTrees(&l2);
  TTree::MergeTrees(&l3);
  out->Write();
  out->Close();
  cout <<"Total Events"<< totalEvents << endl;
  
  failure = 0;

  return failure;
}
