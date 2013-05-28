#!/usr/bin/perl
package config;
require Exporter;

our @ISA = qw(Exporter);
our @EXPORT = qw(%init);

%init = ();
# This script contains configuration constants
# that will be loaded at the beginning

# default path to the directory with input L0 data (only absolute path)
$DEFAULT_DATA_PATH = "/data/exp/ARA/2011/filtered/L0/";
$init{'DATA_PATH'} = $DEFAULT_DATA_PATH;

# default results path (relative part is allowed)
$DEFAULT_RESULTS_PATH = "./Data/";
$init{'RESULTS_PATH'} = $DEFAULT_RESULTS_PATH;

# default maximum input file to run in one condor-job
$DEFAULT_FILE_LIMIT = 20;
$init{'FILE_LIMIT'} = $DEFAULT_FILE_LIMIT;

# default number of waiting periods for condor to complete
$init{'MAX_TRIES'} = 1440; # 24 hours

# default time length between checking condor tries in sec.
$init{'TRY_LENGTH'} = 60;	# 1 min

# default email addresses to send report about processing results
# escape [at] symbol with a backslash (\@)
# list multiple addresses using comma (test1\@wisc.edu, test2\@wisc.edu)
$DEFAULT_EMAILS = "";#"savdeev\@cse.unl.edu";
$init{'EMAILS'} = $DEFAULT_EMAILS;

# default template for searching data-files
$init{'TEMPLATE'} = '*.root';
