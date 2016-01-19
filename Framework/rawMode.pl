#!/usr/bin/perl

sub startRawMode{

    #Script to execute the raw data mode
    #the print function are from functionToPrint.pl
    $template =@_[0];
    $dataPath = @_[1];
    $from = @_[2];
    $to = @_[3];
    $resultPath = @_[4];
    $date = @_[5];
    $fileLimit =@_[6];

    #Find files to process and return an array of files
    ($top, $bottom) = GetFromToFullPath($dataPath, $from, $to, $template);
    ($pList, $pNumber) = FindFiles($dataPath, $template, $top, $bottom);

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

    #print the array of files into a text file
    $inputPath = $resultPath."/".$date."/Input/";
    $allFiles = $inputPath."L0filesToProcess.txt";
    PrintFilesListToFile(\@fileList, \$allFiles);

    printCreate();
    
    #balance input files into equal number based on the file limit for each section
    $totalFiles = SeparateTasks($allFiles, $inputPath, $fileLimit);

    print "Total files = $totalFiles \n";

    printSubmit();
    
    my $clusterID;

    printShortExecution();
    
    $outputPath = $resultPath."/".$date;
    $clusterID = SubmitCondorJobRawData($totalFiles, $outputPath);
    


    return $clusterID;

}
    
sub startRawModeWithTextFile{

    #Scripts to run data data files given only text file containing all the files to process.
    $inputFile =@_[0];
    $resultPath =@_[1];
    $date = @_[2];
    $fileLimit = @_[3];
   
    print "Text file mode \n ";

    
    if (not -e $inputFile){
	print "|-----------------------------|\n";
        print "| Input File: " .  $inputFile . " does not exist |\n";
        print "|-----------------------------|\n";

	
	print "|-----------------------------|\n";
        print "| Nothing to do. Terminating  |\n";
        print "|-----------------------------|\n";
        close(STDOUT);
        exit 0;
    }

    #print the array of files into a text file
    $inputPath = $resultPath."/".$date."/Input/";
    $allFiles = $inputPath."L0filesToProcess.txt";
    `cp $inputFile $allFiles`;

#    PrintFilesListToFile(\@fileList, \$allFiles);

    printCreate();
    
    #balance input files into equal number based on the file limit for each section
    $totalFiles = SeparateTasks($allFiles, $inputPath, $fileLimit);

    print "Total files = $totalFiles \n";

    printSubmit();
    
    my $clusterID;

    printShortExecution();
    
    $outputPath = $resultPath."/".$date;
    $clusterID = SubmitCondorJobRawData($totalFiles, $outputPath);
    
    return $clusterID;

}
    
