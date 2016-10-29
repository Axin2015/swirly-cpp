#!/usr/bin/perl -w

# The Restful Matching-Engine.
# Copyright (C) 2013, 2016 Swirly Cloud Limited.
#
# This program is free software; you can redistribute it and/or modify it under the terms of the
# GNU General Public License as published by the Free Software Foundation; either version 2 of the
# License, or (at your option) any later version.
#
# This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without
# even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
# General Public License for more details.
#
# You should have received a copy of the GNU General Public License along with this program; if
# not, write to the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
# 02110-1301, USA.

use strict;

# Usage: dotdepend.pl <lib>

my $cxx = $ENV{CXX} || 'g++';
my $lib = shift;
$lib or die "Library name not specified";

my $makedepend = "$cxx -std=c++14 -MM -MG -Iinclude include/swirly/$lib/*.hpp src/lib/$lib/*.[ch]pp";
my $tred = 'tred';

sub ltrim {
  $_[0] =~ s/^\s+//;
}

sub rtrim {
  $_[0] =~ s/\s+$//;
}

sub trim {
  ltrim($_[0]);
  rtrim($_[0]);
}

# Remove file extension.
sub rmext {
  $_[0] =~ s/\.\w+$//;
}

# Extract component and library from path.
sub parsedep {
  my ($path) = @_;
  # Remove leading slash.
  $path =~ s|^/||;
  my @parts = split('/', $path);
  my $comp = pop(@parts);
  rmext($comp) if defined $comp;
  my $lib = pop(@parts);
  return ($comp, $lib);
}

my ($comp, %comps);
my $cont = 0;

foreach (`$makedepend`) {

  chomp;

  my $deps;
  if ($cont) {

    # The dependency list is being continued from the previous line.
    $deps = $_;

  } else {

    # The dependency list is separated from the object file by the colon character.
    ($comp, $deps) = split(':');

    # The output of makedepend may lead with a blank line.
    next unless $comp;

    trim($comp);
    rmext($comp);

    $comps{$comp} = []
      unless defined $comps{$comp};
  }

  trim($deps);
  $cont = 0;

  foreach (split(/\s+/, $deps)) {

    trim($_);

    # A backslash encountered at the end of a line signifies a line continuation.
    if ($_ eq '\\') {
      $cont = 1;
    } else {

      my ($dcomp, $dlib) = parsedep($_);

      # If component within our library.
      if ($dlib eq $lib) {

        # Component dependency.
        #
        # Do not add dependency circular dependency to self. This may occur as a result of
        # including the header file associated with a component's source file.
        push(@{$comps{$comp}}, $dcomp)
          unless $comp eq $dcomp;
      } else {

        # Library dependency.
        push(@{$comps{$comp}}, $dlib);
      }
    }
  }
}

# The dot-file is piped through tred in order to remove transitive dependencies.
open(PIPE, "|$tred")
  or die "Failed to open tred pipe: $!";

# Libraries are depicted as boxes.
print PIPE <<EOD;
digraph G {
  ratio="auto";
  size="11.7,8.3!";
  node [shape=box];
  rankdir=BT
EOD

# Components are depicted as ovals.
print PIPE "  $_ [shape=oval];\n"
  foreach keys %comps;

while (my ($comp, $deps) = each %comps) {
  print PIPE "  $comp -> $_;\n"
    foreach @{$deps};
}

print PIPE "}\n";
close(PIPE);
