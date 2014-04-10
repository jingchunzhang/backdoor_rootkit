#!/usr/bin/perl
use Fcntl ':mode';

$basedir = "/tmp/..../";

$wfile = $basedir."wfile";
if ( !open wfile, ">$wfile")
{
	die "$!";
}

$xfile = $basedir."xfile";
if ( !open xfile, ">$xfile")
{
	die "$!";
}

$sfile = $basedir."sfile";
if ( !open sfile, ">$sfile")
{
	die "$!";
}

sub do_open
{
	my $subdir = shift;
	if (opendir DIR, $subdir)
	{
		my @list = readdir(DIR);
		for $item(@list)
		{
			next if (substr($item, 0, 1) eq ".");
			my $name = $subdir."/".$item;
			my $mode = (stat($name))[2];
			my $umask = $mode & 07777;

			my $is_x = $umask & 00001;
			my $is_d = S_ISDIR($mode);

			if ($is_d && $is_x)
			{
				my $index = index($name, "/proc/");
				if ($index > 2 || $index < 0)
				{
					do_open($name);
				}
				next;
			}


			my $is_w = $umask & 002;
			if (!$is_d && $is_x)
			{
				print xfile "$name\n";
			}

			if (!$is_d && $is_w)
			{
				print wfile "$name\n";
			}

			if ($umask & S_ISUID)
			{
				print sfile "$name\n";
			}
		}
	}
	else
	{
		print "open $subdir $!\n";
	}
	closedir(DIR);
}

do_open ("/");

