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
# MinGW64 environment
########################################################################

if [[ -z "$MINGW64_HOME" ]]
then
	echo "error: the MINGW64_HOME environement variable is not set" >&2
	echo "example: export MINGW64_HOME=$HOME/mingw64" >&2
	exit 1
fi

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

export PATH=$MINGW64_HOME/bin:$PATH
export LD_LIBRARY_PATH=$MINGW64_HOME/lib64:$MINGW64_HOME/lib:$LD_LIBRARY_PATH


echo "==> build latbuilder"
cd "$ROOT"
b2 -j $CORES target-os=windows tcode=$tcode link=$LINK /latbuilder//install --prefix=$prefix

# make sure everything worked
if [[ ! -f $prefix/bin/latbuilder.exe ]]; then
	b2 target-os=windows tcode=$tcode link=$LINK /latbuilder//install --prefix=$prefix
fi


########################################################################
# dependencies
########################################################################

echo "==> install dependencies"

# recursively find required DLL's
update_deps=1
while [[ $update_deps -ne 0 ]]
do
	update_deps=0
	dlls=$(objdump -p $(find "$prefix" -name '*.exe' -or -name '*.dll') |
		sed 's/^\s*DLL Name:\s*\(.*\)/\1/; t; d' |
		grep -v '^KERNEL.*\.dll\|USER.*\.dll\|^msvcrt.*\.dll' |
		sort | uniq)

	for dll in $dlls
	do
		if [[ ! -f $prefix/bin/$dll && ! -f $prefix/lib/$dll ]]
		then
			source=$(find "$MINGW64_HOME" -name "$dll" | grep -v /debug/ || echo)
			if [[ -f "$source" ]]
			then
				cp "$source" "$prefix/bin"
				echo "intalling $dll"
				update_deps=1
			else
				echo "skipping $dll"
			fi
		fi
	done
done


########################################################################
# zip package
########################################################################

echo "==> build package"
cd "$prefix/.."
package="$PACKAGE_DIR/latbuilder-$version-bin-mingw64.zip"
zip -r $package $(basename "$prefix")


########################################################################

echo "==> package built: $package"
