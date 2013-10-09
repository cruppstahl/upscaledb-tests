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

  # if filename starts with "ext_": enable extended keys
  $params = "--use-extended $params" if ($file =~ /ext_/);

  if ($valgrind) {
    open(FH, ">>valgrind.txt") or die "Cannot open valgrind.txt for writing";
  }
  else {
    open(FH, ">>monster.txt") or die "Cannot open monster.txt for writing";
  }

  my $output;
  my $fail = 0;
  my $real_params;

  if ($valgrind) {
    $real_params = "--quiet $params $file";
    print "[START] ./ham_bench $real_params $file\n";
    $output = `valgrind --tool=memcheck --suppressions=valgrind.suppress ./ham_bench $real_params 2>&1`;
  }
  else {
    # do not reopen file if 'inmemorydb' is set
    if ($params =~ /inmemorydb/) {
      $real_params = "--quiet --use-berkeleydb=true $params";
    }
    else {
      $real_params = "--quiet --use-berkeleydb=true --reopen=true $params";
    }
    print "[START] ./ham_bench $real_params $file\n";
    $output = `./ham_bench $real_params $file`;
  }


  if ($valgrind) {
    $fail++ unless $output =~ /ERROR SUMMARY: 0 errors from 0 contexts/;
    $fail++ unless $output =~ /in use at exit: 0 bytes in 0 blocks/;
  }

  $fail++ if $?;
  $fail++ if ($output =~ /FAIL/);
  $total++;
  $errors++ if $fail;

  print FH "./ham_bench $real_params $file\n";
  print FH $output;
  if ($fail) {
    print FH "[FAIL]\n";
    print "[FAIL]\n";
  }
  else {
    print FH "\n";
    print "[OK]\n";
  }
  
  close(FH);
}

sub run_directory {
  my $dryrun = shift;
  my $valgrind = shift;
  my $maxdir = shift;
  my $options = shift;

  print "[CONFIGURATION] $options\n";

  if ($valgrind) {
    if ($maxdir == 1) {
      # run tests with scripts
      my @files = ('1/45.tst', '1/220.tst', '1/ext_020.tst', '2/ext_060.tst',
        '2/blb-001.tst');
      foreach my $file (@files) {
        run_single_test($valgrind, "../testfiles/$file", $options);
        return if $dryrun;
      }
    }
    else {
      # or with generated data
      run_single_test($valgrind, "--stop-ops=500000", $options);
    }
  }
  else {
    if ($maxdir == 0) {
      # run tests with random data
      my $max = 1000000;
      $max = 1000 if $dryrun;
      run_single_test(0, "--stop-ops=$max", $options);
    }
    else {
      # or with generated data
      for (my $i = 1; $i <= $maxdir; $i++) {
        my @files = <../testfiles/$i/*.tst>;
        foreach my $file (@files) {
          run_single_test(0, $file, $options);
          return if $dryrun;
        }
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
