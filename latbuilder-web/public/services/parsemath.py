#!/usr/bin/env python2

# Copyright (c) 2013 David Munger, Pierre L'Ecuyer, Université de Montréal.
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


import ast
import operator
import math

__all__ = ['eval']

OPERATORS = {
        ast.Add:    operator.add,
        ast.Sub:    operator.sub,
        ast.Mult:   operator.mul,
        ast.Div:    operator.truediv,
        ast.Pow:    operator.pow,
        ast.BitXor: operator.pow,
        }

FUNCTIONS = {
        'exp':  math.exp,
        'log':  math.log,
        }

def eval(expr, variables={}):
    names = dict(FUNCTIONS)
    names.update(variables)
    return parse_node(ast.parse(expr, mode='eval').body, names)

def parse_node(node, names):
    if isinstance(node, ast.Num):
        return node.n
    if isinstance(node, ast.Name):
        return names[node.id]
    if isinstance(node, ast.Call):
        return parse_node(node.func, names)(
                *(parse_node(n, names) for n in node.args))
    elif isinstance(node, ast.operator):
        return OPERATORS[type(node)]
    elif isinstance(node, ast.BinOp):
        return parse_node(node.op, names)(
                parse_node(node.left, names),
                parse_node(node.right, names))
    else:
        raise TypeError(node)

if __name__ == '__main__':
    import sys
    for expr in sys.argv[1:]:
        print(eval(expr, variables={'j': 10}))
