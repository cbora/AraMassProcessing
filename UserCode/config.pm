#!/usr/bin/perl
package config;
require Exporter;

our @ISA = qw(Exporter);
our @EXPORT = qw(%init);

%init = ();
# This script contains configuration constants
# that will be loaded at the beginning

#********************************************************
#           Global configuration
#********************************************************

#this can process raw data and root files
#for raw data => 'raw'
#for root files => 'root'
#dor arasim => 'arasim'
$DEFAULT_MODE="root";
$init{'MODE'}=$DEFAULT_MODE;


# default results path (relative part is allowed)
#$DEFAULT_RESULTS_PATH = "./Data/";
$DEFAULT_RESULTS_PATH = "./Data/";
$init{'RESULTS_PATH'} = $DEFAULT_RESULTS_PATH;


# default number of waiting periods for condor to complete
$init{'MAX_TRIES'} = 10080000000; #1440; # 24 hours

# default time length between checking condor tries in sec.
$init{'TRY_LENGTH'} = 60;	# 1 min

# default email addresses to send report about processing results
# escape [at] symbol with a backslash (\@)
# list multiple addresses using comma (test1\@wisc.edu, test2\@wisc.edu)
$DEFAULT_EMAILS = "borachristian\@gmail.com";#"borachristian\@gmail.com";
$init{'EMAILS'} = $DEFAULT_EMAILS;

#execution time of a single job
#12 hours (default) => short
#24 hours => long (this has fewer slots available for the jobs submitted)
#this feature only available for ROOT mode
$DEFAULT_EXECUTION_TIME="short";
$init{'EXECUTION_TIME'} = $DEFAULT_EXECUTION_TIME;



#**************************************************
#            ROOT & RAW data Mode configuration
#**************************************************
# default template for searching data-files
#$init{'TEMPLATE'} = '*.root';
#$init{'TEMPLATE'} = '*.TestBed.L1.root'; #template for 2012 L1 data files 
#$init{'TEMPLATE'} = "*.dat";
$init{'TEMPLATE'} ="event1*.root";

# default path to the directory with input L0 data (only absolute path)
#$DEFAULT_DATA_PATH = "/data/exp/ARA/2011/filtered/L0/"; # for 2011 L0data files
$DEFAULT_DATA_PATH = "/data/exp/ARA/2012/filtered/L0/"; #for 2012 L0 data files
$init{'DATA_PATH'} = $DEFAULT_DATA_PATH;


# default maximum input file to run in one condor-job
$DEFAULT_FILE_LIMIT = 2;
$init{'FILE_LIMIT'} = $DEFAULT_FILE_LIMIT;


#************************************************
#            ARASIM Mode configuration
#************************************************
#where AraSim is installed
$DEFAULT_ARASIM_DIR ="/home/cbora/Software/AraSim/";
$init{'ARASIM_DIR'}=$DEFAULT_ARASIM_DIR;


#size limit for files after merging
#in GB 
$DEFAULT_SIZE_LIMIT="10";
$init{'SIZE_LIMIT'}=$DEFAULT_SIZE_LIMIT;


#Number of sections/job (the same setup file will be used on all condor jobs) -
# make sure the random generator is on to avoid generation of duplicated data on all nodes
$DEFAULT_NUMBER_SECTION=4;
$init{'NUMBER_SECTION'}=$DEFAULT_NUMBER_SECTION;












