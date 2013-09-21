#!/usr/bin/env perl

use strict;
use App::Rad;
use Term::ANSIColor;

my $errors = 0;
my $total = 0;

sub run_single_test {
  my $file = shift;
  my $params = shift;

  open(FH, ">>monster.txt") or die "Cannot open monster.txt for writing";
  print "[START] ./test $params $file\n";
  my $output = `./test $params $file`;
  print "[STOP]  ./test $params $file\n";
  print FH "./test $params $file\n";
  print FH $output;
  print FH "\n";
  $errors++ if ($output =~ /FAIL/);
  $total++;
  close(FH);
}

sub run_directory {
  my $dryrun = shift;
  my $maxdir = shift;
  my $options = shift;

  print "[CONFIGURATION] $options\n";
  for (my $i = 1; $i <= $maxdir; $i++) {
    my @files = <../../testfiles/$i/*.tst>;
    foreach my $file (@files) {
      run_single_test($file, $options);
      return if $dryrun;
    }
  }
}

sub run {
  my $c = shift;
  my $dryrun = $c->options->{'dryrun'};

  `rm -f *.db* monster.txt`;

  open FILE, 'monster.lst' or die "Couldn't open file monster.lst: $!";
  foreach (<FILE>) {
    if (/(\d), \"(.*?)\"$/) {
      run_directory($dryrun, $1, $2);
    }
  }

  print "\n$errors of $total tests failed\n";
  return '';
}

sub setup {
  my $c = shift;
  $c->register_commands( {
        run => 'runs the monster tests (can take a day or two...)'
    });
}

App::Rad->run();
