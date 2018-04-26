from .search import Search

weights_corr = {
    'Product': 'product:',
    'Order-Dependent': 'order-dependent:',
    'POD': 'POD:',
    'Projection-Dependent' :'projection-dependent:'
}

def update(string, form, s):
    string += form.children[int(s.dimension) - 1].value + ':'
    for i in range(int(s.dimension)):
        string += form.children[i].value
        if i != int(s.dimension)-1:
            string += ','
    return string

def parse_input(gui):
    s = Search()

    if gui.main_tab.selected_index == 0:
        s.lattice_type = 'ordinary'
    else:
        s.lattice_type = 'polynomial'

    s.embedded_lattice = gui.properties.lattice_type.value

    if gui.properties.size.value != '':
        s.modulus = gui.properties.size.value
    else:   # to be changed
        if s.lattice_type == 'ordinary':
            s.modulus = '1024'
        if s.lattice_type == 'polynomial':
            s.modulus = '01^8'

    merit = ''
    if gui.figure_of_merit.evaluation_method.value:
        merit += 'CU:'
    figtype = gui.figure_of_merit.figure_type.value
    if figtype == 'Spectral':
        merit += 'spectral'
    elif figtype == 'Palpha':
        merit += 'P' + str(gui.figure_of_merit.alpha.value)
    elif figtype == 'Ralpha':
        merit += 'R' + str(gui.figure_of_merit.alpha.value)
    else:
        merit += 'R'
    s.figure_of_merit = merit

    s.norm_type = gui.figure_of_merit.norm_type.value

    s.dimension = gui.properties.dimension.value

    # print(type(construction_choice))
    if gui.construction_method.construction_choice.value is not None:
        construction = gui.construction_method.construction_choice.value
    else:
        construction = 'CBC'
        # raise ValueError("Construction method must be specified")
    if gui.construction_method.is_random.value and construction in ['exhaustive', 'Korobov', 'CBC']:
        if construction == 'exhaustive':
            construction = 'random:'
        else:   # construction = 'Korobov' or 'CBC'
            construction = 'random-' + construction + ':'
        construction += gui.construction_method.number_samples.children[1].value
    if construction == 'explicit:':
        modulus = gui.construction_method.generating_vector.children[1].value
        if modulus != '':
            construction = 'extend:' + modulus + ':'
        for k in range(1, int(s.dimension)+1):
            construction += gui.construction_method.generating_vector.children[0].children[k].value
            if k != int(s.dimension):
                construction += ','
    s.construction = construction

    s.weights_power = int(gui.weights.weight_power.value)

    if gui.filters.normalization.value:
        s.filters.append("norm:P" + gui.figure_of_merit.alpha.value + '-' +
                         gui.filters.normalization_type.value.split(' ')[0])
    if gui.filters.low_pass_filter.value:
        s.filters.append("low-pass:" + gui.filters.low_pass_filter_options.value)

    if gui.properties.lattice_type.value:
        if gui.multi_level.mult_normalization.value:
            norm = "norm:P" + gui.figure_of_merit.alpha.value + '-' + \
                gui.multi_level.mult_normalization_options.value.split(' ')[0]
            if gui.multi_level.minimum_level.value != '' and gui.multi_level.maximum_level.value != '':
                norm += ':even:' + gui.multi_level.minimum_level.value + ',' + gui.multi_level.maximum_level.value
            s.multilevel_filters.append(norm)
        if gui.multi_level.mult_low_pass_filter.value:
            s.multilevel_filters.append(
                "low-pass:" + gui.multi_level.mult_low_pass_filter_options.value)
        if gui.multi_level.mult_combiner.value:
            s.combiner = str(gui.multi_level.combiner_dropdown.value)
            if s.combiner == 'level:':
                s.combiner += str(gui.multi_level.combiner_level.value)

    VBOX_of_weights = gui.weights.main.children[0].children[2]
    for k in range(len(VBOX_of_weights.children)):
        string = ''
        weight = VBOX_of_weights.children[k]
        weight_type = weights_corr[weight.children[0].children[0].value.split(' ')[
            1]]
        string += weight_type
        if weight_type == 'order-dependent:' or weight_type == 'product:':
            form = VBOX_of_weights.children[k].children[1].children[0].children[1]
            string = update(string, form, s)
        elif weight_type == 'POD:':
            form = VBOX_of_weights.children[k].children[1].children[0].children[1]
            string = update(string, form, s)
            form = VBOX_of_weights.children[k].children[1].children[2].children[1]
            string += ':'
            string = update(string, form, s)
        else:
            proj_dep_string = VBOX_of_weights.children[k].children[1].value
            string += proj_dep_string.replace('\n', ':')
        s.weights.append(string)

    return s
