import re
import numpy as np

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
    def __init__(self, s, polynomial):
        if not polynomial:
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
        else:
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
    def __init__(self, lattice=None, seconds=None, polynomial=False, matrices=None):
        self.lattice = lattice
        self.seconds = seconds
        self.polynomial = polynomial
        self.matrices = matrices
    def __str__(self):
        return '{} ({} s)'.format(self.lattice, self.seconds)
    def __repr__(self):
        return str(self)

    def getDim(self):
        if self.lattice is None:
            return 0
        else:
            return len(self.lattice.gen)

    def getMaxLevel(self):
        return int(np.log2(self.lattice.size.points))

    def matrix(self, coord):
        # n_cols = len(self.lattice.size.base)
        # matrice = np.zeros((n_cols, n_cols))
        # for i in range(n_cols):
        #     for j in range(n_cols):
        #         matrice[i][j] = 0
        # return matrice
        return self.matrices[coord]

    def getNet(self, coord, level=None):
        if self.lattice is None:
            return np.array([])
        if level is None:
            nb_points = self.lattice.size.points
        else:
            nb_points = 2**level
        assert coord < self.getDim()

        if not self.polynomial:
            return np.array([self.lattice.gen[coord]*i/nb_points % 1 for i in range(nb_points)])

        else:
            points = []
            # gen_poly = self.lattice.gen[coord]
            matrice = self.matrix(coord)
            # nb_points = self.lattice.size.points
            width = matrice.shape[0]
            for x in range(nb_points):
                binary_repr = np.array([((x>>i)&1) for i in range(width)])
                prod = np.mod(matrice.dot(binary_repr), 2)
                points.append(float(sum([prod[width-1-i] << i for i in range(width)]))/nb_points)
            # print(len(points))
            return points



def parse_output(s, polynomial):

    if polynomial:
        pat_latdef = re.compile(r'^BEST LATTICE:\s*PolynomialLattice\((?P<size>[^,]*),\s*\[(?P<gen>[^\)]*)\]\s*\)\s*:\s*(?P<merit>.*)')
    else:
        pat_latdef = re.compile(r'^BEST LATTICE:\s*lattice\((?P<size>[^,]*),\s*\[(?P<gen>[^\]]*)\]\s*\)\s*:\s*(?P<merit>.*)')
    
    pat_time = re.compile(r'^ELAPSED CPU TIME:\s*(?P<seconds>\S*)\s*seconds')    

    seconds = None

    ret = []
    matrices = []
    if polynomial:
        new = False

    for line in s.split('\n'):
        # print(line)
        m = pat_latdef.match(line)
        if m:
            size = SizeParam(m.group('size'), polynomial)
            if not polynomial:
                gen = [int(x) for x in m.group('gen').split(',')]
            else:
                gen = [[int(y) for y in x.strip('[ ,]').split(' ')] for x in m.group('gen').split(',')]
            merit = float(m.group('merit'))
            latmerit = LatMerit(size, gen, merit)
            continue
        m = pat_time.match(line)
        if m:
            seconds = float(m.group('seconds'))
            continue

        if polynomial:
            if '//dim' in line:
                width = size.width
                M = []
                new = True
                continue
            try:
                L = int(line)
                M.append(np.array([((L>>i)&1) for i in range(width)]))
                continue
            except:
                if line == '' and new:
                    matrices.append(np.array(M))
                    new = False

    # print(matrices)
    ret.append(Result(latmerit, seconds, polynomial, matrices))

    return ret