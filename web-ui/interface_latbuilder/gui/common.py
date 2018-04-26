style_default = {'description_width': 'initial'}

trigger_display_dic = {'lattice_type': ['multi_level', 'main'], 
                    'normalization': ['filters', 'normalization_options'],
                    'low_pass_filter': ['filters', 'low_pass_filter_options'],
                    'mult_normalization': ['multi_level', 'mult_normalization_options'],
                    'mult_low_pass_filter': ['multi_level', 'mult_low_pass_filter_options'],
                    'mult_combiner': ['multi_level', 'combiner_options']}

def trigger_display(b, gui, owner):
    obj_id = trigger_display_dic[owner]
    obj = getattr(getattr(gui, obj_id[0]), obj_id[1])
    if b['new'] == False:
        obj.layout.display = 'none'
    elif b['new'] == True:
        obj.layout.display = 'flex'