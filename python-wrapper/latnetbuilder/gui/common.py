import os
from pkg_resources import resource_string

import numpy as np

# primitive polynomials - necessary for sobol net output
_primPolyRaw = resource_string(__name__, '../data/primitive_polynomials.csv').decode("utf-8")

# joe and kuo sobol nets - necessary for sobol net interface
JoeKuoSobolNetsRaw = resource_string(__name__, '../data/JoeKuoSobolNets.csv').decode("utf-8")

JoeKuoSobolNets = []
lines = JoeKuoSobolNetsRaw.split('\r\n')
i=0
for line in lines:
    if len(line) > 0 and line[0].isdigit():
        i+=1
        JoeKuoSobolNets.append(line)
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


class BaseGUIElement():
    def __init__(self, **kwargs):
        self._name = {}
        for (key, value) in kwargs.items():
            if key != '_callbacks' and key != '_on_click_callbacks':
                setattr(self, key, value)
                if type(value) != dict:
                    self._name[value] = key
        if '_callbacks' in kwargs.keys():
            self._callbacks = {}  
            for (key, value) in kwargs['_callbacks'].items():
                self._callbacks[getattr(self, key)] = value
                if value == trigger_display:        # exception due to the structure of trigger_display
                    self._callbacks[getattr(self, key)] = lambda b, gui: trigger_display(b, gui, self._name[b['owner']])
        if '_on_click_callbacks' in kwargs.keys():
            self._on_click_callbacks = {}
            for (key, value) in kwargs['_on_click_callbacks'].items():
                self._on_click_callbacks[getattr(self, key)] = value

    def link_callbacks(self, gui):
        callbacks = self._callbacks
        for key in callbacks.keys():
            key.observe(lambda b: self._callbacks[b['owner']](b, gui))

    def link_on_click_callbacks(self, gui):
        callbacks = self._on_click_callbacks
        for key in callbacks.keys():
            key.on_click(lambda b: self._on_click_callbacks[b](b, gui))