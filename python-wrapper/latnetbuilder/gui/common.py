"""Tools used by different elements of the GUI."""

import os
from pkg_resources import resource_string

import numpy as np

MAX_DIM_JOE_KUO = 300   # max dim of Joe and Kuo nets loaded in memory
INITIAL_DIM = 3     # default dimension for the GUI

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
        if i == MAX_DIM_JOE_KUO:
            break

style_default = {'description_width': 'initial'}

# key: the name of a sub-element of the GUI (i.e. an ipywidget)
# value: a list of length 2 [name of a GUI element, name of one of its subelements] which
# corresponds to the widget whose display status will changed in the callback launched by the key widget.
# For instance, it means that if the user checks the `is_multilevel` Checkbox, it will trigger
# the display of the main widget of the multi_level GUI element.
trigger_display_dic = {'is_multilevel': [['figure_of_merit', 'combiner_options']], 
                    'low_pass_filter': [['figure_of_merit', 'low_pass_filter_options']],
                    'is_normalization': [['figure_of_merit', 'minimum_level'], ['figure_of_merit', 'maximum_level']]}

def trigger_display(change, gui, owner):
    '''Change the display status of a widget.
    
    The arg owner corresponds to the name of the widget that triggers this callback.
    '''
    
    if change['name'] != 'value':
        return
    for obj_id in trigger_display_dic[owner]:
        obj = getattr(getattr(gui, obj_id[0]), obj_id[1])
        if not change['new']:
            obj.layout.display = 'none'
        elif change['new']:
            obj.layout.display = 'flex'

def parse_polynomial(s):
    '''Parse a polynomial string (e.g. 1101^7).
    
    Returns the corresponding polynomial in a Latex-friendly form.'''
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
        elif modulus[k] not in [0, 1]:
            return ''
    if poly_str == '':
        return ''
    if power != 1:
        poly_str = '(' + poly_str + ')^{' + str(power) + '}'  
            
    return poly_str

class ParsingException(Exception):
    '''Exception raised due to an error in the parsing of the input.'''
    pass


class BaseGUIElement():
    '''Class for base GUI elements.
    
    This class allows for a neat syntax when defining GUI elements, e.g.:
    my_element = BaseGUIElement('widget1': widget1, 
                                'widget2': widget2,
                                _callbacks:{'widget1': callback_widget1},
                                _on_click_callbacks:{'widget2'}: callback_widget2)
    where widget1 and widget2 designate ipywidgets, and callback_widget1 and callback_widget2 are widget callbacks with signature:
    callback_widget(change, gui). 

    The callbacks which are values in the dictionary _callbacks are linked with their corresponding key widget 
    thanks to the observe method: key.observe(lambda change: _callbacks[change['owner']](change, gui))
    The callbacks which are values in the dictionary _on_click_callbacks are linked with their corresponding key widget 
    thanks to the on_click method: key.observe(lambda change: _on_click_callbacks[change](change, gui)). It is useful
    in particular for Button widgets.

    Thus the example above is pretty much equivalent to doing:
    my_element.widget1 = widget1
    my_element.widget2 = widget2
    my_element.widget1.observe(lambda change: callback_widget1(change, gui))
    my_element.widget2.on_click(lambda change: callback_widget2(change, gui))

    There is a special case for the trigger_display callback (see trigger_display and trigger_display_dic above).
    If you want to use this callback, just write trigger_display as the value of the callback, 
    and add a new (key, value) pair to the trigger_display_dic.
    '''

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
                    self._callbacks[getattr(self, key)] = lambda change, gui: trigger_display(change, gui, self._name[change['owner']])
        if '_on_click_callbacks' in kwargs.keys():
            self._on_click_callbacks = {}
            for (key, value) in kwargs['_on_click_callbacks'].items():
                self._on_click_callbacks[getattr(self, key)] = value

    def link_callbacks(self, gui):
        callbacks = self._callbacks
        for key in callbacks.keys():
            key.observe(lambda change: self._callbacks[change['owner']](change, gui))

    def link_on_click_callbacks(self, gui):
        callbacks = self._on_click_callbacks
        for key in callbacks.keys():
            key.on_click(lambda change: self._on_click_callbacks[change](change, gui))