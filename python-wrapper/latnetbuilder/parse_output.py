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
    # def __str__(self):
    #     return '{} : {} ({} s)'.format(self.latnetbuilder, self.merit, self.seconds)
    def __repr__(self):
        # return str(self)
        return "TODO: display"

    def getNet(self, coord, level=None):
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
                points.append(sum([ prods[i % self.interlacing][i / self.interlacing] * (0.5 **(i+1)) for i in range(level*self.interlacing)]))
            return points



def parse_output(file_output, output, search_type):

    Lines = file_output.split("\n")
    sep = '  //'

    if search_type == 'ordinary':
        nb_points = int(Lines[1].split(sep)[0])
        dim = int(Lines[2].split(sep)[0])
        base = int(Lines[3].split(sep)[0])
        max_level = int(Lines[4].split(sep)[0])
        if base > 0:
            set_type = 'ordinary-multi'
        else:
            set_type = 'ordinary-uni'
        gen_vector = []
        for i in range(dim):
            gen_vector.append(int(Lines[5+i].split(sep)[0]))
        merit = float(Lines[5+dim].split(sep)[0])
        time = float(Lines[6+dim].split(sep)[0])
        output.result_obj = Result(set_type, nb_points, dim, merit, time, gen_vector=gen_vector, base=base, max_level=max_level)

    else:
        nb_cols = int(Lines[0].split(sep)[0])
        nb_rows = int(Lines[1].split(sep)[0])
        nb_points = int(Lines[2].split(sep)[0])
        dim = int(Lines[3].split(sep)[0])
        interlacing = int(Lines[4].split(sep)[0])
        set_type = Lines[5].split(sep)[0]

        line = 7

        if set_type == 'polynomial':
            modulus = [int(x) for x in Lines[line].split(' ')]
            gen_vector = []
            for i in range(dim):
                gen_vector.append(int(Lines[line+1+i].split(' ')[0]))
            line += dim + 1

        elif set_type == 'sobol':
            gen_vector = []
            for i in range(dim):
                gen_vector.append(int(Lines[line+i].split(' ')[0]))
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

        if set_type == 'polynomial':
            output.result_obj = Result(set_type, nb_points, dim, merit, time, gen_vector=gen_vector, modulus=modulus, nb_cols=nb_cols, nb_rows=nb_rows, matrices=matrices, interlacing=interlacing)
        elif set_type == 'sobol':
            output.result_obj = Result(set_type, nb_points, dim, merit, time, gen_vector=gen_vector, nb_cols=nb_cols, nb_rows=nb_rows, matrices=matrices, interlacing=interlacing)
        elif set_type == 'explicit':
            output.result_obj = Result(set_type, nb_points, dim, merit, time, nb_cols=nb_cols, nb_rows=nb_rows, matrices=matrices, interlacing=interlacing)


    
