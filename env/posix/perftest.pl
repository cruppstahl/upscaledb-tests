#!/usr/bin/env perl

use strict;
use App::Rad;
use Term::ANSIColor;

sub run_single_test {
  my $file = shift;
  my $params = shift;

  open(FH, ">>perftest.txt") or die "Cannot open perftest.txt for writing";
  my $output=`./test $params --no-berkeleydb $file`;
  print "./test $params --no-berkeleydb $file\n";
  print FH "./test $params --no-berkeleydb $file\n";
  print FH $output;
  print FH "\n";
  close(FH);
}

sub run_tests {
  my $exttest="../../testfiles/2/ext_062.tst";
  my $blbtest="../../testfiles/4/blb-003.tst";
  my $deftest="../../testfiles/4/215.tst";
  #my $exttest="../../testfiles/1/01.tst";
  #my $blbtest="../../testfiles/1/01.tst";
  #my $deftest="../../testfiles/1/01.tst";

  unlink 'perftest.txt';

  run_single_test($exttest);
  run_single_test($blbtest);
  run_single_test($deftest);
  run_single_test($deftest, "--disable-mmap");
  run_single_test($deftest, "--disable-mmap --cacheunlimited");
  run_single_test($deftest, "--duplicate");
  run_single_test($deftest, "--use-cursors");
  run_single_test($deftest, "--use-transactions=5");
  run_single_test($deftest, "--use-transactions=20");
  run_single_test($deftest, "--use-transactions=100");
  run_single_test($deftest, "--writethrough --use-transactions=5");
  run_single_test($deftest, "--writethrough --use-transactions=20");
  run_single_test($deftest, "--writethrough --use-transactions=100");
  run_single_test($deftest, "--use-recovery");
  run_single_test($deftest, "--inmemorydb");
  run_single_test($deftest, "--use-cursors --duplicate");
  run_single_test($deftest, "--use-cursors --duplicate --use-transactions=5");
  run_single_test($deftest, "--num-threads=5");
  run_single_test($deftest, "--num-threads=20");
  run_single_test($deftest, "--num-threads=5 --use-transactions=5");
  run_single_test($deftest, "--num-threads=20 --use-transactions=5");
  return '';
}

sub read_data {
  my $path = shift;
  my %h = ();
  open(FH, "< $path") or die "Cannot open $path for reading";
  while (<FH>) {
    chomp;
    die "expected './test ...'" unless /^\.\/test /;
    my $test = $_;

    $_ = <FH>;
    chomp;
    die "expected '[OK]...'" unless /^\[OK\]/;

    $_ = <FH>;
    chomp;
    die "expected 'hamsterdb'" unless /^hamsterdb/;

    my %t = ();
    while (<FH>) {
      chomp;
      last if ($_ eq '');
      /^\s*(\S+)\s*(\S+)$/;
      $t{$1} = $2;
    }
    $h{$test} = \%t;
  }
  close(FH);
  return %h;
}

sub compare {
  my $c = shift;
  my $s = '';
  my $threshold = 3;

  die "arguments: <old> <new>" 
    unless ($c->options->{'new'} and $c->options->{'old'});

  # read perftest.txt from the previous version
  my %old = read_data($c->options->{'old'});

  # read perftest from the new version
  my %new = read_data($c->options->{'new'});

  # foreach test...
  foreach my $test (sort keys %old) {
    my $t = '';
    while ((my $k, my $oldv) = each($old{$test})) {
      # calculate and print the diff
      my $newv = $new{$test}->{$k};
      if ($newv > $oldv) {
        if ($oldv != 0) {
          my $p = ($newv - $oldv) / $oldv;
          $t .= "\t$k\t" . color("green") . sprintf("+%.2f %", $p)
                  . color("reset") . "\n" if ($p > $threshold);
        }
        else {
          $t .= "\t$k\t" . color("red") . "??? -> $newv"
                  . color("reset") . "\n";
        }
      }
      elsif ($oldv > $newv) {
        if ($newv != 0) {
          my $p = ($oldv - $newv) / $newv;
          $t .= "\t$k\t" . color("red") . sprintf("-%.2f %", $p)
                  . color("reset") . "\n" if ($p > $threshold);
        }
        else {
          $t .= "\t$k\t" . $k . "\t" . color("red") . "$oldv -> ???"
                  . color("reset") . "\n";
        }
      }
    }
    $s .= "$test\n$t" if $t ne '';
  }

  return $s;
}

sub setup {
  my $c = shift;
  $c->register_commands( {
        run_tests => 'runs the tests to gather performance data',
        compare => 'compares performance data; arguments: --old=OLD --new=NEW'
    });
}

App::Rad->run();
