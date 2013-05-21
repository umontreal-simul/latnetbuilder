#!/usr/bin/env python2

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
