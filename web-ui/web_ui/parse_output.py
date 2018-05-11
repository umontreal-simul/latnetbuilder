import re
import math
import numpy as np

class LatNet:
    def __init__(self, size, gen, self_type):
        self.size = size
        self.gen = gen
        self.self_type = self_type
    def getDim(self):
        if self.self_type == 'lattice':
            return len(self.gen.gen_vector)
    def __str__(self):
        if self.self_type == 'lattice':
            return 'lattice({}, {})'.format(self.size, self.gen)
        else:
            pass
    def __repr__(self):
        return str(self)

class GenParam:
    def __init__(self, s, search_type):
        if search_type == 'ordinary':
            self.gen_vector = [int(x) for x in s.split(',')]
        elif search_type == 'polynomial':
            self.gen_vector = [[int(y) for y in x.strip('[ ,]').split(' ')] for x in s.split(',')]
    def __str__(self):
        return str(self.gen_vector)
    def __repr__(self):
        return str(self)

class SizeParam:
    def __init__(self, s, search_type):
        if search_type == 'ordinary':
            # try:
            p = s.split('^')
            if len(p) == 2:
                self._set_embedded(int(p[0]), int(p[1]))
            else:
                self._set_simple(int(s))
            # except:
            #     if type(s) == tuple and len(s) == 2:
            #         self._set_embedded(int(s[0]), int(s[1]))
            #     else:
            #         self._set_simple(int(s))

        elif search_type == 'polynomial':
            p = s.split('^')
            if len(p) == 2:
                self.base = [int(y) for y in p[0].strip('[]').split(' ')]
                self.power = int(p[1])
                self.width = (len(self.base)-1)*self.power
            else:
                self.base = [int(y) for y in s.strip('[]').split(' ')]
                self.power = 1
                self.width = len(self.base)-1
            self.points = 2**self.width

    def _set_embedded(self, base, power):
        self.base = base
        self.power = power
        self.nb_points = self.base**self.power

    def _set_simple(self, points):
        self.nb_points = points
        self.base = self.nb_points
        self.power = 1

    def __str__(self):
        if self.power == 1:
            return str(self.nb_points)
        else:
            return '{}^{}'.format(self.base, self.power)

    def __repr__(self):
        return str(self)


class Result:
    def __init__(self, latnet=None, seconds=None, search_type=None, matrices=None, merit=None):
        self.latnet = latnet
        self.seconds = seconds
        self.search_type = search_type
        self.matrices = matrices
        self.merit = merit
    def __str__(self):
        return '{} : {} ({} s)'.format(self.latnet, self.merit, self.seconds)
    def __repr__(self):
        return str(self)

    def getDim(self):
        if self.latnet is None:
            return 0
        else:
            return self.latnet.getDim()

    def getMaxLevel(self):
        return int(math.log2(self.latnet.size.nb_points))

    def matrix(self, coord):
        return self.matrices[coord]

    def getNet(self, coord, level=None):
        if self.latnet is None:
            return np.array([])
        if level is None:
            nb_points = self.latnet.size.nb_points
        else:
            nb_points = 2**level
        assert coord < self.getDim()

        if self.search_type == 'ordinary':
            return np.array([self.latnet.gen.gen_vector[coord]*i/nb_points % 1 for i in range(nb_points)])

        elif self.search_type == 'polynomial':
            points = []
            matrice = self.matrix(coord)
            width = matrice.shape[0]
            for x in range(nb_points):
                binary_repr = np.array([((x>>i)&1) for i in range(width)])
                prod = np.mod(matrice.dot(binary_repr), 2)
                points.append(float(sum([prod[width-1-i] << i for i in range(width)]))/nb_points)
            return points



def parse_output(s, gui, search_type):

    if search_type == 'ordinary':
        pat_latdef = re.compile(r'^BEST LATTICE:\s*lattice\((?P<size>[^,]*),\s*\[(?P<gen>[^\]]*)\]\s*\)\s*:\s*(?P<merit>.*)')
    elif search_type == 'polynomial':
        pat_latdef = re.compile(r'^BEST LATTICE:\s*PolynomialLattice\((?P<size>[^,]*),\s*\[(?P<gen>[^\)]*)\]\s*\)\s*:\s*(?P<merit>.*)')
    
    pat_time = re.compile(r'^ELAPSED CPU TIME:\s*(?P<seconds>\S*)\s*seconds') 
    # seconds = None  

    if search_type in ['ordinary', 'polynomial']:
        for line in s.split('\n'):
            match = pat_latdef.match(line)
            if match:
                size = SizeParam(match.group('size'), search_type)
                gen = GenParam(match.group('gen'), search_type)
                merit = float(match.group('merit'))
                latnet = LatNet(size, gen, self_type='lattice')
                continue

            match = pat_time.match(line)
            if match:
                seconds = float(match.group('seconds'))
                continue
    else:   # to be modified
        pass

    matrices = []
    if search_type == 'polynomial' or 'digital' in search_type:
        afterOldMatrix = False
        for line in s.split('\n'):
            if search_type == 'polynomial' or search_type == 'digital':
                if '//dim' in line:
                    width = size.width
                    M = []
                    afterOldMatrix = True
                    continue
                try:
                    L = int(line)
                    M.append(np.array([((L>>i)&1) for i in range(width)]))
                    continue
                except:
                    if line == '' and afterOldMatrix:
                        matrices.append(np.array(M))
                        afterOldMatrix = False

    gui.output.result_obj.latnet = latnet
    gui.output.result_obj.merit = merit
    gui.output.result_obj.seconds = seconds
    gui.output.result_obj.search_type = search_type
    gui.output.result_obj.matrices = matrices
