#!/usr/bin/perl

#package main;
#use HA;

#startRootMode();
sub startRootMode{
    
    #in case of root mode this script is called, and calls other functions in other text files
    $template = @_[0];
    $dataPath = @_[1];
    $from = @_[2];
    $to = @_[3];
    $resultPath = @_[4];
    $date = @_[5];
    $fileLimit =@_[6];

    print "$template .. \n";
    print "$dataPath .. \n";
    print "$from .. \n";
    print "$to .. \n";
    print "resultPath .. \n";
    print "$date .. \n";
    print "$fileLimit .. \n";

 #   do "./functionToPrint.pl";
#    sub printCollect;
    printCollect();
    print "je \n";
#    print nonne;
#    do "./fileList2.pl";

    ($pList, $pNumber) = FindFiles($dataPath, $template, $from, $to);
#print "
    print "$pList .. \n";
    @fileList = @{$pList};
    $totNum = ${$pNumber};
    print "Files matching template \"$template\" : $totNum\n";
    print "Files satisfying the from-to criteria: " . scalar(@fileList) . "\n";
    
    if (scalar(@fileList) == 0) {
        print "|-----------------------------|\n";
        print "| Nothing to do. Terminating  |\n";
        print "|-----------------------------|\n";
        close(STDOUT);
        exit 0;
    }

    #PrintFileListToFile(\@fileList, \"$Init{'RESULTS_PATH'}/$date/Input/L0filesToProcess.txt");
    #print all files to process into a single text file
    $inputPath = $resultPath."/".$date."/Input/";
    print "$inputPath ..... \n";
    $allFiles = $inputPath."L0filesToProcess.txt";
    print "$allFiles ... \n";
    PrintFileListToFile(\@fileList, \$allFiles);
    
    printCreate();

    #load equally all files into separate files
#    do "./loadBalance.pl";
#    $totalFiles = SeparateTasks("$Init{'RESULTS_PATH'}/$date/Input/L0filesToProcess.txt", "$Init{'RESULTS_PATH'}/$date/Input", $Init{'FILE_LIMIT'});
    $totalFiles = SeparateTasks($allFiles, $inputPath, $fileLimit);
    
    print "Total files = $totalFiles\n";

    printSubmit();

    do "./condorSubmit.pl";
    $outputPath = $resultPath."/".$date;
    
    my $clusterID;

    if($executionTime eq "long"){

	printLongExecution();
	$clusterID = SubmitCondorJobLong($totalFiles, $outputPath);

    }else {
	
	printShortExecution();
	$clusterID =SubmitCondorJob($totalFiles, $outputPath);
    }

    print "......clusterID $clusterID \n";
    return $clusterID;

}
