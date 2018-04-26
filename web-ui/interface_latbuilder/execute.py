import time
import subprocess
import threading
import os

from .gui import *
from .parse import parse_output

weights_corr = {
    'Product': 'product:',
    'Order-Dependent': 'order-dependent:',
    'POD': 'POD:',
    'Projection-Dependent' :'projection-dependent:'
}

def work(process, result, result2, result3, abort, display_button, polynomial):
    while process.poll() is None:
        time.sleep(0.5)
        stdout_read = open('testfile.txt','r')
        data = stdout_read.read()
        result2.value = data
        stdout_read.close()
    if process.poll() == 0:
        abort.button_style = ''
        with open('testfile.txt') as f:
            output = f.read()
        ret = parse_output(output, polynomial)
        result3.lattice = ret[0].lattice
        result3.seconds = ret[0].seconds
        result3.polynomial = polynomial
        result3.matrices = ret[0].matrices
        result.value = "<p> <b> Lattice Size </b>: %s </p> \
        <p> <b> Generating Vector </b>: %s </p>\
        <p> <b> Merit value </b>: %s </p>\
        <p> <b> CPU Time </b>: %s s </p>" % (str(ret[0].lattice.size), str(ret[0].lattice.gen), str(ret[0].lattice.merit), str(ret[0].seconds))
        display_button.disabled = False
        display_button.button_style = 'success'
        display_button.value = False
    else:
        abort.button_style = ''
        with open('errfile.txt') as f:
            output = f.read()
        result.value = '<p style="color:red"> %s </p>' % (output)
        if output == '':
            result.value = 'You aborted the search.'
    abort.disabled = True


def abort_process(b, process):
    # print(b)
    if b['name'] == 'value' and b['new'] == True:
        process.kill()

class Search():
    def __init__(self):
        self.lattice_type = 'ordinary'
        self.embedded_lattice = False
        self.modulus = ''
        self.figure_of_merit = ''
        self.norm_type = '2'
        self.construction = ''
        self.dimension = 0
        self.filters = []
        self.multilevel_filters = []
        self.combiner = ''
        self.weights = []
        self.weights_power = 2  # to be changed for inf norm ?

    def construct_command_line(self):
        from . import LATBUILDER
        command=  [LATBUILDER,
            '--lattice-type', self.lattice_type,
            '--embedded-lattice', str(self.embedded_lattice).lower(),
            '--modulus', self.modulus,
            '--figure-of-merit', self.figure_of_merit,
            '--norm-type', self.norm_type,
            '--construction', self.construction,
            '--weights-power', str(self.weights_power),
            '--dimension', str(self.dimension),
            ]
        command += ['--weights'] + self.weights
        if self.filters != []:
            command += ['--filters'] + self.filters
        if self.multilevel_filters != []:
            command += ['--multilevel-filters'] + self.multilevel_filters
        if self.combiner != '':
            command += ['--combiner', self.combiner]
        if self.lattice_type == 'polynomial':
            command += ['--output-poly', 'file:output_poly.txt,format:web']
        return command


def update(string, form, s):
    string += form.children[int(s.dimension) -1].value + ':'
    for i in range(int(s.dimension)):
        string += form.children[i].value
        if i != int(s.dimension)-1:
            string += ','
    return string

def build_command_line(b=None):
    s = Search()

    if main_gui.selected_index == 0:
        s.lattice_type = 'ordinary'
    else:
        s.lattice_type = 'polynomial'

    s.embedded_lattice = lattice_type.value

    if size.value != '':
        s.modulus = size.value
    else:   # to be changed
        if s.lattice_type == 'ordinary':
            s.modulus = '1024'
        if s.lattice_type == 'polynomial':
            s.modulus = '01^8'

    merit = ''
    if evaluation_method.value:
        merit += 'CU:'
    figtype = figure_type.value
    if figtype == 'Spectral':
        merit += 'spectral'
    elif figtype == 'Palpha':
        merit += 'P' + str(alpha.value)
    elif figtype == 'Ralpha':
            merit += 'R' + str(alpha.value)
    else:
            merit += 'R'
    s.figure_of_merit = merit

    s.norm_type = norm_type.value

    s.dimension = dimension.value
    
    # print(type(construction_choice))
    if construction_choice.value is not None:
        construction = construction_choice.value
    else:
        construction = 'CBC'
        # raise ValueError("Construction method must be specified")
    if is_random.value and construction in ['exhaustive', 'Korobov', 'CBC']:
        if construction == 'exhaustive':
            construction = 'random:'
        else:   # construction = 'Korobov' or 'CBC'
            construction = 'random-' + construction + ':'
        construction += number_samples.children[1].value
    if construction == 'explicit:':
        modulus = generating_vector.children[1].value
        if modulus != '':
            construction = 'extend:' + modulus + ':'
        for k in range(1, int(s.dimension)+1):
            construction += generating_vector.children[0].children[k].value
            if k != int(s.dimension):
                construction += ','     
    s.construction = construction
    
    s.weights_power = int(weight_power.value)

    if normalization.value:
        s.filters.append("norm:P" + alpha.value + '-' + normalization_type.value.split(' ')[0])
    if low_pass_filter.value:
        s.filters.append("low-pass:" + low_pass_filter_options.value)

    if lattice_type.value:
        if mult_normalization.value:
            norm  = "norm:P" + alpha.value + '-' + normalization_type.value.split(' ')[0]
            if minimum_level.value != '' and maximum_level.value != '':
                norm += ':even:' + minimum_level.value + ',' + maximum_level.value
            s.multilevel_filters.append(norm)
        if mult_low_pass_filter.value:
            s.multilevel_filters.append("low-pass:" + mult_low_pass_filter_options.value)
        if mult_combiner.value:
            s.combiner = combiner_dropdown.value
            if s.combiner == 'level:':
                s.combiner += str(combiner_level.value)

    
    VBOX_of_weights = weights.children[0].children[2]
    for k in range(len(VBOX_of_weights.children)):
        string = ''
        # string += '"'
        weight = VBOX_of_weights.children[k]
        weight_type = weights_corr[weight.children[0].children[0].value.split(' ')[1]]
        # weight_type = weight.children[0].children[0].value.split(' ')[1]
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
            # if proj_dep_string != '':
            #     with open(os.path.join(os.path.split(LATBUILDER)[0], 'weights.txt'), 'w') as f: 
            #         f.write(proj_dep_string)
            # string += 'weights.txt'
        # string += '"'
        s.weights.append(string)

    # print(s.construct_command_line())
    command = s.construct_command_line()
    command_line_out.value = ' '.join(command)
    return command

def execute_search(b):
    try:
        command = build_command_line()
    except:
        result.value = '<p style="color:red"> Something went wrong in the parsing of the input. Please double-check. </p>'
        return
    # print(command)
    result.value = ''
    polynomial = ('polynomial' in command)
    abort.disabled = False
    abort.button_style = 'warning'
    abort.value = False
    abort.observe(lambda b: abort_process(b, process))
    stdout_file = open('testfile.txt', 'w')
    stderr_file = open('errfile.txt', 'w')
    process = subprocess.Popen(["export LD_LIBRARY_PATH=$HOME/dependencies/lib:$LD_LIBRARY_PATH && " + ' '.join(command)], stdout=stdout_file, stderr=stderr_file, shell=True)
    thread = threading.Thread(target=work, args=(process, result, result2, result3, abort, display_button, polynomial))
    thread.start()
    