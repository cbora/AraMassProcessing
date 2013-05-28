#!/usr/bin/perl

use File::Basename;

# This function finds files that match the template (@_[1]),
# located recursevly in directory @_[0],
# the basename of this files should be equal or greater than $to and
# less or equal to $from (lexically)
sub FindFiles {
	my $directoryPath = @_[0];#"/data/exp/ARA/2012/filtered/L0/0101/";
	my $rootFileExtension = @_[1];#"*.root";
	my $from = @_[2];
	my $to = @_[3];

	my $oneLineFindResults = `find $directoryPath -name \"$rootFileExtension\"`;
	my @findResults = split /\n/, $oneLineFindResults;
	
	my @fileList;

	for my $full (@findResults) {
		$basename = basename($full);
		if ((!defined($from) or $basename ge $from) and (!defined($to) or $basename le $to)) {
			push(@fileList, $full);
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

#($ref1, $ref2) = FindFiles("/data/exp/ARA/2012/filtered/L0/", "*.root", "run010944.root", "run010955.root");
#@list = @$ref1;
#$tot = $$ref2;
#print "@list\n";
#print "$tot\n";
#PrintFileListToFile(\@list, \"fileList.tmp");
