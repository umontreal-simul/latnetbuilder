import re
import math
import numpy as np

class Result:
    def __init__(self, set_type, nb_points, dim, merit, time, gen_vector=[], modulus= [], nb_cols=0, nb_rows=0, matrices = [], interlacing=1, base=0, max_level=0):
        self.set_type = set_type
        self.nb_points = nb_points
        self.dim = dim
        self.merit = merit
        self.time = time
        self.gen_vector = gen_vector
        self.modulus = modulus
        self.nb_cols = nb_cols
        self.nb_rows = nb_rows
        self.matrices = matrices
        self.interlacing = interlacing

        if self.nb_cols == 0:   # ordinary set type
            self.base = base
            self.max_level = max_level
        else:
            self.base = 2
            self.max_level = nb_cols

    def __str__(self):
        s1 = "Result:\nNumber of points: %s" % (str(self.nb_points))
        if self.modulus != []:
            s2 = "\nModulus: %s" % (str(self.modulus))
        else:
            s2 = ""
        if self.gen_vector != []:
            s3 = "\nGenerating Vector: %s" % (str(self.gen_vector))
        else:
            s3 = ""
        s4 = "\nMerit value: %s \nCPU Time: %s seconds" % (str(self.merit), str(self.time))
        return s1 + s2 + s3 + s4
    
    def _repr_html_(self):
        s1 = "<span> <b> Number of points </b>: %s </span>" % (str(self.nb_points))
        if self.modulus != []:
            s2 = "<p> <b> Modulus </b>: %s </p>" % (str(self.modulus))
        else:
            s2 = ""
        if self.gen_vector != []:
            s3 = "<p> <b> Generating Vector </b>: %s </p>" % (str(self.gen_vector))
        else:
            s3 = ""            
        s4 = "<p> <b> Merit value </b>: %s </p> <p> <b> CPU Time </b>: %s seconds </p>" % (str(self.merit), str(self.time))
        return s1 + s2 + s3 + s4


    def getPoints(self, coord, level=None):
        assert coord < self.dim and (level==None or self.max_level > 0)

        if level is None:
            nb_points = self.nb_points
        else:
            nb_points = self.base ** level

        if self.matrices == []:
            return np.array([(self.gen_vector[coord] * i / nb_points) % 1 for i in range(nb_points)])

        else:
            points = []
            if level is None:
                level = self.nb_cols
            for x in range(nb_points):
                binary_repr = np.array([((x>>i)&1) for i in range(self.nb_cols)])
                prods = [np.mod(self.matrices[coord * self.interlacing + r].dot(binary_repr), 2) for r in range(self.interlacing)]
                points.append(sum([ prods[i % self.interlacing][i // self.interlacing] * (0.5 **(i+1)) for i in range(level*self.interlacing)]))
            return points



def parse_output(file_output):

    Lines = file_output.split("\n")
    sep = '  //'

    if Lines[0].split(sep)[0] == 'Ordinary':
        nb_points = int(Lines[1].split(sep)[0])
        dim = int(Lines[2].split(sep)[0])
        base = int(Lines[3].split(sep)[0])
        max_level = int(Lines[4].split(sep)[0])
        if base > 0:
            set_type = 'Ordinary-multi'
        else:
            set_type = 'Ordinary-uni'
        gen_vector = []
        for i in range(dim):
            gen_vector.append(int(Lines[5+i].split(sep)[0]))
        merit = float(Lines[5+dim].split(sep)[0])
        time = float(Lines[6+dim].split(sep)[0])
        return Result(set_type, nb_points, dim, merit, time, gen_vector=gen_vector, base=base, max_level=max_level)

    else:
        nb_cols = int(Lines[0].split(sep)[0])
        nb_rows = int(Lines[1].split(sep)[0])
        nb_points = int(Lines[2].split(sep)[0])
        interlacing = int(Lines[4].split(sep)[0])
        dim = int(Lines[3].split(sep)[0])
        set_type = Lines[5].split(sep)[0]

        line = 6

        if set_type == 'Polynomial':
            modulus = [int(x) for x in Lines[line].split('  //')[0].split(' ')]
            gen_vector = []
            for i in range(dim):
                gen_vector.append([int(x) for x in Lines[line+1+i].split(' ')])
            line += dim + 1

        elif set_type == 'Sobol':
            gen_vector = []
            for i in range(dim):
                gen_vector.append([int(x) for x in Lines[line+i].split(' ')])
            line += dim

        matrices = []
        for c in range(dim):
            line += 1
            M = []
            for i in range(nb_rows):
                M.append(np.array([int(x) for x in Lines[line+i].split(' ')]))
            line += nb_rows
            matrices.append(np.array(M))

        merit = float(Lines[line].split(sep)[0])
        time = float(Lines[line+1].split(sep)[0])

        if set_type == 'Polynomial':
            return Result(set_type, nb_points, dim // interlacing, merit, time, gen_vector=gen_vector, modulus=modulus, nb_cols=nb_cols, nb_rows=nb_rows, matrices=matrices, interlacing=interlacing)
        elif set_type == 'Sobol':
            return Result(set_type, nb_points, dim // interlacing, merit, time, gen_vector=gen_vector, nb_cols=nb_cols, nb_rows=nb_rows, matrices=matrices, interlacing=interlacing)
        elif set_type == 'Explicit':
            return Result(set_type, nb_points, dim // interlacing, merit, time, nb_cols=nb_cols, nb_rows=nb_rows, matrices=matrices, interlacing=interlacing)


    
