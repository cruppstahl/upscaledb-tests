#!/usr/bin/env perl

use strict;
use App::Rad;
use Term::ANSIColor;
use Data::Dumper;

sub higher_is_better
{
  my $metric = shift;
  my @tags = (
	"cache_hits",
	"freelist_hits",
	"extkey_cache_hits",
    "insert_#ops",
    "erase_#ops",
    "find_#ops",
    "insert_throughput",
    "find_throughput",
    "erase_throughput"
  );
  return grep(/$metric/, @tags);
}

sub lower_is_better
{
  return (!higher_is_better(shift));
}

sub run_single_test {
  my $file = shift;
  my $params = shift;

  open(FH, ">>perftest.txt") or die "Cannot open perftest.txt for writing";
  print "[START] ./test $params --quiet --metrics=all $file\n";
  my $output = `./test $params --quiet --metrics=all $file`;
  print "[STOP]  ./test $params --quiet --metrics=all $file\n";
  print FH "./test $params --quiet --metrics=all $file\n";
  print FH $output;
  print FH "\n";
  close(FH);
}

sub run {
  my $exttest="../../testfiles/2/ext_062.tst";
  my $blbtest="../../testfiles/4/blb-003.tst";
  my $duptest="../../testfiles/4/215.tst";
  my $deftest="../../testfiles/4/301.tst";

  unlink 'perftest.txt';

  run_single_test($exttest);
  run_single_test($blbtest);
  run_single_test($duptest);
  run_single_test($deftest);
  run_single_test($deftest, "--no-mmap");
  run_single_test($deftest, "--cache=unlimited");
  run_single_test($duptest, "--duplicate=last");
  run_single_test($deftest, "--use-cursors");
  run_single_test($deftest, "--use-transactions=5");
  run_single_test($deftest, "--use-transactions=20");
  run_single_test($deftest, "--use-transactions=100");
  run_single_test($deftest, "--use-fsync --use-transactions=5");
  run_single_test($deftest, "--use-recovery");
  run_single_test($deftest, "--inmemorydb");
  run_single_test($duptest, "--use-cursors --duplicate=last");
  run_single_test($duptest, "--use-cursors --duplicate=last --use-transactions=5");
  run_single_test($deftest, "--num-threads=5");
  run_single_test($deftest, "--num-threads=5 --use-transactions=5");
  run_single_test($duptest, "--use-remote --use-cursors --duplicate=last --use-transactions=5");
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
    die "expected 'Configuration: ...'" unless /^Configuration/;
    $_ = <FH>; # skip empty line
    $_ = <FH>;
    die "expected '[OK]...'" unless /^\[OK\]/;

    my %t = ();
    while (<FH>) {
      chomp;
      last if ($_ eq '');
      if (/total elapsed time \(sec\)\s*(\S+)$/) {
        $t{'total_elapsed_time'} = $1;
      }
      elsif (/latency \(min, avg, max\)/) {
        # skip
      }
      elsif (/^\s+hamsterdb (\S+)\s*(\S+)$/) {
        $t{$1} = $2;
      }
    }
    $h{$test} = \%t;
  }
  close(FH);
  return %h;
}

sub compare {
  my $c = shift;
  my $s = '';

  $c->options->{'new'} = 'perftest.txt'
    unless ($c->options->{'new'});

  die "arguments: <old> <new>" 
    unless ($c->options->{'old'});

  # read perftest.txt from the previous version
  my %old = read_data($c->options->{'old'});

  # read perftest from the new version
  my %new = read_data($c->options->{'new'});

  # foreach test...
  foreach my $test (sort keys %old) {
    if (!$new{$test}) {
      $s .= $test . ' is ' . color('red') . 'missing' . color('reset') . "\n";
      next;
    }

    my $t = '';
    foreach my $k (sort keys $old{$test}) {
      my $oldv = $old{$test}->{$k};
      # calculate and print the diff
      my $improved = 0;
      my $dontcare = 0;
      my $newv = $new{$test}->{$k};
      my $color = 'reset';

      if (lower_is_better($k)) {
        $improved = 1
          if ($newv < $oldv);
      }
      elsif (higher_is_better($k)) {
        $improved = 1
          if ($newv > $oldv);
      }
      else {
        $dontcare = 1;
      }

      if (!$dontcare) {
        if ($improved) {
          $color = 'green';
        }
        else {
          $color = 'red';
        }
      }

      my $p = 0;
      my $sign = '+';
      if ($newv > $oldv) {
        my $diff = $newv - $oldv;
        $p = $diff / ($oldv / 100.0)
          if ($oldv != 0);
        $newv = '???' unless $newv;
        $sign = '-';
      }
      elsif ($oldv > $newv) {
        my $diff = $oldv - $newv;
        $p = $diff / ($oldv / 100.0);
        $newv = '???' unless $newv;
        $sign = '+';
      }
      else {
        next;
      }
      $t .= sprintf('%25s', $k) . "   "
          . sprintf('%12s', $oldv) . " -> "
          . sprintf('%12s', $newv) . " == "
          . color($color) . sprintf('%s%6.2f %', $sign, $p)
          . color('reset') . "\n";
    }
    $s .= "$test\n$t" if $t ne '';
  }

  return $s;
}

sub setup {
  my $c = shift;
  $c->register_commands( {
        run => 'runs the tests to gather performance data',
        compare => 'compares performance data; arguments: --old=OLD --new=NEW'
    });
}

App::Rad->run();
