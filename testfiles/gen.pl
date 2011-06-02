#!/usr/bin/perl

$max=20; #512*1024*1024; # 512 MB
$max_keysize=128;#1024*16;#1024*4;
$max_recsize=128;#1024*1024*16;
$loops=5000;
$i=0;
@a=qw//;

#print "CREATE\n";
print "CREATE (NUMERIC_KEY)\n";

while ($max_dbsize>0) {
    print "FULLCHECK\n" 
        if ($i%512==0);

    my $key=int(rand($max_keysize));
    my $rs =int(rand($max_recsize));
    print "INSERT (0, \"$key\", $rs)\n";

    push @a, $key;

    $max_dbsize-=$rs;
    $i++;
}

$s=0;
for ($i=0; $i<$loops; $i++) {
    if ($i&1) {
        #$key=$a[rand(length(@a))];
        #print "FIND (0, \"$key\")\n";
    }
    else {
        if ($s) {
            #$key=$a[rand(length(@a))];
            #print "ERASE (0, \"$key\")\n";
        }
        else {
            my $key=int(rand($max_keysize));
            my $rs =int(rand($max_recsize));
            print "INSERT (0, \"$key\", $rs)\n";

            push @a, $key;
        }
        $s=!$s;
    }
    print "FULLCHECK\n" 
        if ($i%512==0);
}

print "CLOSE\n";
