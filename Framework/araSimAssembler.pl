#!/usr/bin/perl
use Cwd;

use File::Basename;

#this script collects files to process given a level number
#$level = @_[0];
#$level = shift;
#($val) = start($level);

#function to run all the function in this script and pass appropriate arguments
sub start {
    $level = @_[0];
    sub getFileList;
    print "level $level \n";
    ($fileListArr, $tot) = getFileList($level);

    print "total $tot \n";


    $totFiles = $tot;

    $fileName1 = "inputFilesToProcess.txt";

    print "fileName: $fileName1 \n";

    printFileList($fileListArr, $fileName1);
#($totFiles) = getTotalNumberOfFiles(@fileListArr);

    ($textFileName) = generateTextFileName($level);

    $limit = 2;

    ($totalJobs) = loadBalance($fileName1, $level, $limit, $totFiles);

    print "total jobs . $totalJobs \n";
    return ($level);
}



sub getFileList {
    #this function collects appropriate file names depending on the level of the merge
    #and saves them into an array

    my $levelNumber = @_[0];
    
#    print `pwd`;
    my $directoryPath = getcwd;
    print "directory $directoryPath \n";
    my $oneLineResults;
    $fileTemplate = "levelOut_"."$levelNumber"."_*.root";
    if($levelNumber == 0){
    
	$oneLineResults = `find $directoryPath -name \"AraOut.*.root\"`;
    
	print "$oneLineResults \n";
    }else {
	$decrLevel = $levelNumber - 1;
	$fileTemplate = "mergeOutput_".$decrLevel."_"."*.root";
	#$fileTemplate = "mergeOutput_".$decrLevel."_";
	print "fileTemplate $fileTemplate \n";
	
	$oneLineResults = `find $directoryPath -name \"$fileTemplate\"`;
    }
    
    my @fileList = split /\n/, $oneLineResults;
#    my @fileList = $oneLineResults;
    for my $line (@fileList){
	print "line is $line \n";
    }
    $tot = @fileList;
    print "total toal $tot \n";
    return (\@fileList, $tot);
}

sub printFileList {
    #takes array and writes content into text file

    my @list = @{@_[0]};
    print "asa @_[0] \n";
    print "list @_[1] \n";
#    my $fileName = ${@_[1]};
    my $fileName = @_[1];
    print "$fileName \n";
    open(FILE, ">$fileName") or die "Cannot open file $fileName : $!\n";
    for my $line (@list) {
                #print "$line\n";                                                                                                                                                              
	print FILE "$line\n";
    }
}

sub loadBalance {

    #this function balances input files per job
    
    my $input = @_[0];
    my $levelNumber =@_[1];
#    my $totNumberOfFiles = @_[2];   #total Number of files
    my $limit =@_[2];
    my $totNumberOfFiles =@_[3];
    
    print "ototal @_[3] \n";
    
    print "*********************************************** \n";
    print totNumberOfFiles "\n";

   
    if( $totNumberOfFiles % 2 == 1){
	print "is ODD \n";
#	$limit = 3;
	$isOdd = 1;
    }

   
    my $counter = 0;
    if( $isOdd == 1){
	print "*********************************************** \n";
	loadBalanceOdd($input, $levelNumber);
	$input = "temp.txt";
#	$input ="inputFilesToProcess.txt";
	$counter = 2;
    }
    print "input $input \n";
    print "counter $counter \n";

    open(FILE, "$input");
    


    ($prefix) = generateTextFileName($levelNumber);
    print "prefix $prefix \n";
    $postfix =".txt";
    $output = undef;
    while(<FILE>){
#	if(($isOdd == 1) && ($totNumberOfFiles == 3) && ($counter == 0)){
#	    print "only 3 files \n";
#	    $limit = 3;
	
#	}
	print "in while loop******************************* \n";
	if($counter % $limit == 0){
	    #   $limit =int(2);  # set to 2 in case the number of files was odd
	    
	    if(defined($output)){
		close(OUTPUT);
	    }
	    
	    $output = open(OUTPUT, ">$prefix". int($counter/$limit) . "$postfix\n");
	    print "$output \n";
	}
	chomp;
	print OUTPUT "$_\n";
	
	$counter = $counter + 1;
#	if(($isOdd == 1) && ($counter == ($totNumberOfFiles - 3)) && ($totNumberOfFiles != 3)){
#	    print "***************************** \n";
#	    $limit = 3;
#	}

    }

    print "counter   $counter \n";
    close(FILE);

    $totalFiles = 1 + int(($counter - 1)/$limit);
#    $totalFiles = 10;

    print "Job files created: ", $totalFiles, "\n";
    
    `rm temp.txt`;
    
    return ($totalFiles);

	     
    

}

sub getTotalNumberOfFiles {
    # this function returns the number of files to collect on each level
    
    my @arrayList = @_[0];
    
    
    my $tot = @arrayList;
    return ($tot);

}

sub generateTextFileName {
    my $levelNumber = @_[0];
    #my $processNumber = @_[1];

    #generating text file for each level
    #level 0 files => have the form of AraOut.*.root
    #level 1, 2, 3,... => have the form of level_$_&.root => where $ is the level number, and & is the $(Process)

    #textFile have the format of
    #inputLevel_$_&.txt => where $ is the level of merging and & is the $(Process)

    my $textFileName = "inputFile_". "$levelNumber"."_"; #."_".$processNumber.".txt";

    return ($textFileName);

}


sub loadBalanceOdd {
#    $input = "list.txt";

    #in case the number of files is odd this function is called just once to group 3 files
    #then the initial loadBaalance is does the rest with 2 files in each file
    $input = @_[0];
    $levelNumber = @_[1];

    open(FILE, "$input");

    $count= 0;

#    $outputFile = "test.txt";
    $prefix = generateTextFileName($levelNumber);
    $postfix = ".txt";
    $tempFile= "temp.txt";

    while(<FILE>){
	print "in while loop";
	if($count < 3){
	    if($count == 0){
		print "creating file \n";
#		$output = open(OUTPUT, ">$outputFile");
		$output = open(OUTPUT, ">$prefix". int($count) . "$postfix\n");
	    }
	    chomp;
	    print OUTPUT "$_\n";
	    $count = $count + 1;

	}else{
	    if($count == 3){
		print "in second statement \n";
		if(defined($output)){
		    close OUTPUT;
		}
		$output = open(OUTPUT, ">$tempFile");
	    }
	    chomp;
	    print OUTPUT "$_\n";
	    $count = $count + 1;
	}

    }
    close(FILE);
    if(defined($output)){
	close OUTPUT;
    }
    print "done \n";
}
