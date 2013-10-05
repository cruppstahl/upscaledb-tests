#!/usr/bin/env perl

use strict;
use App::Rad;
use Term::ANSIColor;

my $errors = 0;
my $total = 0;

sub run_single_test {
  my $valgrind = shift;
  my $file = shift;
  my $params = shift;

  if ($valgrind) {
    open(FH, ">>valgrind.txt") or die "Cannot open valgrind.txt for writing";
  }
  else {
    open(FH, ">>monster.txt") or die "Cannot open monster.txt for writing";
  }
  print "[START] ./ham_bench --quiet $params $file\n";

  my $fail = 0;
  my $output;
  if ($valgrind) {
    $output = `valgrind --tool=memcheck --suppressions=valgrind.suppress ./ham_bench --quiet $params $file 2>&1`;
    #$fail++ unless $output =~ /ERROR SUMMARY: 0 errors from 0 contexts/;
    $fail++ unless $output =~ /in use at exit: 0 bytes in 0 blocks/;
  }
  else {
    $output = `./ham_bench --quiet $params --use-berkeleydb=true --reopen=true $file`;
  }
  print "[STOP]  ./ham_bench --quiet $params $file\n";
  print FH "./ham_bench --quiet $params $file\n";
  print FH $output;
  print FH "\n";
  $fail++ if ($output =~ /FAIL/);
  $total++;
  $errors++ if $fail;
  close(FH);
}

sub run_directory {
  my $dryrun = shift;
  my $valgrind = shift;
  my $maxdir = shift;
  my $options = shift;

  print "[CONFIGURATION] $options\n";

  if ($valgrind) {
    my @files = ('1/45.tst', '1/220.tst', '1/ext_020.tst', '2/ext_060.tst',
        '2/blb-001.tst');
    foreach my $file (@files) {
      run_single_test($valgrind, "../testfiles/$file", $options);
      return if $dryrun;
    }
  }
  else {
    for (my $i = 1; $i <= $maxdir; $i++) {
      my @files = <../testfiles/$i/*.tst>;
      foreach my $file (@files) {
        run_single_test($valgrind, $file, $options);
        return if $dryrun;
      }
    }
  }
}

sub run {
  my $c = shift;
  my $dryrun = $c->options->{'dryrun'};

  `rm -f *.db* monster.txt`;

  if ($c->options->{'dir'}) {
    run_directory($dryrun, 0, $c->options->{'dir'}, $c->options->{'options'});
  }
  else {
    open FILE, 'monster.lst' or die "Couldn't open file monster.lst: $!";
    foreach (<FILE>) {
      if (/(\d), \"(.*?)\"$/) {
        run_directory($dryrun, 0, $1, $2);
      }
    }
  }

  print "\n$errors of $total tests failed\n";
  return '';
}

sub valgrind {
  my $c = shift;
  my $dryrun = $c->options->{'dryrun'};

  `rm -f *.db* valgrind.txt`;

  open FILE, 'valgrind.lst' or die "Couldn't open file valgrind.lst: $!";
  foreach (<FILE>) {
    if (/(\d), \"(.*?)\"$/) {
      run_directory($dryrun, 1, $1, $2);
    }
  }

  print "\n$errors of $total tests failed\n";
  return '';
}

sub setup {
  my $c = shift;
  $c->register_commands( {
        run => "runs the monster tests (can take a day or two...;\n" .
                "\t\targuments: --dir=1|2|3|4; --options=OPTIONS)",
        valgrind => 'runs assorted tests in valgrind' 
    });
}

App::Rad->run();
