#!/usr/bin/perl
# this script separates files into different files according to the run number
sub SeparateTasks{
#    print "Inside loadBalanceRawData.pl";
    my $input=$_[0];
    my $outputPath=$_[1];
    my $limit=$_[2];

    my $prefix="$outputPath/job"; 
    my $postfix=".txt";

#    my $output=undef;

    open(FILE, "$input");
    while(<FILE>){
	push(@arrayInput, $_);
    }



#find differnt run Numbers in the input file
#and make them unique

    my $runSubstr="run_";
    my $substrLength= 10;
    my @listOfRun;
    for my $full(@arrayInput){
	
	my $indexOfRun=index($full, $runSubstr);
	my $fragment= substr $full, $indexOfRun, $substrLength;
	if( grep( /^$fragment$/, @listOfRun)){
	    #value already exists, do nothing
	}else{
	    push(@listOfRun, $fragment);
	}
    }
    my $counter=0;

    
#create a text file for each run
    for $element(@listOfRun){
	if(defined($output)){
	    #if there is an open file close it
	    close(OUTPUT);
        }
	$output=open(OUTPUT, ">$prefix". int($counter) . "$postfix\n");
	my @results =();
	for $line(@arrayInput){
	    if(index($line, $element) != -1){
		push(@results, $line);
	    }
	}
	print OUTPUT @results;
	$counter= $counter+1;
    }


    $totalFiles = $counter;
    return $totalFiles;
}
