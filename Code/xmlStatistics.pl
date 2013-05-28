#!/usr/bin/perl

use Switch;
use XML::Simple;
use Data::Dumper;

# Return new empty statistics (reference to empty hash)
sub EmptyStat {
    return {};
}

# Return new statistics with two attribute in the root tag
sub CreateStat {
    my $fileName = @_[0];
    my $res = {'inputFile'=>"$fileName", 'totalFiles'=>1};
    return $res;
}

# Add a value (hash reference) into statistics
sub AddValue {
    my $stat = @_[0];
    my $value = @_[1];
    push(@{$stat->{val}}, $value);
}

sub ReadStat {
	my $fileName = @_[0];
	$parsedXml = XMLin($fileName, keyattr => [], forcearray => 1);
	#print Dumper($parsedXml);
	return $parsedXml;
}

sub WriteStat {
	my $fileName = @_[1];
	my $parsedXml = @_[0];
	#print Dumper($parsedXml);
	open my $fh, '>:encoding(iso-8859-1)', "$fileName" or die "Cannot open file $fileName : $!";
	if (not XMLout($parsedXml, RootName => 'stat', OutputFile => $fh)) {
		die "Cannot produce xml-statistics in file $parsed : $!";
	}
}

# This function summarize two value tags of the same name into one
sub DigestValues {
	my $val1 = @_[0];
	my $val2 = @_[1];
	my $inFile1 = @_[2];
	#my $inFile2 = @_[3];
	my $res = $val1;
	my $digestType = $val1->{digest};
	if ($digestType ne $val2->{digest}) {
		print "Warning: different digests with equal names: $digestType, $val2->{digest}\n";
	}
	my $content1 = $val1->{content};
	my $content2 = $val2->{content};
	my $elem1 = $val1->{values} || 1;
	my $elem2 = $val2->{values} || 1;
	$res->{values} = $elem1 + $elem2;
	switch ($digestType) {
		case 'sum' {	# Add two numbers in content
			$res->{content} = $content1 + $content2;
		} 
		case 'conc' {	# Concatenate two string in content
			$res->{content} = $content1 . $content2;
		}
		case 'concCR' {	# Concatenate content through Carriage Return in the middle
			$res->{content} = $content1 . "\n" . $content2;
		}
		case 'ave' {	# Average values using 'values' and 'sum' arguments
			# Weighted average can be calculated if argument 'values' contains 
			# weigth instead of number of accumulated value tags
			my $sum1 = $val1->{sum} || $content1 * $elem1;
			my $sum2 = $val2->{sum} || $content2 * $elem2;
			#print "sum1 = $sum1, sum2 = $sum2\n";
			$res->{sum} = $sum1 + $sum2;
			$res->{content} = ($sum1 + $sum2) / ($elem1 + $elem2);
		}
		case 'min' {	# Numerically minimum content wins, copy all its arguments
			$res = $content2 < $content1 ? $val2 : $val1;
		}
		case 'max' {	# Numerically maximum content wins, all its arguments propagate
			$res = $content2 > $content1 ? $val2 : $val1;
		}
		case 'join' {	# Join arguments, take second in case of conflict
			my %tmp = %$val1;
			@tmp{keys %{$val2}} = values %{$val2};
			$res = {%tmp};
		}
		else {	# Copy all from first value
			$res = $val1;
			$res->{inputFile} = $inFile1;
		}
	}
	return $res;
}

sub AcquireStat {
	my $old = @_[0];	# Assumption: old data structure do not need to be summarized 
	my $new = @_[1];
	my $res = {};

	#delete $res->{inputFile};	# Don't want to preserve long list of all input files
	$res->{totalFiles} = $old->{totalFiles} + $new->{totalFiles};
	my $inFile1 = $old->{inputFile};
	my $inFile2 = $new->{inputFile};
	if (not defined $old->{val} and not defined $new->{val}) {
		return $res;
	}
	if (not defined $old->{val}) {
		$res->{val} = $new->{val};
		return $res;
	}
	foreach $val (@{$old->{val}}) {	# Iterate through all values in old statistics
		if (defined $val->{name}) {
			$name = $val->{name};
			$summary = $val->{digest};
			$summary = 'copy' if not defined $summary;
			next if $summary eq 'omit';
			if ($summary eq 'copy') {
				push(@{$res->{val}}, $val);
				next;
			}
			# Find the same name in new stat
			my $resVal = $val;
			foreach my $newVal (@{$new->{val}}) {
				$newName = $newVal->{name};
				next if not defined $newName;
				if ($name eq $newName) { # Summarize two value tags
					$resVal = DigestValues($resVal, $newVal, $inFile1);
					$resVal = {%$resVal}; # Shallow hash copy, might not work for nested data structures
					undef $newVal->{name};	# To skip in second loop
					#print "$name\n";
				}
			}
			push(@{$res->{val}}, $resVal);
		}
		#print "$value\n";
	}
	#print Dumper($new);
	
	# Second loop: copy the rest of values from second stat-xml
	for (my $i = 0; $i < scalar(@{$new->{val}}); $i++) {
	#foreach my $newVal (@{$new->{val}}) {
		$newVal = @{$new->{val}}[$i];
		$newName = $newVal->{name};
		next if not defined $newName or (defined $newVal->{digest} and $newVal->{digest} eq 'omit');
		#print "i = $i, additional value = $newName\n";
		my $resVal = {%$newVal};
		if (not defined $newVal->{digest} or $newVal->{digest} eq 'copy') {
			$resVal->{inputFile} = $inFile2;
		} else {
			for (my $j = $i+1; $j < scalar(@{$new->{val}}); $j++) {
				$nextVal = @{$new->{val}}[$j];
				$nextName = $nextVal->{name};
                                next if not defined $nextName;
				#print "j = $j, search name = $nextName\n";
                                if ($newName eq $nextName) { # Summarize two value tags
                                        #print "Matched name = $newName\n";
                                        $resVal = DigestValues($resVal, $nextVal, $inFile2);
                                        $resVal = {%$resVal}; # Shallow hash copy, might not work wor nested data structures
                                        undef $nextVal->{name};  # To skip in second loop
                                }

			}
		}
		push(@{$res->{val}}, $resVal);
	}
	#my $fileName = @_[0];
	#my $parsed = ReadStat($fileName);#XMLin($fileName, keyattr => []);
	#WriteStat($parsed, 'stat3.xml');
	return $res;
}

#$processed = 'reportShaper.xml';
#$newXml = 'digest.xml';
#$outXml = 'stat3.xml';
#$stat = AcquireStat(ReadStat($processed), ReadStat($newXml));
#WriteStat($stat, $outXml);
#print Dumper($stat);
#print "Done\n";
