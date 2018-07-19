from ..search import SearchLattice, SearchNet
from .common import ParsingException

weights_corr = {
    'Product': 'product:',
    'Order-Dependent': 'order-dependent:',
    'POD': 'POD:',
    'Projection-Dependent' :'projection-dependent:'
}

def update(string, form, s):
    string += '0:'
    for i in range(int(s.dimension)):
        string += form.children[i].value
        if i != int(s.dimension)-1:
            string += ','
    return string

def parse_input(gui):
    if gui.main_tab.selected_index == 1:
        return parse_input_net(gui)
    else:
        return parse_input_lattice(gui)

def parse_input_common(s, gui):
    s.modulus = gui.properties.modulus.value
    if s.modulus == '' :
        raise ParsingException('Modulus or nb of points must be specified')

    s.dimension = gui.properties.dimension.value

    if gui.properties.is_multilevel.value:
        if gui.multi_level.mult_normalization.value:
            norm = "norm:P" + gui.figure_of_merit.figure_alpha.value + '-' + \
                gui.multi_level.mult_normalization_options.children[0].children[0].value.split(' ')[0]
            if gui.multi_level.minimum_level.value != '' and gui.multi_level.maximum_level.value != '':
                norm += ':select:' + gui.multi_level.minimum_level.value + ',' + gui.multi_level.maximum_level.value
            s.multilevel_filters.append(norm)
        if gui.multi_level.mult_low_pass_filter.value:
            s.multilevel_filters.append(
                "low-pass:" + gui.multi_level.mult_low_pass_filter_options.value)
        if gui.multi_level.mult_combiner.value:
            s.combiner = str(gui.multi_level.combiner_dropdown.value)
            if s.combiner == 'level:':
                s.combiner += str(gui.multi_level.combiner_level.value)

    exploration_method = gui.exploration_method.exploration_choice.value
    if gui.exploration_method.is_random.value and exploration_method in ['exhaustive', 'Korobov', 'CBC', 'full-CBC']:
        if exploration_method == 'exhaustive':
            exploration_method = 'random:'
        elif exploration_method == 'full-CBC':
            if gui.exploration_method.mixed_CBC_level.value == 1:
                exploration_method = 'random-CBC:'
            else:
                exploration_method = 'mixed-CBC:'
        else:   # exploration_method = 'Korobov' or 'CBC'
            exploration_method = 'random-' + exploration_method + ':'
        exploration_method += gui.exploration_method.number_samples.value
        if 'mixed-CBC' in exploration_method:
            exploration_method += ':' + str(gui.exploration_method.mixed_CBC_level.value)
    s.exploration_method = exploration_method

    merit = ''
    if gui.figure_of_merit.coord_unif.value:
        merit += 'CU:'
    figtype = gui.figure_of_merit.figure_type.value
    if figtype == 'Spectral':
        merit += 'spectral'
    elif figtype == 'Palpha':
        merit += 'P' + str(gui.figure_of_merit.figure_alpha.value)
    elif figtype == 'Ralpha':
        merit += 'R' + str(gui.figure_of_merit.figure_alpha.value)
    else:
        merit += figtype
    s.figure_of_merit = merit 

    s.figure_power = gui.figure_of_merit.figure_power.value

    VBOX_of_weights = gui.weights.VBOX_of_weights
    if len(VBOX_of_weights.children) == 0:
        raise ParsingException('You must specify at least one type of weight.')
    for k in range(len(VBOX_of_weights.children)):
        string = ''
        weight = VBOX_of_weights.children[k]
        weight_type = weights_corr[weight.children[0].children[0].value.split(' ')[1]]
        string += weight_type
        if weight_type == 'order-dependent:' or weight_type == 'product:':
            form = weight.children[1].children[0].children[1]
            string = update(string, form, s)
        elif weight_type == 'POD:':
            # Warning: inverse order in the GUI and in the CLI
            form = weight.children[2].children[0].children[1]
            string = update(string, form, s)
            form = weight.children[1].children[0].children[1]
            string += ':'
            string = update(string, form, s)
        else:
            proj_dep_string = weight.children[1].value
            string += proj_dep_string.replace('\n', ':')
        s.weights.append(string)


    # s.weights_power = int(gui.weights.weight_power.value)
    if s.figure_power.isdigit():
        s.weights_power = int(s.figure_power)
    else:
        s.weights_power = 1

def parse_input_net(gui):
    s = SearchNet()
    parse_input_common(s, gui)
    
    if gui.properties.is_multilevel.value:
        s.set_type = 'sequence'
    else:
        s.set_type = 'net'

    s.construction = gui.construction_method.construction_choice.value
    if s.construction == 'polynomial':
        if gui.construction_method.construction_modulus.value == '' :
            raise ParsingException('Modulus must be specified')
        s.construction += ':' + gui.construction_method.construction_modulus.value

    if gui.filters.equidistribution_filter.value:
        equi_value = 'equidistribution:' + str(gui.filters.equidistribution_weight.value) + '/'
        equi_value += str(gui.filters.equidistribution_options.value)
        s.filters.append(equi_value)

    if s.exploration_method == 'net-explicit:':
        s.exploration_method = 'evaluation:'
        if s.construction == 'sobol':
            s.exploration_method += gui.exploration_method.generating_numbers_sobol.value.replace('\n', '/')

        elif 'polynomial' in s.construction:
            for k in range(1, int(s.dimension)+1):
                s.exploration_method += gui.exploration_method.generating_vector_simple.children[k].value
                if k != int(s.dimension):
                    s.exploration_method += '/'

        elif s.construction == 'explicit':
            s_matrices = gui.exploration_method.generating_matrices.value
            s_matrices = s_matrices.replace('\n', ',').replace(',,', '/').replace(' ', '')
            s.construction += ':' + str(len(s_matrices.split(',')[0]))
            s.exploration_method += s_matrices
            
    return s


def parse_input_lattice(gui):
    s = SearchLattice()
    parse_input_common(s, gui)

    s.lattice_type = gui.lattice_type.type_choice.value

    s.embedded_lattice = gui.properties.is_multilevel.value

    if s.exploration_method == 'explicit:':
        modulus = gui.exploration_method.generating_vector.children[1].value
        if modulus != '':
            s.exploration_method = 'extend:' + modulus + ':'
        for k in range(1, int(s.dimension)+1):
            s.exploration_method += gui.exploration_method.generating_vector.children[0].children[k].value
            if k != int(s.dimension):
                s.exploration_method += ','

    if gui.filters.is_normalization.value:
        s.filters.append("norm:P" + gui.figure_of_merit.figure_alpha.value + '-' +
                         gui.filters.normalization_options.value.split(' ')[0])
    if gui.filters.low_pass_filter.value:
        s.filters.append("low-pass:" + gui.filters.low_pass_filter_options.value)

    return s
