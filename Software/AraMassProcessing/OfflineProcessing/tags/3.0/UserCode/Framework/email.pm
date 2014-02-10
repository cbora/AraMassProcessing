#package email;

# Necessary lines
#use Exporter;
#@ISA = ('Exporter');

# Define functions/variables for public use
#@EXPORT = qw(&send_mail);

#--------------------------------#
# All user code goes below       #
#--------------------------------#

#do "Code/config.pl";

#print "email = $DEFAULT_EMAILS\n";
#print "list = @list\n";

sub send_mail{
  # Send email to the list of people found
  # in the file $RICE_ANALYSIS_CFGDIR/email.cfg
  # Argument list: email subject
  #                txt filename to be sent via email
  #                string listing recipients
  $subject = @_[0];
  $file = @_[1];
  $recipients = @_[2];
  if (length($recipients) == 0) {
    print "No recipients to send to.\n";
    return;
  }
  @list = split(',', $recipients);
  #$riceCfgDir = $ENV{RICE_ANALYSIS_CFGDIR} || ($status = -1);
  #if($status == -1){
  #  print "email:ERROR: env. variable RICE_ANALYSIS_CFGDIR",
  #    " is not defined, can not send e-mail\n";
  #  return;
  #}
  #$addressFile = sprintf("%s/email.cfg", $riceCfgDir);
  #if( !( -e $addressFile) ){
  #  print "email:ERROR: can not open address file $addressFile\n";
  #  return;
  #}
  #if( !( -e $file) ){
  #  print "email:ERROR: can not open the file to be sent\n";
  #  return;
  #}
  # Send email to each person in the email list
  #open(ADDR, $addressFile);
  @lines = @list;
  #close(ADDR);
  foreach $person (@lines){
    $mailCommand = "mail -s \"$subject\" ";
    system("more $file | $mailCommand $person");
  }
  print "Email has been sent: subject=$subject\n",
    "           file=$file\n";
  return;
}


#----------------------------------------#
# This line is required to be the last   #
#----------------------------------------#

1;
