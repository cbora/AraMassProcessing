#!/usr/bin/perl
# This script takes list of the files to process and divide it in several lists
# It expects one command line parameter
# Input file name - list of names of files to process

#$input = shift || die "Missing argument: input file name.\n";
#$outputPath = shift || die "Missing argument: path for output files.\n";

#do 'Code/config.pl'; # Initialize configuration variables

# This function separates input file list into several file lists
sub SeparateTasks{
	my $input = @_[0];
	my $outputPath = @_[1];
	my $limit = @_[2];	# The limit of number of files to be sent to one worker node
	print "Separating jobs for condor...\n";
	print "Input list file  : $input\n";
	print "Output file path : $outputPath\n";

	$counter = 0;
	$prefix = "$outputPath/job"; # New list will be like "job0.txt"
	$postfix = ".txt"; 
	$output = undef;

	open(FILE, "$input");

	while (<FILE>)
	{
	  if ($counter % $limit == 0)
	  { # Current file is at the limit, start new file to continue
	    #print "New File: $prefix". int($counter/$limit) . "$postfix\n";
	    if (defined($output))
	    { # If some file existed before, then close it
	      close(OUTPUT);
	    } # And open new file with the next index
	    $output = open(OUTPUT, ">$prefix". int($counter/$limit) . "$postfix\n");
	  }
	  chomp;
	  print OUTPUT "$_\n";
	  $counter = $counter + 1;
	} 

	close(FILE);

	$totalFiles = 1 + int(($counter - 1)/$limit);
	print "Job files created: ", $totalFiles, "\n";
	return $totalFiles;
}
