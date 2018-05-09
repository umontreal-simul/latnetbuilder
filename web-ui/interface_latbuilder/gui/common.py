import numpy as np

INITIAL_DIM = 3

style_default = {'description_width': 'initial'}

trigger_display_dic = {'is_embedded': ['multi_level', 'main'], 
                    'is_normalization': ['filters', 'normalization_box'],
                    'low_pass_filter': ['filters', 'low_pass_filter_options'],
                    'mult_normalization': ['multi_level', 'mult_normalization_options'],
                    'mult_low_pass_filter': ['multi_level', 'mult_low_pass_filter_options'],
                    'mult_combiner': ['multi_level', 'combiner_options'],
                    'is_random': ['construction_method', 'number_samples']}

def trigger_display(b, gui, owner):
    obj_id = trigger_display_dic[owner]
    obj = getattr(getattr(gui, obj_id[0]), obj_id[1])
    if b['new'] == False:
        obj.layout.display = 'none'
    elif b['new'] == True:
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