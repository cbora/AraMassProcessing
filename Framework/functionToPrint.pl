#!/usr/bin/perl

#This script contains most of the printing steps


sub printCollect{
   
    print "|-----------------------------|\n";
    print "| Collecting files to process |\n";
    print "|-----------------------------|\n";

}


sub printNothingToDo{

    print "|-----------------------------|\n";
    print "| Nothing to do. Terminating  |\n";
    print "|-----------------------------|\n";
    close(STDOUT);
    exit 0;

}

sub printCreate{
    
    print "|-----------------------------|\n";                                                                                                                                                      
    print "| Creating separate job files |\n";                                                                                                                                                     
    print "|-----------------------------|\n";                   

}

sub printSubmit{

    print "|-----------------------------|\n";                                                                                                                                                    
    print "|  Submitting jobs to Condor  |\n";                                                                                                                                                     
    print "|-----------------------------|\n";      

}


sub printLongExecution{

    print "|-----------------------------|\n";                                                                                                                                                 
    print "|  24 hours max excution time |\n";                                                                                                                                                      
    print "|          Per job            |\n";                                                                                                                                                    
    print "|-----------------------------|\n";                

}


sub printShortExecution{
    
    print "|-----------------------------|\n";                                                                                                                                               
    print "|  12 hours max excution time |\n";
    print "|          Per job            |\n";                                                                                                                           
    print "|-----------------------------|\n";       

}


sub printWaiting{

    print "|------------------------------|\n";                                                                                                                                                    
    print "| Waiting for jobs to complete |\n";                                                                                                                                                    
    print "|------------------------------|\n";              
}


sub printPrepare{
    
    print "|------------------------------|\n";                                                                                                                                                    
    print "| Preparing and sending report |\n";                                                                                                                                                    
    print "|------------------------------|\n";               
}


sub printSendReport{
    
    print "|------------------------------|\n";                                                                                                                                                    
    print "| Preparing and sending report |\n";                                                                                                                                                    
    print "|------------------------------|\n";    
}


sub printDone{
    

    print "|---------------------------|\n";                                                                                                                                                       
    print "|           Done!           |\n";                                                                                                                                                       
    print "|---------------------------|\n";    

}

sub printStartMerge{
    
    print "|---------------------------|\n";
    print "|     Starting Merging...   |\n";
    print "|---------------------------|\n";

}

sub printMergeStep{
    $step = @_[0];
    
    print "|---------------------------|\n";
    print "|  Merge Level: $step ...   |\n";
    print "|---------------------------|\n";
}

sub printMergeDone{
    
    print "|---------------------------|\n";
    print "|     Merging Done!         |\n";
    print "|---------------------------|\n";

}

sub printFailure{

    print "|---------------------------|\n";
    print "|     Job Failure           |\n";
    print "|---------------------------|\n";
    
}

sub printAraSimFileNotFound {
    $fileName = @_[0];
    print "|---------------------------|\n";
    print "| File not found: $fileName |\n";
    print "| Check if path is correct! |\n";
    print "|---------------------------|\n";
}

sub printAraSimPathError {
    $path =@_[0];

    print "|---------------------------|\n";
    print "| AraSim path does not exists $path |\n";
    print "| Correct path and run again |\n";
    print "|---------------------------|\n";
1;


