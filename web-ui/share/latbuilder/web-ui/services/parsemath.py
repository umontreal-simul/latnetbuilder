#!/usr/bin/env python

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


import ast
import operator
import math
import re

__all__ = ['eval']

OPERATORS = {
        ast.Add:    operator.add,
        ast.Sub:    operator.sub,
        ast.Mult:   operator.mul,
        ast.Div:    operator.truediv,
        ast.Pow:    operator.pow,
        ast.BitXor: operator.pow,
        ast.USub:   operator.neg,
        ast.Mod:    operator.mod,
        }

FUNCTIONS = {
        'exp':        math.exp,
        'log':        math.log,
        #'factorial':  math.factorial,
        }

def eval(expr, variables={}):
    # add multiplication sign where needed
    expr = re.sub(r"([0-9])\s*([A-Za-z])", r"\1*\2", expr) # coeff * var
    return Parser(variables)(expr)

class Parser(object):
    def __init__(self, variables={}):
        self._names = dict(FUNCTIONS)
        self._names.update(variables)
    def __call__(self, expr):
        return self._parse_node(ast.parse(expr, mode='eval').body)
    def _parse_node(self, node):
        if isinstance(node, ast.Num):
            return node.n
        if isinstance(node, ast.Name):
            return self._names[node.id]
        if isinstance(node, ast.Call):
            return self._parse_node(node.func)(
                    *(self._parse_node(n) for n in node.args))
        elif isinstance(node, ast.operator) or isinstance(node, ast.unaryop):
            return OPERATORS[type(node)]
        elif isinstance(node, ast.BinOp):
            return self._parse_node(node.op)(
                    self._parse_node(node.left),
                    self._parse_node(node.right))
        elif isinstance(node, ast.UnaryOp):
            return self._parse_node(node.op)(self._parse_node(node.operand))
        else:
            raise TypeError(node)

if __name__ == '__main__':
    import sys
    for expr in sys.argv[1:]:
        print(eval(expr, variables={'j': 10}))
