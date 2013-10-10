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
  my $params = shift;

  open(FH, ">>perftest.txt") or die "Cannot open perftest.txt for writing";
  print "[START] ./ham_bench $params --quiet --metrics=all\n";
  my $output = `./ham_bench $params --quiet --metrics=all`;
  print "[STOP]  ./ham_bench $params --quiet --metrics=all\n";
  print FH "./ham_bench $params --quiet --metrics=all\n";
  print FH $output;
  print FH "\n";
  close(FH);
}

sub run {
  unlink 'perftest.txt';

  my $opt = "--seed=1380279291 --stop-ops=5000000";

  # a few quickies
  run_single_test("--seed=12345 --stop-ops=50000");
  run_single_test("--seed=12345 --stop-ops=50000 --key=uint16");
  run_single_test("--seed=12345 --stop-ops=50000 --key=uint32");
  run_single_test("--seed=12345 --stop-ops=50000 --key=uint64");

  # various work loads (uint16)
  run_single_test("$opt --key=uint16 --erase-pct=25 --find-pct=40");
  run_single_test("$opt --key=uint16");
  run_single_test("$opt --key=uint16 --open --find-pct=100");

  # various work loads (uint32)
  run_single_test("$opt --key=uint32 --erase-pct=25 --find-pct=40");
  run_single_test("$opt --key=uint32");
  run_single_test("$opt --key=uint32 --open --find-pct=100");

  # various work loads (uint64)
  run_single_test("$opt --key=uint64 --erase-pct=25 --find-pct=40");
  run_single_test("$opt --key=uint64");
  run_single_test("$opt --key=uint64 --open --find-pct=100");

  # various work loads (real32)
  run_single_test("$opt --key=real32 --erase-pct=25 --find-pct=40");
  run_single_test("$opt --key=real32");
  run_single_test("$opt --key=real32 --open --find-pct=100");

  # various work loads (real64)
  run_single_test("$opt --key=real64 --erase-pct=25 --find-pct=40");
  run_single_test("$opt --key=real64");
  run_single_test("$opt --key=real64 --open --find-pct=100");

  # various work loads (random distribution)
  run_single_test("$opt --erase-pct=25 --find-pct=40");
  run_single_test("$opt");
  run_single_test("$opt --open --find-pct=100");

  # various work loads (ascending distribution)
  run_single_test("$opt --distribution=ascending --erase-pct=25 --find-pct=40");
  run_single_test("$opt --distribution=ascending");
  run_single_test("$opt --distribution=ascending --open --find-pct=100");

  # extended keys
  run_single_test("$opt --btree-keysize=20 --keysize=1024");
  # record size 8
  run_single_test("$opt --recsize=8");
  # record size 256
  run_single_test("$opt --recsize=256");
  # disabled mmap
  run_single_test("$opt --no-mmap");
  # small cache
  run_single_test("$opt --cache=10");
  # unlimited cache
  run_single_test("$opt --cache=unlimited");
  # inmemory
  run_single_test("$opt --inmemorydb");
  # duplicates
  run_single_test("$opt --duplicate=last");
  # overwrites
  run_single_test("$opt --overwrite");
  # cursors
  run_single_test("$opt --use-cursors");
  # cursors w/ duplicate=first
  run_single_test("$opt --use-cursors --duplicate=first");
  # cursors w/ duplicate=last
  run_single_test("$opt --use-cursors --duplicate=last");
  # cursors w/ overwrites
  run_single_test("$opt --use-cursors --overwrite");
  # recovery
  run_single_test("$opt --use-recovery");
  # txn temp
  run_single_test("$opt --use-transactions=tmp");
  # txn 5
  run_single_test("$opt --use-transactions=5");
  # txn 20
  run_single_test("$opt --use-transactions=20");
  # txn 100
  run_single_test("$opt --use-transactions=100");
  # txn temp (inmemory)
  run_single_test("$opt --use-transactions=tmp --inmemorydb");
  # txn 5 (inmemory)
  run_single_test("$opt --use-transactions=5 --inmemorydb");
  # txn 20 (inmemory)
  run_single_test("$opt --use-transactions=20 --inmemorydb");
  # txn 100 (inmemory)
  run_single_test("$opt --use-transactions=100 --inmemorydb");
  # txn 5 w/ fsync
  run_single_test("$opt --use-transactions=5 --use-fsync");
  # threads 5
  run_single_test("$opt --use-threads=5");
  # encryption
  run_single_test("$opt --use-encryption");
  # remote
  run_single_test("$opt --use-remote");

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
