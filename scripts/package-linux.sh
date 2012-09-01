#!/bin/bash

# Copyright (c) 2012 David Munger, Pierre L'Ecuyer, Université de Montréal.
# 
# This file is part of Lattice Builder.
# 
# Lattice Builder is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
# 
# Lattice Builder is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with Lattice Builder.  If not, see <http://www.gnu.org/licenses/>.


set -e

########################################################################
# settings
########################################################################

ROOT=$(readlink -f "$(dirname $0)/..")
test -z "$PACKAGE_DIR"	&& PACKAGE_DIR="$PWD"
test -z "$CORES"		&& CORES=$(grep -c ^processor /proc/cpuinfo)
test -z "$LINK"			&& LINK=static


########################################################################
# temporary directory
########################################################################

TMPDIR=$(mktemp -d)
trap "rm -rf $TMPDIR" EXIT


########################################################################
# guess version
########################################################################

cd "$ROOT"
version=$(git describe --tags --match 'v[0-9]*' | sed 's/^v//g')
echo "==> current version: $version"

prefix=$TMPDIR/latbuilder-$version


########################################################################
# compile tcode
########################################################################

echo "==> build tcode"
cd "$ROOT"
tcode=$(b2 -a /tools//tcode | sed 's/^common.copy\s\+//; t; d')


########################################################################
# compile Lattice Builder
########################################################################

echo "==> build latbuilder"
cd "$ROOT"
b2 -j $CORES tcode=$tcode link=$LINK /latbuilder//install --prefix=$prefix

# make sure everything worked
if [[ ! -f $prefix/bin/latbuilder ]]; then
	b2 tcode=$tcode link=$LINK /latbuilder//install --prefix=$prefix
fi


########################################################################
# tar package
########################################################################

echo "==> build package"

# guess system type
if [[ ! -f /lib/ld-linux-x86-64.so.2 ]]
then
	# fedora-like system
	suffix=-fedora
else
	suffix=
fi

cd "$prefix/.."
package="$PACKAGE_DIR/latbuilder-$version-bin-linux-x86_64$suffix.tar.bz2"
tar cvjf $package $(basename "$prefix")


########################################################################

echo "==> package built: $package"
