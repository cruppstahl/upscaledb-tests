#!/usr/bin/perl

$max_keysize=0xffffff;#1024*16;#1024*4;
$max_recsize=1024;#1024*1024*16;
$loops=1000000;
$i=0;
@a=qw//;

#print "CREATE\n";
print "CREATE (NUMERIC_KEY)\n";

while ($max_dbsize>0) {
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
    if (1) {
        if ($i > 0 && $i % 5 == 0) {
            $key=$a[rand(length(@a))];
            print "ERASE (0, \"$key\")\n";
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
        if ($i>0 && $i%10000==0);
}

print "CLOSE\n";
