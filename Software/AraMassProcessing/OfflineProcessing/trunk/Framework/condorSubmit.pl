#!/usr/bin/perl
# This script submit jobs to condor system
# Expects one command line argument
# First - number of jobs to create
# this number should match number of job[0-9]*.txt files in the location defined in 
# condor submit file

#$n = shift || die "Argument missing: number of jobs to launch\n";
#print "Number of jobs to submit: $n\n";

#use main;
#print "\n";
sub checkCondorJob;

# There are a bunch of different functions to submit jobs on condor
# I prefered this instead of many if statement cause it makes debugging easier

sub SubmitCondorJob{
    #Function to submit Root file mode jobs on condor - 12 hours max per section
	my $n = @_[0];
	my $dataPath = @_[1];
	#my $executionTime = @_[2];
	    
	print "n = $n, dataPath = $dataPath\n";
#	print "$executionTime \n";

	#make an archive for the files in the UserCode directory
	`tar -zcf archive.tar.gz -C UserCode/RootExe/ .`;
       
	my $submitFile = "Framework/parallelJobs.condor";
	

	print "CONDOR SHORT execution time \n";
	my $submitCommand = "condor_submit ".
		"-append 'log        = $dataPath/Logs/System/log.\$(Process)' ".
		"-append 'output     = $dataPath/Logs/System/out.\$(Process)' ".
		"-append 'error      = $dataPath/Logs/System/err.\$(Process)' ".
		"-append 'transfer_input_files = $dataPath/Input/job\$(Process).txt, Framework/xmlStatistics.pl, Framework/loadlib.h, Framework/vtxgrid.h, Framework/vtxlib.h, Framework/vtxlibvars.h, Framework/vtxutil.h, Framework/rootlogon.C, archive.tar.gz' ".
		"-append 'arguments = \$(Process) $dataPath/' ".
		"-append 'queue $n' $submitFile";

	#print "$submitCommand\n";
	#$cmd= `pwd`;
	#$cmd1 = "cd $thisJobDir";
	#$cmd2 = "condor_submit parallelJobs.condor";
	my $getClusterID = "tail -1 | awk -F \"cluster \" \'{print \$2}\'";
	my $clusterId = `$submitCommand | $getClusterID`;
	print "clusterId = $clusterId\n";
	chomp($clusterId);
	return $clusterId;
}

sub SubmitCondorJobLong{
    #this function is for submitting condor job to execute to a maximum of 24 hours
    #because the default execution time is 12hours
    

    my $n = @_[0];
    my $dataPath = @_[1];
    #my $executionTime = @_[2];
    my $username =`whoami`;	    
    print "n = $n, dataPath = $dataPath\n";

    print "username: $username \n";

    #generating string for AccountingGroup varibale
    
    my $userString = "long.".$username;
    print "userString is : $userString \n";
    chomp($userString);
    $userString = "\"".$userString."\"";
    print "New userString : $userString \n";

#		"-append '\+AccountingGroup = \"long.\$(username)\"' ".
	#make an archive for the files in the UserCode directory
    `tar -zcf archive.tar.gz -C UserCode/RootExe/ .`;
       
    my $submitFile = "Framework/parallelJobs.condor";


    print "CONDOR LONG execution time \n";
    my $submitCommand = "condor_submit ".
		"-append 'log        = $dataPath/Logs/System/log.\$(Process)' ".
		"-append 'output     = $dataPath/Logs/System/out.\$(Process)' ".
		"-append 'error      = $dataPath/Logs/System/err.\$(Process)' ".
		"-append 'transfer_input_files = $dataPath/Input/job\$(Process).txt, Framework/xmlStatistics.pl, Framework/loadlib.h, Framework/vtxgrid.h, Framework/vtxlib.h, Framework/vtxlibvars.h, Framework/vtxutil.h, Framework/rootlogon.C, archive.tar.gz' ".
		"-append 'arguments = \$(Process) $dataPath/' ".
		"-append '\+AccountingGroup = $userString' ".
		"-append 'queue $n' $submitFile";

    my $getClusterID = "tail -1 | awk -F \"cluster \" \'{print \$2}\'";
    my $clusterId = `$submitCommand | $getClusterID`;
    print "clusterId = $clusterId\n";
    chomp($clusterId);
    return $clusterId;
}

sub SubmitCondorJobRawData{
    #Funtion to submit Raw data jobs on condor
	my $n = @_[0];
	my $dataPath = @_[1];
	print "n = $n, dataPath = $dataPath\n";

	#make an archive for the files in the UserCode directory
	`tar -zcf archive.tar.gz -C UserCode/RawExe/ .`;
#	print " in raw dataa submit file \n";

	my $submitFile = "Framework/parallelJobsRawData.condor";
	my $submitCommand = "condor_submit ".
		"-append 'log        = $dataPath/Logs/System/log.\$(Process)' ".
		"-append 'output     = $dataPath/Logs/System/out.\$(Process)' ".
		"-append 'error      = $dataPath/Logs/System/err.\$(Process)' ".
		"-append 'transfer_input_files = $dataPath/Input/job\$(Process).txt, Framework/xmlStatistics.pl, Framework/rootlogon.C, archive.tar.gz' ".
		"-append 'arguments = \$(Process) $dataPath/' ".
		"-append 'request_disk = 40000000' ".
		"-append 'queue $n' $submitFile";

	#print "$submitCommand\n";
	#$cmd= `pwd`;
	#$cmd1 = "cd $thisJobDir";
	#$cmd2 = "condor_submit parallelJobs.condor";
	my $getClusterID = "tail -1 | awk -F \"cluster \" \'{print \$2}\'";
	my $clusterId = `$submitCommand | $getClusterID`;
	print "clusterId = $clusterId\n";
	chomp($clusterId);
	return $clusterId;
}
#use Data::Dumper;

sub SubmitCondorJobAraSim{
    #Function to submit ARASIM jobs on condor
    my $n = @_[0];
    my $dataPath = @_[1];
#    my $level = @_[2];
    my $level = 0; #we dont care about the level of merging when running arasim
    my $flag = 0; # to tell workerscript that it should run arasim
    

    print "n = $n, dataPath = $dataPath\n";

    $logFile ="/Logs/System/logAraSim_";
    $errFile ="/Logs/System/errAraSim_";
    $outFile ="/Logs/System/outAraSim_";

#        `cd ./Temp/`;
    my $submitFile = "Framework/parallelJobsAraSim.condor";
        my $submitCommand = "condor_submit ".
                "-append 'log        = $dataPath/$logFile.\$(Process)' ".
                "-append 'output     = $dataPath/$outFile.\$(Process)' ".
                "-append 'error      = $dataPath/$errFile.\$(Process)' ".
                "-append 'transfer_input_files = Framework/runAraSim.sh, UserCode/AraSimExe/mergeTree.C, archive.tar.gz' ".
                "-append 'arguments = $dataPath \$(Process) $level $flag' ".
		"-append 'request_disk = 50000000' ".
                "-append 'getEnv = TRUE' ".
                "-append 'queue $n' $submitFile";

    my $getClusterID = "tail -1 | awk -F \"cluster \" \'{print \$2}\'";
    my $clusterId = `$submitCommand | $getClusterID`;
    print "clusterId = $clusterId\n";
    chomp($clusterId);

    return $clusterId;

}

sub SubmitCondorJobMerge{
    #Function to submit merge jobs in the mode of ARASIM
    my $n = @_[0];
    my $dataPath = @_[1];
    my $level = @_[2];
    my $flag = 1; # to tell workerscript that it should run merger
    my $size =@_[3];

    
#    $size = $size . "000000";
#    $size = 40000000;
    print "n = $n, dataPath = $dataPath\n";
    $dataPathInput = "$dataPath/Input/";
    $inFile = "inputFile_".$level."_";
    $dataPathLog = "$dataPath/Logs/System/merge_";
    $logFile ="log_".$level . "_";
    $errFile ="err_".$level . "_";
    $outFile ="out_".$level . "_";

    my $submitFile = "Framework/parallelJobsAraSim.condor";
        my $submitCommand = "condor_submit ".
                "-append 'log        = $dataPathLog$logFile.\$(Process)' ".
                "-append 'output     = $dataPathLog$outFile.\$(Process)' ".
                "-append 'error      = $dataPathLog$errFile.\$(Process)' ".
                "-append 'transfer_input_files = $dataPathInput/$inFile\$(Process).txt, UserCode/AraSimExe/mergeTree.C, Framework/runAraSimMerger.sh' ".
                "-append 'arguments = $dataPath \$(Process) $level $flag $dataPathInput/$inFile\$(Process).txt' ".
		"-append 'request_disk = 50000000' ".
                "-append 'getEnv = TRUE' ".
                "-append 'queue $n' $submitFile";

    my $getClusterID = "tail -1 | awk -F \"cluster \" \'{print \$2}\'";
    my $clusterId = `$submitCommand | $getClusterID`;
    print "clusterId = $clusterId\n";
    chomp($clusterId);
    return $clusterId;
}



sub WaitForCondor{
	my $cluster = @_[0];#SubmitCondorJob($n);
	my $maxTries = $main::Init{'MAX_TRIES'};#100;	# Number of tries before give up
	my $deltaTTry = $main::Init{'TRY_LENGTH'};#60;	# Seconds between tries
	#print Dumper({%main::Init});
	my $hangingTaskLeft = 1;
	#print "Waiting for jobs to complete...\n";
	for( $iTry = 1; $iTry <= $maxTries; $iTry++ ) {
		sleep $deltaTTry;
		$timeByNow = $iTry * $deltaTTry;
		print " Try $iTry: time of processing $timeByNow seconds\n";
		print "   Check job $cluster....";
		#print "*** CALL clusterID = $cluster ***\n";
		($status, $hangingTaskLeft) = checkCondorJob($cluster);
		#print "*** RESULT = $status ***\n";
		if ( $status == 0 ) {
			# This job is still in progress
			#$allDone = 0;
			print "in progress\n";
		} elsif ( $status == 1 ) {
			# This job is finished
			print "done\n";
			$hangingTaskLeft = 0;
			last;
		} else {
			# Problems contacting the condor scheduler
			print "failed to contact condor scheduler, giving up\n";
		}
	}
	return $hangingTaskLeft;
}

# Remove codor jobs
sub RemoveCondorJob {
	my $thisJobId = @_[0]+0;
	my $status = system("condor_rm $thisJobId");
	if ($status <=> 0) {
		print "Cannot remove condor job $thisJobId .\n";
	} else {
		print "Condor job $thisJobId was removed.\n";
	}
}

sub checkCondorJob{
    # This subroutine checks if a job with the given Condor ID
    # is still in the queue
    # Arguments: condorId
    my $condorId = @_[0];
    my $failedToContact = "Failed to fetch ads";

    my $outline = `condor_q $condorId 2>&1`;
    #print "DEBUG: output of condor_q\n";
	#print $outline;
    print "\nDEBUG: end of output of condor_q\n";
    my @tmp = split("\n",$outline);
    my $counter = 0;
	my $tasksRemain = 0;
	for my $tmp2 (@tmp) {
		next if length($tmp2) == 0;
		$counter = $counter + 1;
		next if $counter < 3;
		#print "$tmp2\n";
		$tasksRemain = $tasksRemain + 1;
	}
	my @tmpCheckFailure = grep(/$failedToContact/,@tmp);
    my $isSchedulerResponding = 1;
    my $status = 0;
    #print "DEBUG: tmp is \n";
    #print @tmp;
    #print "DEBUG: end tmp\n";
    #print "DEBUG: tmpCheckFailure length is $#tmpCheckFailure\n";
    if( $#tmpCheckFailure != -1 ) {
        print "\n                       ... failed to contact condor, sleep 5min...";
        sleep(300);
        $outline = `condor_q $condorId`;
        @tmp = split("\n",$outline);
        @tmpCheckFailure = grep(/$failedToContact/,@tmp);
        if( $#tmpCheckFailure != -1 ) {
            print "\n                       ... failed to contact condor, sleep 5min...";
            sleep(300);
            $outline = `condor_q $condorId`;
            @tmp = split("\n",$outline);
            @tmpCheckFailure = grep(/$failedToContact/,@tmp);
            if( $#tmpCheckFailure != -1 ) {
                print "\n                       ... final failure to contact condor ...";
                print "\n";
                $isSchedulerResponding = 0;
            }
        }
    }
    #print "DEBUG: isSchedulerResponding = $isSchedulerResponding\n";
    
    if ( $isSchedulerResponding == 1 ) {
        @tmpCheckJobThere = grep(/$condorId/,@tmp);
        if( $#tmpCheckJobThere >= 0 ) {
            #print "DEBUG: The job is still running\n";
            $status = 0;
        } else {
            #print "Debug: The job has finished\n";
            $status = 1;
        }
    }else{
        $status = -1;
    }
    #print "DEBUG: returning status = $status\n";
    #my $status = system("condor_q $condorId | grep $condorId >& /dev/null");
    return ($status, $tasksRemain);
}

1;
