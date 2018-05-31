import os
import csv
import numpy as np

dev_mod = not os.path.exists('../share/latbuilder/data/JoeKuoSobolNets.csv')

# joe and kuo sobol nets - necessary for digital net evaluation
JoeKuoSobolNets = []

if dev_mod:
    _JoeKuoSobolNetsPath = '../../data/JoeKuoSobolNets.csv'
    _primPolyPath = '../../data/primitive_polynomials.csv'
else:
    _JoeKuoSobolNetsPath = '../share/latbuilder/data/JoeKuoSobolNets.csv'
    _primPolyPath = '../share/latbuilder/data/primitive_polynomials.csv'

with open(_JoeKuoSobolNetsPath, newline='') as csvfile:
    csvreader = csv.reader(csvfile, delimiter=';')
    i=0
    for row in csvreader:
        JoeKuoSobolNets.append(",".join(row))
        i += 1
        if i == 100:
            break


INITIAL_DIM = 3

style_default = {'description_width': 'initial'}

trigger_display_dic = {'is_multilevel': ['multi_level', 'main'], 
                    'is_normalization': ['filters', 'normalization_box'],
                    'low_pass_filter': ['filters', 'low_pass_filter_options'],
                    'mult_normalization': ['multi_level', 'mult_normalization_options'],
                    'mult_low_pass_filter': ['multi_level', 'mult_low_pass_filter_options'],
                    'mult_combiner': ['multi_level', 'combiner_options'],
                    'equidistribution_filter': ['filters', 'equidistribution_box']}

def trigger_display(b, gui, owner):
    if b['name'] != 'value':
        return
    obj_id = trigger_display_dic[owner]
    obj = getattr(getattr(gui, obj_id[0]), obj_id[1])
    if not b['new']:
        obj.layout.display = 'none'
    elif b['new']:
        obj.layout.display = 'flex'

def parse_polynomial(s):
    poly_str = ''
    p = s.split('^')
    if len(p) == 2:
        base = np.array([int(y) for y in p[0]])
        power = int(p[1])
        modulus = np.flip(base, axis=0)
    else:
        base = np.array([int(y) for y in s])
        power = 1
        modulus = np.flip(base, axis=0)
    for k in range(len(modulus)):
        if modulus[k] == 1:
            if poly_str != '':
                poly_str += '+'
            poly_str += ' z^{' + str(len(modulus)-k-1) + '}'
    if power != 1:
        poly_str = '(' + poly_str + ')^{' + str(power) + '}'  
            
    if poly_str == '':
        poly_str = '0'
    return poly_str

class ParsingException(Exception):
    pass