#!/usr/bin/perl

use File::Basename;
use Tie::File;

#use strict;
#use warning;
# This function finds files that match the template (@_[1]),
# located recursevly in directory @_[0],
# the basename of this files should be equal or greater than $to and
# less or equal to $from (lexically)
sub FindFiles {
	my $directoryPath = @_[0];#"/data/exp/ARA/2012/filtered/L0/0101/";
	my $rootFileExtension = @_[1];#"*.root";
	my $from = @_[2];
	my $to = @_[3];

	print "$directoryPath \n";
	print "$rootFileExtension \n";
	print "$from \n";
	print "$to \n";
#	$from= "/data/exp/ARA/2011/minbias/SPS-ARA/Testbed_2011/event/run_001000/ev_1294457901/ev_1294457901.217633.run001000.dat";

#	$to= "/data/exp/ARA/2011/minbias/SPS-ARA/Testbed_2011/event/run_001010/ev_1294475541/ev_1294475675.040201.run001010.dat";
	my $oneLineFindResults = `find $directoryPath -name \"$rootFileExtension\"`;
#	print "testing after find \n";
#	print "$oneLineFindResults \n";
	my @findResults = split /\n/, $oneLineFindResults;	
	my @fileList;




#check if basename of from and to has the same length
	$fromlength= length $from;
	$tolength= length $to;
	print "$fromlength \n";
	print "$tolength \n";

	if($fromlength != $tolength){
	#if the lenght of the from and to is not equal
	    print "INSIDE \n";
	my ($fromstr)=$from =~ /(\d+)/;

	 my ($tostr)=$to =~ /(\d+)/;
	 
	 $fromstr="0".$fromstr;
	 my ($tol) =length $tostr;
	 
	 for my $full (@findResults) {
	     $basename = basename($full);
	     my ($basenamestr)= $basename =~ /(\d+)/;
	     my $basenamelength = length $basenamestr;
	     if($basenamelength != $tol){
		 $basenamestr = "0".$basenamestr;
	     }
	     if((!defined($fromstr) or $basenamestr ge $fromstr) and (!defined($tostr) or $basenamestr le $tostr)) {
		 push (@fileList, $full);
	     }  
	 }   
	}else{
	    
	    print "HERERERERE \n";
	    for my $full (@findResults) {
		$basename = basename($full);
#		$basename= $full;
#		print "$basename \n";

		if ((!defined($from) or $basename ge $from) and (!defined($to) or $basename le $to)) {
		    print "$full \n";
			push(@fileList, $full);
		}
	    }
	}
	@fileList = sort(@fileList);
	my $tot = @findResults;
	    #my $tot1 = @fileList;
	    #print "tot1 = $tot, tot2 = $tot1\n";
       
	return (\@fileList, \$tot);
}

# Prints file list to a file
sub PrintFileListToFile {
	my @list = @{@_[0]};
	my $fileName = ${@_[1]};
	open(FILE, ">$fileName") or die "Cannot open file $fileName : $!\n";
	for my $line (@list) {
		#print "$line\n";
		print FILE "$line\n";
	}
	close(FILE);
}

sub GetFromToFullPath{
    #this function return the first file in the specified FROM directory and the last file in the TO directory
    #which are then used in Find files function
    my $dirPath=$_[0];
    my $from=$_[1];
    my $to=$_[2];
    my $ext=$_[3];

    my $topPath=$dirPath . "/" . $from;
    my $bottomPath=$dirPath . "/" . $to;
    my $resultTop=`find $topPath -name \"$ext\"`;
    my $resultBottom=`find $bottomPath -name \"$ext\"`;

#look in the FROM directory
    open(my $ftop, '>', "../temp1.txt");
    print $ftop $resultTop;
    close $ftop;
    
    tie my @fileListTop, 'Tie::File', '../temp1.txt' or die $!;
    my ($top, $b) = @fileListTop[0, -1];


#look in the TO directory
    open(my $fbottom, '>',  "../temp2.txt");
    print $fbottom $resultBottom;
    close $fbottom;

    tie my @fileListBottom, 'Tie::File', '../temp2.txt' or die $!;
    my ($t, $bottom) = @fileListBottom[0, -1];


#return top and bottom files found
    return ($top, $bottom);

}

#($ref1, $ref2) = FindFiles("/data/exp/ARA/2012/filtered/L0/", "*.root", "run010944.root", "run010955.root");
#@list = @$ref1;
#$tot = $$ref2;
#print "@list\n";
#print "$tot\n";
#PrintFileListToFile(\@list, \"fileList.tmp");
