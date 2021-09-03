import re
import math
import numpy as np

from .generate_points import generate_points_digital_net, generate_points_ordinary_lattice, generate_points_polynomial_lattice, generate_points_sobol_net

class Result:
    def __init__(self, set_type, nb_points, dim, merit, gen_vector=[], modulus= [], nb_cols=0, nb_rows=0, matrices = [], interlacing=1, max_level=0, matrices_cols=None):
        self.set_type = set_type
        self.nb_points = nb_points
        self.dim = dim
        self.merit = merit
        self.gen_vector = gen_vector
        self.modulus = modulus
        self.nb_cols = nb_cols
        self.nb_rows = nb_rows
        self.matrices = matrices
        self.interlacing = interlacing
        self.matrices_cols = matrices_cols

        if self.nb_cols == 0:   # ordinary set type
            self.max_level = max_level
        else:
            self.max_level = nb_cols

    def __str__(self):
        s1 = "Result:\nNumber of points: %s" % (str(self.nb_points))
        if self.modulus != []:
            s2 = "\nModulus: %s" % (str(self.modulus))
        else:
            s2 = ""
        if self.gen_vector != []:
            if self.set_type == 'Sobol':
                s3 = "\nDirection numbers: %s" % (str(self.gen_vector))
            else:
                s3 = "\nGenerating Vector: %s" % (str(self.gen_vector))
        else:
            s3 = ""
        s4 = "\nMerit value: %s" % (str(self.merit))
        return s1 + s2 + s3 + s4
    
    def _repr_html_(self):
        s1 = "<span> <b> Number of points</b>: %s </span>" % (str(self.nb_points))
        if self.modulus != []:
            s2 = "<p> <b> Modulus</b>: %s </p>" % (str(self.modulus))
        else:
            s2 = ""
        if self.gen_vector != []:
            if self.set_type == 'Sobol':
                s3 = "<p> <b> Direction numbers</b>: %s </p>" % (str(self.gen_vector))
            else:
                s3 = "<p> <b> Generating Vector</b>: %s </p>" % (str(self.gen_vector))
        else:
            s3 = ""            
        s4 = "<p> <b> Merit value</b>: %s </p>" % (str(self.merit))
        return s1 + s2 + s3 + s4


    def getPoints(self, coord, level=None):
        assert coord < self.dim and (level==None or self.max_level > 0)

        if 'Ordinary' in self.set_type:
            if level == None:
                return generate_points_ordinary_lattice(self.gen_vector, self.nb_points, coord)
            else:
                return generate_points_ordinary_lattice(self.gen_vector, 2 ** level, coord)
        elif self.set_type == 'Polynomial':
            return generate_points_polynomial_lattice(self.modulus, self.gen_vector, self.interlacing, coord, level)
        elif self.set_type == 'Sobol':
            return generate_points_sobol_net(self.dim, int(np.log2(self.nb_points)), self.gen_vector, self.interlacing, coord, level)
        else:
            return generate_points_digital_net(self.matrices, self.interlacing, coord, level)


def generatingMatricesFromColumns(columns):
    m = len(columns)
    matrix = np.zeros((m, m), dtype=np.int32)
    for j in range(m):
        for i in range(m):
            matrix[i][j] = (columns[j] >> (30-i)) & 1
    return matrix


def parse_output(file_output):

    Lines = file_output.split("\n")
    merit = float(Lines[1].split(': ')[1])
    sep = "   #"

    if 'ordinary' in Lines[0]:
        dim = int(Lines[3].split(sep)[0].strip())
        nb_points = int(Lines[4].split(sep)[0].strip())
        max_level = int(np.log2(nb_points))
        gen_vector = []
        for i in range(dim):
            gen_vector.append(int(Lines[6+i].strip()))
        return Result('Ordinary', nb_points, dim, merit, gen_vector=gen_vector, max_level=max_level)

    elif 'polynomial' in Lines[0]:
        dim = int(Lines[3].split(sep)[0].strip())
        if 'Interlacing' in Lines[4]:
            interlacing, next_line = int(Lines[4].split(sep)[0].strip()), 7
        else:
            interlacing, next_line = 1, 5
        modulus = int(Lines[next_line].split(sep)[0].strip())
        gen_vector = []
        for i in range(dim * interlacing):
            gen_vector.append(int(Lines[next_line+2+i].strip()))
        return Result('Polynomial', 2 ** int(np.log2(modulus)), dim, merit, gen_vector=gen_vector, modulus=modulus, interlacing=interlacing)

    elif 'sobol' in Lines[0]:
        dim = int(Lines[3].split(sep)[0].strip())
        if 'Interlacing' in Lines[4]:
            interlacing, next_line = int(Lines[4].split(sep)[0].strip()), 6
        else:
            interlacing, next_line = 1, 4
        max_level = int(Lines[next_line].split(sep)[0].strip())
        nb_points = 2 ** max_level
        gen_vector = [[0]]
        for i in range(dim * interlacing - 1):
            gen_vector.append([int(x) for x in Lines[next_line+2+i].split(' ')])
        return Result('Sobol', nb_points, dim, merit, gen_vector=gen_vector, interlacing=interlacing, max_level=max_level)

    elif 'explicit' in Lines[0]:
        dim = int(Lines[3].split(sep)[0].strip())
        if 'Interlacing' in Lines[4]:
            interlacing, next_line = int(Lines[4].split(sep)[0].strip()), 6
        else:
            interlacing, next_line = 1, 4
        nb_cols = int(Lines[next_line].split(sep)[0].strip())
        nb_rows = nb_cols
        nb_points = 2 ** nb_cols
        if "--multilevel true" in Lines[0]:
            max_level = nb_cols
        else:
            max_level = 0
        matrices = []
        matrices_cols = []
        for c in range(dim * interlacing):
            matrices.append(generatingMatricesFromColumns([int(x) for x in Lines[next_line+3+c].split(' ')]))
            matrices_cols.append([int(x) for x in Lines[next_line+3+c].split(' ')])
        return Result('Explicit', nb_points, dim, merit, nb_cols=nb_cols, nb_rows=nb_rows, matrices=np.array(matrices), interlacing=interlacing, matrices_cols=np.array(matrices_cols), max_level=max_level)
