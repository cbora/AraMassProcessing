#!/usr/bin/perl

#use strict;
use lib 'UserCode';   #To make compiler aware about config.pm location in Framework/ directory

use config;
use Data::Dumper;
use Tie::File;

package main;
require Exporter;

our @ISA = qw(Exporter);
our @EXPORT = qw(%init);

%Init = %config::init;

sub CreateIfNotExist;
sub GetDateString;

# Get time stamp string. It will be a name of directory with results
$date = GetDateString(); #"2012-08-14_11:02:59";

sub PrintHelp{
    print "run.pl usage:
		-dir directory where input files are collected
		-from file name or path + file name  of the starting file
		-to file name or path + file name  of the finishing file
		-asyn asynchronous execution (user may logout after hitting enter)
		-email user e-mail for sending report
                -mode \"root\" or \"raw\" or \"arasim\"

=> Visit http://ara.physics.wisc.edu/wiki/index.php/Processing_System for further documentation \n";
	#$switch = undef if $switch;
	exit(1);
}

sub PrintHelpRoot{
    print "run.pl usage: ";

}

my %initCommandLine = ();
my %initConfigFile = ();
my @ARGVinit = @ARGV;
my $argsString = "./run.pl " . join(" ", @ARGV);
my $switch = shift;

if (!$switch) {
	PrintHelp();
}
while (1)
{
	if (!$switch){
		last;
	} elsif ($switch eq "-dir") {	# Where input datafiles are stored
		$initCommandLine{'DATA_PATH'} = shift;
		#print "-dir $directory\n";
	} elsif ($switch eq "-from") {	# Starting file
		$initCommandLine{'FROM'} = shift;
		#print "-from $from\n";
	} elsif ($switch eq "-to") {	# Ending file
		$initCommandLine{'TO'} = shift;
		#print "-to $to\n";
	} elsif ($switch eq "-file") {
	    $initCommandLine{'INPUT_FILE_NAME'} = shift;
	} elsif ($switch eq "-syn") {	# [bool] Synchronous execution mode
		$initCommandLine{'ASYN'} = 0;
	} elsif ($switch eq "-asyn") {	# [bool] Asynchronous execution mode
		$initCommandLine{'ASYN'} = 1;
		#print "-asyn\n";
	} elsif ($switch eq "-email") {	# Email to send report
		$initCommandLine{'EMAILS'} = shift;
		#print "-email $email\n";
	} elsif ($switch eq "-h" or $switch eq "--help" or $switch eq "-help") {
		PrintHelp();
	} elsif ($switch eq "-mode") { #specifying data type
	    $initCommandLine{'MODE'} = shift;
	} elsif($switch eq "-template") {#specifying template of which files to process
	    $initCommandLine{'TEMPLATE'} =shift;   
        } elsif ($switch eq '-config') {
		$configFileName = shift;
		print "-config $configFileName\n";
		if (defined $configFileName) {
			use XML::Simple;
			$initHashRef = XMLin($configFileName);
			%initConfigFile = %{$initHashRef};
			#print Dumper(%initConfigFile);
		}
	} else {
		print "Unknown key $switch\n"
	}
	#print "$switch\n";
	$switch = shift;
}
# First, substitute default with config file values (if any)
@Init{keys %initConfigFile} = values %initConfigFile;
# Second, override with command line arguments (if any)
@Init{keys %initCommandLine} = values %initCommandLine;
#print Dumper(%Init);

# Create directory structure
sub GetDateString{
        my ($sec, $minute, $hour, $day, $month, $year) = localtime();
        $year +=1900;
        $month +=1;
        return sprintf("%d-%02d-%02d_%02d:%02d:%02d", $year, $month, $day, $hour, $minute, $sec);
}

use File::Spec;
$Init{'RESULTS_PATH'} = File::Spec->rel2abs($Init{'RESULTS_PATH'});
my $resultsDir = "$Init{'RESULTS_PATH'}/$date";
my $mainLog = "$resultsDir/Logs/main.log";

# Check for asynchronous execution
if ($Init{'ASYN'}){
	# This script launches another instance of the same script in the background,
	# but with the key '-syn' at the end, which overrides asynchronous
	# since command line arguments have highest priority
	# and the latest key has more priority (-syn will be the last key)
	# However, this will affect 'Initial command line:' line of the report by adding '-syn' key
	# which was not entered by a user
	print "system = " . system("perl run.pl @ARGVinit -syn >& /dev/null &")."\n";
	
	# There is a possibility for time stamps to be different in this process and in the background
	# So, if it is different try +-1 sec
	print "The background process has been launched to process your request.\n";
	print "To see current progress execute:\n";
	
	# Get time stamp string. It will be a name of directory with results
	$date = GetDateString(); #"2012-08-14_11:02:59";
	my $resultsDir = "$Init{'RESULTS_PATH'}/$date";
	my $mainLog = "$resultsDir/Logs/main.log";
	
	print "tail -f $mainLog \n";
	exit(0);
}

# Create directory structure for the results
CreateIfNotExist("$resultsDir");


#Variables from configuration file
$dataType= $Init{'MODE'};
$dataPath = $Init{'DATA_PATH'};
$resultPath = $Init{'RESULTS_PATH'};
$template = $Init{'TEMPLATE'}; 
$from = $Init{'FROM'};
$to = $Init{'TO'};
$sizeLimit = $Init{'SIZE_LIMIT'};
$araSimPath = $Init{'ARASIM_DIR'};
$executionTime = $Init{'EXECUTION_TIME'};
$fileLimit = $Init{'FILE_LIMIT'};
$araSimSectionNumber = $Init{'NUMBER_SECTION'};
$inputFile = $Init{'INPUT_FILE_NAME'};

#print "arasim $araSimPath \n";
#Make ready all necessary files
do "Framework/condorSubmit.pl";
do "Framework/functionToPrint.pl";


do "Framework/araSimMode.pl";
#do "Framework/araSimOutputSearch.pl";
do "Framework/araSimOutputFileSearch.pl";
do "Framework/xmlStatistics.pl";
#Variables used by all modes
my $mode;
my $clusterID;
my $numberOfRemovedJobs;
my $reportStrRoot;
my $reportStrRaw;


if(($dataType eq "root")||($dataType eq "Root")||($dataType eq "ROOT")){

    #Root mode
    print `cp -r Framework/DirStructure/* $resultsDir/`;
    print "Dublicate STDOUT to the file $mainLog\n";
    open(STDOUT, "| tee -ai $mainLog") or die "Cannot tee to $mainLog : $!";
    
    sleep 0.5;
    print "|-----------------------------|\n";
    print "| MODE : Root files           |\n";
    print "|-----------------------------|\n";

    print "\n";   
    $mode  = 1;

    do "Framework/rootMode.pl";
    do "Framework/loadBalance.pl";
    do "Framework/fileList2.pl";
    ($clusterID) = startRootMode($template, $dataPath, $from, $to, $resultPath, $date, $fileLimit);
    
    print "Jobs are submitted on the cluster: $clusterID \n";

    sub GetReadyOrRemove;
    $numberOfRemovedJobs = GetReadyOrRemove($clusterID);

    CleanDir($mode);

    sub CompileReport;
    $reportStrRoot  = CompileReport;
    
    SendReport($reportStrRoot);

}
elsif(($dataType eq "raw")||($dataType eq "Raw")||($dataType eq "RAW")) {
    
    #RRaw data mode
    print `cp -r Framework/DirStructureRawData/* $resultsDir/`;
    print "Dublicate STDOUT to the file $mainLog\n";
    open(STDOUT, "| tee -ai $mainLog") or die "Cannot tee to $mainLog : $!";


    sleep 0.5;
    print "|-----------------------------|\n";
    print "| MODE : Raw data             |\n";
    print "|-----------------------------|\n";
 
    print "\n";
    $mode=2; 

    do "Framework/rawMode.pl";
    do "Framework/fileListRawData.pl";

    if($inputFile eq ""){
	do "Framework/loadBalanceRawData.pl";
	$clusterID = startRawMode($template, $dataPath, $from, $to, $resultPath, $date, $fileLimit);
    }
    else{
	do "Framework/loadBalance.pl"; # we use this one since we are iterating over a list of tar files
	$clusterID = startRawModeWithTextFile($inputFile, $resultPath, $date, $fileLimit);
    }

    print "Jobs are submitted on the cluster: $clusterID\n";

    sub GetReadyOrRemove;
    $numberOfRemovedJobs = GetReadyOrRemove($clusterID);
    
    CleanDir($mode);

    sub CompileReportRawData;
    $reportStrRaw = CompileReportRawData;
    SendReport($reportStrRaw);


}elsif(($dataType eq "arasim")||($dataType eq "AraSim")||($dataType eq "ARASIM")||($dataType eq "araSim")) {
    
    #AraSim Mode
    print `cp -r Framework/DirStructureAraSim/* $resultsDir/`;
    print "Dublicate STDOUT to the file $mainLog\n";
    open(STDOUT, "| tee -ai $mainLog") or die "Cannot tee to $mainLog : $!";

    sleep 0.5;

    print "|-----------------------------|\n";
    print "| MODE : AraSim               |\n";
    print "|-----------------------------|\n";
    
    print "\n";
    $mode = 3;
   
    sub runAraSim;
    sub getFileList;

    araSimDirExist($araSimPath);
    ($clusterID) = runAraSim($resultsDir, $araSimPath, $date, $araSimSectionNumber);
    print "Jobs are submitted on the cluster: $clusterID\n";

    sub GetReadyOrRemove;
    $numberOfRemovedJobs = GetReadyOrRemove($clusterID);
    print "Normal termination \n";

    $sizeLimit = convertGBtoBytes($sizeLimit);
    
    $isEmpty = isEmptyDir($resultsDir);
    
    my $status;
    
    my $reportName;
    my $level;
    if ( $isEmpty == 1){
	$status = "PROBLEMS";
	$dateEnd = GetDateString();
	($reportName) = writeAraSimReport($resultsDir, $araSimSectionNumber, 0, 0, 0, $argsString, $date, $dateEnd, $status);
	printFailure();
    }else {
	if($numberOfRemovedJob < 1){
	    $status = "GOOD";
	}else {
	    $status = "PROBLEMS";
	}
	($level) = doMerge($resultsDir, $date, $sizeLimit);
	
	($nFiles, $dirSize) = organizeAraSimOutput($resultsDir, ($level - 1));


	$dateEnd = GetDateString();
        ($reportName) = writeAraSimReport($resultsDir, $araSimSectionNumber, $dirSize, $nFiles, 500, $argsString, $date, $dateEnd, $status);
    }

    SendReportTxt($reportName);
    CleanDir($mode);


}else {
    print `cp -r Framework/DirStructureAraSim/* $resultsDir/`;
    print "Dublicate STDOUT to the file $mainLog\n";
    open(STDOUT, "| tee -ai $mainLog") or die "Cannot tee to $mainLog : $!";
    print "|-----------------------------|\n";
    print "| MODE DOES NOT EXIST         |\n";
    print "|-----------------------------|\n";
    print "| EXITING SYSTEM              |\n";
    print "|-----------------------------|\n";
    exit(0);
}

#print "cluster id: ... $clusterID \n";



close(STDOUT);

# Waits MAX_TRIES*TRY_LENGTH [sec] for Condor jobs to be ready, if they are still not remove them
sub GetReadyOrRemove{
	print "|------------------------------|\n";
	print "| Waiting for jobs to complete |\n";
	print "|------------------------------|\n";

	my $clusterID = @_[0];
	my $numberOfRemovedJobs = 0;
	if (($numberOfRemovedJobs = WaitForCondor($clusterID)) > 0) {
		# Remove all timed out tasks
		RemoveCondorJob($clusterID);
		print "Removed jobs = $numberOfRemovedJobs\n";
	}
	return $numberOfRemovedJobs;
}
sub CompileReportRawData {
	print "|------------------------------|\n";
	print "| Preparing and sending report |\n";
	print "|------------------------------|\n";

	# Get time stamp string of the processing end
	$dateEnd = GetDateString();

       	do "Framework/xmlStatistics.pl";
	# Prepare statistics from this execution unit (run.pl)
	my $stat = CreateStat('run.pl');
	#print "Total file $totalFiles \n";
	#print "Number of removed jobs $numberOfRemovedJobs \n";
	$completed = $totalFiles - numberOfRemovedJobs;
	AddValue($stat, {'name'=>'Total submitted', 'digest'=>'copy', 'content'=>$totalFiles});
        #AddValue($stat, {'name'=>'Completed normally', 'digest'=>'copy', 'content'=>$completed});
	AddValue($stat, {'name'=>'Killed/time out', 'digest'=>'sum', 'content'=>$numberOfRemovedJobs});

	AddValue($stat, {'name'=>'Initial command line', 'digest'=>'copy', 'content'=>$argsString});
	AddValue($stat, {'name'=>'Processing start', 'digest'=>'copy', 'content'=>$date});
	AddValue($stat, {'name'=>'Processing end', 'digest'=>'copy', 'content'=>$dateEnd});

	AddValue($stat, {'name'=>'Files in the directory matching template', 'digest'=>'sum', 'content'=>$totNum});

	$statDir = "$Init{'RESULTS_PATH'}/$date/Logs/Stat";
	print "statDir = $statDir\n";
	WriteStat($stat, "$statDir/run.xml");
	

		# Merge all stat-files from all execution units:
	# 	run.pl
	# 	workerScript.pl
	# 	physics analysis executables
	# All stat-files are assumed to be in /Logs/Stat/ directory

	$stat = EmptyStat();
	my $reportShaperPresent = 0;

	opendir STAT_DIR, "$statDir" or die "Cannot open directory $statDir : $!";
	foreach my $name (readdir(STAT_DIR)) {
		next if $name eq "." or $name eq "..";
		next unless $name =~ /\.xml$/;
		# Accumulate all except reportShaper.xml
		$reportShaperPresent = 1 and next if $name eq "reportShaper.xml";
		$statFile = $name;
		print "Accumulating stat-file $statFile\n";
		my $newStat = ReadStat("$statDir/$statFile");
		$stat = AcquireStat($stat, $newStat);
	}

	# Calculate the status of the processing
	my $filesToProcess = scalar(@fileList);

#for raw data case, "PROBLEMS" is determined if there are 1 or more jobs removed
	my $status = ($numberOfRemovedJobs == 0) ? 'GOOD' : 'PROBLEMS';
	AddValue($stat, {'name'=>'Status', 'digest'=>'copy', 'content'=>$status});

	if ($reportShaperPresent) {
		$statFile = "reportShaper.xml";
		print "Accumulating report shaper file $statFile\n";
		my $shaper = ReadStat("$statDir/$statFile");
		$stat = AcquireStat($shaper, $stat);
	}
	closedir STAT_DIR;

	# Sort all values according to their 'order' in ascending order
	# If order is not defined value will appear after those with defined order
	# Special comparison criteria is used to do this
	@{$stat->{'val'}} = sort {my $defa = defined $a->{'order'}; my $defb = defined $b->{'order'}; ($defa and $defb)? $a->{'order'} <=> $b->{'order'}: $defb <=> $defa} @{$stat->{'val'}};

	WriteStat($stat, "$statDir/digest.xml");

	# Create text file based on digest.xml stat-file
	sub StatReport;
	$report = StatReport($stat);
	return $report;

}

# Write stat from this file, accumulate with other, compile report from digest.xml
sub CompileReport {
	print "|------------------------------|\n";
	print "| Preparing and sending report |\n";
	print "|------------------------------|\n";

	# Get time stamp string of the processing end
	$dateEnd = GetDateString();

       	do "Framework/xmlStatistics.pl";
	# Prepare statistics from this execution unit (run.pl)
	my $stat = CreateStat('run.pl');

	AddValue($stat, {'name'=>'Total Root files to process', 'digest'=>'sum', 'content'=>scalar(@fileList)});
	# During the stat digestion job*.xml will subtract Processed successfully from this value
	AddValue($stat, {'name'=>'Not processed/condor section failure', 'digest'=>'sum', 'content'=>scalar(@fileList)});
	
	AddValue($stat, {'name'=>'Total submitted', 'digest'=>'copy', 'content'=>$totalFiles});
        #AddValue($stat, {'name'=>'Completed normally', 'digest'=>'sum', 'content'=> $totalFiles - $numberOfRemovedJobs});
	AddValue($stat, {'name'=>'Killed/time out', 'digest'=>'sum', 'content'=>$numberOfRemovedJobs});

	AddValue($stat, {'name'=>'Initial command line', 'digest'=>'copy', 'content'=>$argsString});
	AddValue($stat, {'name'=>'Processing start', 'digest'=>'copy', 'content'=>$date});
	AddValue($stat, {'name'=>'Processing end', 'digest'=>'copy', 'content'=>$dateEnd});

	AddValue($stat, {'name'=>'Files in the directory matching template', 'digest'=>'sum', 'content'=>$totNum});

	$statDir = "$Init{'RESULTS_PATH'}/$date/Logs/Stat";
	print "statDir = $statDir\n";
	WriteStat($stat, "$statDir/run.xml");

	# Merge all stat-files from all execution units:
	# 	run.pl
	# 	workerScript.pl
	# 	physics analysis executables
	# All stat-files are assumed to be in /Logs/Stat/ directory

	$stat = EmptyStat();
	my $reportShaperPresent = 0;

	opendir STAT_DIR, "$statDir" or die "Cannot open directory $statDir : $!";
	foreach my $name (readdir(STAT_DIR)) {
		next if $name eq "." or $name eq "..";
		next unless $name =~ /\.xml$/;
		# Accumulate all except reportShaper.xml
		$reportShaperPresent = 1 and next if $name eq "reportShaper.xml";
		$statFile = $name;
		print "Accumulating stat-file $statFile\n";
		my $newStat = ReadStat("$statDir/$statFile");
		$stat = AcquireStat($stat, $newStat);
	}

	# Calculate the status of the processing
	my $filesToProcess = scalar(@fileList);
	my $processedSuccessfully = 0; #?

	# Looking for name 'Processed successfully'
	my @array = @{$stat->{'val'}};
	foreach my $value (@array) {
		if ($value->{'name'} eq "Processed successfully") {
			$processedSuccessfully = $value->{'content'};
			last;
		}   
	}

	my $status = ($filesToProcess == $processedSuccessfully) ? 'GOOD' : 'PROBLEMS';
	AddValue($stat, {'name'=>'Status', 'digest'=>'copy', 'content'=>$status});

	if ($reportShaperPresent) {
		$statFile = "reportShaper.xml";
		print "Accumulating report shaper file $statFile\n";
		my $shaper = ReadStat("$statDir/$statFile");
		$stat = AcquireStat($shaper, $stat);
	}
	closedir STAT_DIR;

	# Sort all values according to their 'order' in ascending order
	# If order is not defined value will appear after those with defined order
	# Special comparison criteria is used to do this
	@{$stat->{'val'}} = sort {my $defa = defined $a->{'order'}; my $defb = defined $b->{'order'}; ($defa and $defb)? $a->{'order'} <=> $b->{'order'}: $defb <=> $defa} @{$stat->{'val'}};

	WriteStat($stat, "$statDir/digest.xml");

	# Create text file based on digest.xml stat-file
	sub StatReport;
	$report = StatReport($stat);
	return $report;
}


# Send report.txt as an e-mail to the user(s)
sub SendReport {
	# Send text file to users
        do "Framework/email.pm";
	
	my $report = @_[0];
	$reportFile = "$Init{'RESULTS_PATH'}/$date/Logs/report.txt";
	open(REPORT, ">$reportFile") or die("Cannot open file $reportFile to write a report: $!.\n");
	print(REPORT "ARA offline processing system\n");
	print(REPORT "BEGIN OF REPORT:\n");
	print(REPORT "$report\n");
	print(REPORT "END OF REPORT.");
	close(REPORT);
	print "Report file location: $reportFile\n";
	$subject = "ARA processing results on $date";
	send_mail($subject, $reportFile, $Init{'EMAILS'});
	
        
	print "|---------------------------|\n";
	print "|           Done!           |\n";
	print "|---------------------------|\n";
}

sub SendReportTxt {
    do "Framework/email.pm";
    
    my $report =@_[0];
    $subject = "ARA processing results on $date";
    send_mail($subject, $report, $Init{'EMAILS'});
    

    print "|---------------------------|\n";
    print "|           Done!           |\n";
    print "|---------------------------|\n";

}
# Check if chain of dirictories exist, if not the procedure creates them one-by-one
sub CreateIfNotExist {
        my $path = @_[0];
        my $verbose = @_[1] || 0;
        my @dirs = split /\//, $path;
        my $curPath = "";
        for my $dir (@dirs) {
                if (length($dir) > 0) {
                        $curPath = $curPath . "/" . $dir;
                        if (-d $curPath) {
                                print "directory $curPath - exists\n" if ($verbose);
                        } else {
                                print "directory $curPath - doesn't exist, create: ";#if ($verbose);
                                if (mkdir $curPath) {
                                        print "DONE\n";# if ($verbose);
                                } else {
                                        print "FAILED\n";# if ($verbose);
                                        die "Error! cannot create directory $curPath : $!\n";
                                }
                        }
                }
        }
}

# Return format:   NAME:\tVALUE
sub GetShortRecord {
	my $value = @_[0];
	return "$value->{'name'}:\t$value->{'content'}";
}

sub CleanDir {
    my $myMode = @_[0];
        #removing unwanted files sent back by condor
#    print "$myMode \n";
    if($myMode == 1){
	$fileToDelete= `ls -1 UserCode/RootExe/ | awk '{ ORS=" "; print; }' `;
	`rm $fileToDelete`;
    }
 #   print "in clean dir \n";
    if($myMode == 2){
#	print "in mode 2 \n";
	$fileToDelete= `ls -1 UserCode/RawExe/ | awk '{ ORS=" "; print; }' `;
	`rm current`;
	`rm log.txt`;
	`rm temp1.txt`;
	`rm temp2.txt`;
	`rm $fileToDelete`;
    }
	print $fileToDelete . "\n";

    
#	`rm myL2vrtx_compile*`;
#	`rm GridVtx*`;
#	`rm zout.txt`;
	`rm archive.tar.gz`;
}

# Return format:   NAME:\tVALUE\t( attr: val ... )
sub GetFullRecord {
	my $value = @_[0];
	my $record = GetShortRecord($value);
	if (scalar keys %{$value} > 3) {
		$record .= "\t( ";
		# All additional attributes are printed in brackets
		foreach my $key (keys %{$value}) {
			next if $key eq 'name' or $key eq 'content' or $key eq 'digest';
			$record .= "$key: $value->{$key} ";
		}
		$record .= ")";
	}
	return $record;
}

sub GetTab {
	my $order = @_[0];
	return $order % 100 == 0 ? "" : "\t";
}

sub GetEmptyLines {
	my $order1 = @_[0];
	my $order2 = @_[1];
	return int($order1 / 100) < int($order2 / 100) ? "\n".GetEmptyLines($order1, $order2 - 100) : "";
}

sub StatReport {
	my $stat = @_[0];
	my $report = "";
	my $oldOrder = 0;
	foreach my $elem (@{$stat->{'val'}}) { # Default value representation NAME: VALUE
		if (defined $elem->{'order'}) {
			$curOrder = $elem->{'order'};
			$report .= GetEmptyLines($oldOrder, $curOrder).GetTab($curOrder).GetShortRecord($elem);
			$oldOrder = $curOrder;
		} else {
			$report .= "\t".GetFullRecord($elem);
		}
		$report .= "\n";
	}   
	return $report;
}

1;
