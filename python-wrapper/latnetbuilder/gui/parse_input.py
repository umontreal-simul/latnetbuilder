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

    s.dimension = gui.properties.dimension.value
    s.interlacing = gui.properties.interlacing.value

    s.multilevel = gui.properties.is_multilevel.value

    if gui.figure_of_merit.is_normalization.value:
        norm = "norm"
        if gui.figure_of_merit.minimum_level.value != '':
            norm += ':even:' + gui.figure_of_merit.minimum_level.value 
            if gui.figure_of_merit.maximum_level.value != '':
                norm += ',' + gui.figure_of_merit.maximum_level.value
        s.filters.append(norm)

    if gui.figure_of_merit.low_pass_filter.value:
        s.filters.append("low-pass:" + gui.figure_of_merit.low_pass_filter_options.value)

    if s.multilevel:
        s.combiner = str(gui.figure_of_merit.combiner_dropdown.value)
        if s.combiner == 'level:':
            s.combiner += str(gui.figure_of_merit.combiner_level.value)

    exploration_method = gui.exploration_method.exploration_choice.value
    if gui.exploration_method.is_random.value and exploration_method in ['exhaustive', 'Korobov', 'full-CBC']:
        if exploration_method == 'exhaustive':
            exploration_method = 'random:'
        elif exploration_method == 'full-CBC':
            if gui.exploration_method.mixed_CBC_level.value == 1:
                exploration_method = 'random-CBC:'
            else:
                exploration_method = 'mixed-CBC:'
        else:   # exploration_method = 'Korobov'
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
    elif 'alpha' in figtype:
        merit += figtype.split('alpha')[0] + str(gui.figure_of_merit.figure_alpha.value)
    else:
        merit += figtype
    s.figure_of_merit = merit 

    s.norm_type = gui.figure_of_merit.norm_type.value

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

    if gui.output_folder.children[1].value:
        s.output_folder = gui.output_folder.children[1].value

def parse_input_net(gui):
    s = SearchNet()
    parse_input_common(s, gui)

    s.construction = gui.construction_method.construction_choice.value

    if s.exploration_method == 'evaluation':
        if s.construction == 'sobol':
            s.exploration_method += ':' + gui.exploration_method.generating_numbers_sobol.value.replace('\n', '-')
        if s.exploration_method == 'evaluation:':
            raise ParsingException("The direction number input field cannot be left empty for Evaluation.")

        elif 'polynomial' in s.construction:
            s.exploration_method += ':'
            for k in range(int(s.dimension)):
                s.exploration_method += gui.exploration_method.generating_vector_simple.children[k].value
                if k != int(s.dimension):
                    s.exploration_method += '-'

        elif s.construction == 'explicit':
            s_matrices = gui.exploration_method.generating_matrices.value
            s_matrices = s_matrices.replace('\n', ',').replace(',,', '-').replace(' ', '')
            if s_matrices == '':
                raise ParsingException("The matrices input field cannot be left empty for Evaluation.")
            s.exploration_method += ':' + s_matrices
            
    return s


def parse_input_lattice(gui):
    s = SearchLattice()
    parse_input_common(s, gui)

    s.construction = gui.lattice_type.type_choice.value

    if s.exploration_method == 'evaluation':
        modulus = gui.exploration_method.generating_vector.children[1].value
        if modulus != '':
            s.exploration_method = 'extend:' + modulus + ':'
        else:
            s.exploration_method += ':'
        for k in range(int(s.dimension)):
            s.exploration_method += gui.exploration_method.generating_vector.children[0].children[k].value
            if k != int(s.dimension):
                s.exploration_method += '-'

    return s
