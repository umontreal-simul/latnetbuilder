# Copyright (c) 2013 David Munger, Pierre L'Ecuyer, Universite de Montreal.
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

from __future__ import division
from __future__ import print_function
from __future__ import generators

import os, subprocess, re

LATBUILDER = 'latbuilder'
#LATBUILDER = os.path.abspath(os.path.join(
#   os.path.dirname(__file__),
#   'bin',
#   'latbuilder',
#   ))

class LatMerit:
    def __init__(self, size, gen, merit):
        self.size = size
        self.gen = gen
        self.merit = merit
    def __str__(self):
        return 'lattice({}, {}): {}'.format(self.size, self.gen, self.merit)
    def __repr__(self):
        return str(self)

class SizeParam:
    def __init__(self, s):
        try:
            p = s.split('^')
            if len(p) == 2:
                self._set_embedded(int(p[0]), int(p[1]))
            else:
                self._set_ordinary(int(s))
        except:
            if type(s) == tuple and len(s) == 2:
                self._set_embedded(int(s[0]), int(s[1]))
            else:
                self._set_ordinary(int(s))

    def _set_embedded(self, base, power):
        self.base = base
        self.power = power
        self.points = self.base**self.power

    def _set_ordinary(self, points):
        self.points = points
        self.base = self.points
        self.power = 1

    def __str__(self):
        if self.power == 1:
            return str(self.points)
        else:
            return '{}^{}'.format(self.base, self.power)

    def __repr__(self):
        return str(self)


class Result:
    def __init__(self, lattice, seconds):
        self.lattice = lattice
        self.seconds = seconds
    def __str__(self):
        return '{} ({} s)'.format(self.lattice, self.seconds)
    def __repr__(self):
        return str(self)

def execute(
        lattype,
        size, dimension,
        normtype,
        merit,
        weights,
        construction,
        filters=None,
        mlfilters=None,
        combiner=None,
        repeat=None):
    command = [LATBUILDER,
            '--lattice-type', lattype,
            '--size', str(size),
            '--dimension', str(dimension),
            '--norm-type', str(normtype),
            '--figure-of-merit', merit,
            '--construction', construction,
            ]

    command.append('--weights')
    for w in weights.split(' '):
        command.append(w)

    if filters:
        command += ['--filters'] + filters

    if mlfilters:
        command += ['--multilevel-filters'] + mlfilters

    if combiner:
        command += ['--combiner', combiner]

    if repeat:
        command += ['--repeat', str(repeat)]
    
    output = subprocess.check_output(command, stderr=subprocess.STDOUT)

    ret = parse_output(output)
    return repeat is None and ret[0] or ret


def parse_output(s):

    pat_latdef = re.compile(r'^BEST LATTICE:\s*lattice\((?P<size>[^,]*),\s*\[(?P<gen>[^\]]*)\]\s*\)\s*:\s*(?P<merit>.*)')
    pat_time = re.compile(r'^ELAPSED CPU TIME:\s*(?P<seconds>\S*)\s*seconds')

    seconds = None

    ret = []

    for line in s.decode().split('\n'):
        line = line.decode()
        m = pat_latdef.match(line)
        if m:
            size = SizeParam(m.group('size'))
            gen = [int(x) for x in m.group('gen').split(',')]
            merit = float(m.group('merit'))
            latmerit = LatMerit(size, gen, merit)
            continue
        m = pat_time.match(line)
        if m:
            seconds = float(m.group('seconds'))
            ret.append(Result(latmerit, seconds))
    
    return ret


if __name__ == '__main__':
    print('testing latbuilder module')
    result = execute(
            lattype='ordinary',
            size=100,
            normtype='2',
            merit='sum:P2',
            weights='product:0.1',
            dimension=3,
            construction='CBC')
    print('size:    {}'.format(result.lattice.size))
    print('gen:     {}'.format(result.lattice.gen))
    print('merit:   {}'.format(result.lattice.merit))
    print('seconds: {}'.format(result.seconds))
