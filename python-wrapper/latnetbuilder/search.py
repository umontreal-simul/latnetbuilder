import subprocess
import os
import time
import logging
import traceback
from IPython.display import display
from jinja2 import Environment, PackageLoader
import numpy as np

from .parse_output import parse_output, Result
from .gui.output import output, create_output
from .gui.progress_bars import progress_bars

logging.basicConfig(filename='stderr.txt')

class Search():
    def __init__(self):
        self.modulus = ''
        self.dimension = 0
        self.multilevel_filters = []
        self.combiner = ''
        self.exploration_method = ''
        self.figure_of_merit = ''
        self.figure_power = '2'
        self.weights = []
        self.weights_power = 2  # to be changed for inf norm ?
        self.filters = []
        self.output = None

    def construct_command_line(self):
        pass
    def search_type(self):
        pass

    def _launch_subprocess(self, stdout_file, stderr_file):
        command = self.construct_command_line()
        process = subprocess.Popen(['exec ' + ' '.join(command)], stdout=stdout_file, stderr=stderr_file, shell=True)
        return process

    def _parse_progress(self, line):
        try_split = line.split('-')
        if len(try_split) == 1:
            current_nb_nets = int(line.split('/')[0].split(' ')[-1])
            total_nb_nets = int(line.split('/')[1])
            return (0, float(current_nb_nets) / total_nb_nets)
        elif len(try_split) == 2:
            current_nb_nets = int(try_split[1].split('/')[0].split(' ')[-1])
            total_nb_nets = int(try_split[1].split('/')[1])
            current_dim = int(try_split[0].split('/')[0].split(' ')[-1])
            total_dim = int(try_split[0].split('/')[1])
            return (float(current_dim) / total_dim, float(current_nb_nets) / total_nb_nets)

    def execute(self, stdout_filename='cpp_outfile.txt', stderr_filename='cpp_errfile.txt', delete_files=True, display_progress_bar=False):
        stdout_file = open(stdout_filename, 'w')
        stderr_file = open(stderr_filename, 'w')
        process = self._launch_subprocess(stdout_file, stderr_file)
        self._monitor_process(process, display_progress_bar=display_progress_bar)

    def _monitor_process(self, process, gui=None, display_progress_bar=False):
        search_type = self.search_type()
        try:
            if gui is not None:
                abort = gui.button_box.abort
                my_progress_bars = gui.progress_bars
                display_progress_bar = True
                my_result_obj = gui.output.result_obj
            else:
                from .gui import BaseGUIElement
                if display_progress_bar:
                    my_progress_bars = progress_bars()
                    my_progress_bars.progress_bar_dim.layout.display = 'flex'
                    my_progress_bars.progress_bar_nets.layout.display = 'flex'
                    display(my_progress_bars.progress_bar_nets)
                    display(my_progress_bars.progress_bar_dim)
                self.output = output()
                my_result_obj = self.output.result_obj
            
            while process.poll() is None:
                time.sleep(1)
                with open('cpp_outfile.txt','r') as f:
                    data = f.read()
                try:
                    last_line = data.split('\n')[-2]
                    prog_dimension, prog_net = self._parse_progress(last_line)
                    if display_progress_bar:
                        my_progress_bars.progress_bar_nets.value = prog_net
                        my_progress_bars.progress_bar_dim.value = prog_dimension
                except:
                    pass 
            
            if display_progress_bar:
                my_progress_bars.progress_bar_dim.layout.display = 'none'
                my_progress_bars.progress_bar_nets.layout.display = 'none'
            if gui is not None:
                abort.button_style = ''
                abort.disabled = True

            if process.poll() == 0:
                with open('cpp_outfile.txt') as f:
                    console_output = f.read()
                try:
                    with open('output_latnet.txt') as f:
                        file_output = f.read()
                except:
                    file_output = None
                parse_output(console_output, file_output, my_result_obj, search_type)

                if gui is not None:
                    gui.output.result_html.value = "<span> <b> Lattice Size </b>: %s </span> \
                    <p> <b> Generating Vector </b>: %s </p>\
                    <p> <b> Merit value </b>: %s </p>\
                    <p> <b> CPU Time </b>: %s s </p>" % (str(my_result_obj.latnet.size), str(my_result_obj.latnet.gen), str(my_result_obj.merit), str(my_result_obj.seconds))
                    create_output(gui.output)
                else:
                    print("Result:\n\
Lattice Size: %s \n\
Generating Vector: %s \n\
Merit value: %s \n\
CPU Time: %s s" % (str(my_result_obj.latnet.size), str(my_result_obj.latnet.gen), str(my_result_obj.merit), str(my_result_obj.seconds)))

            else:
                with open('cpp_errfile.txt') as f:
                    err_output = f.read()
                
                if gui is not None:
                    gui.output.result_html.value = '<span style="color:red"> %s </span>' % (err_output)
                    if err_output == '':
                        if abort.value == True:
                            gui.output.result_html.value = 'You aborted the search.'
                        else:
                            gui.output.result_html.value = '<span style="color:red"> The C++ process crashed without returning an error message. </span>'
                else:
                    if err_output == '':
                        print("The C++ process crashed without returning an error message.")
                    else:
                        print(err_output)

            
        except Exception as e:
            logging.warn(e)
            logging.warn(traceback.format_exc())
            if gui is not None:
                gui.output.result_html.value += '<span style="color:red"> An error happened in the communication with the C++ process. </span>'
            else:
                print("An error happened in the communication with the C++ process.")

    def rich_output(self):
        if self.output is None:
            print("Run self.execute() before outputing")
        else:
            display(self.output.output)
            create_output(self.output)

    def points(self, verbose=0):
        if self.output is None:
            print("Run self.execute() before using points")
        else:
            if len(self.output.output.children) == 0:
                create_output(self.output, create_graph=False)
            
            code = self.output.output.children[2].value
            if verbose > 0:
                print('Executing....')
                print(code)
            # loc = {}
            glob = {}
            exec(code, glob)
            return np.array(glob['points'])


class SearchLattice(Search):
    def __init__(self):
        self.lattice_type = ''
        self.embedded_lattice = False
        super(SearchLattice, self).__init__()

    def __repr__(self):
        return "TODO"
          
        
    def construct_command_line(self):
        from . import LATBUILDER
        command = [LATBUILDER,
                   '--set-type', 'lattice',
                   '--construction', self.lattice_type,
                   '--multilevel', str(self.embedded_lattice).lower(),
                   '--modulus', self.modulus,
                   '--figure-of-merit', self.figure_of_merit,
                   '--norm-type', self.figure_power,
                   '--construction', self.exploration_method,
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
            command += ['--output-format', 'file:output_latnet.txt,format:gui']
        return command

    def search_type(self):
        command = self.construct_command_line()
        if 'ordinary' in command:
            return 'ordinary'
        elif 'polynomial' in command:
            return 'polynomial'


class SearchNet(Search):
    def __init__(self):
        self.set_type = 'net'
        self.construction = ''
        super(SearchNet, self).__init__()
    
    def construct_command_line(self):
        from . import LATBUILDER
        command = [LATBUILDER,
                   '--set-type', 'net',
                   '--construction', self.construction,
                   '--set-type', self.set_type,
                   '--size', self.modulus,
                   '--exploration-method', self.exploration_method,
                   '--dimension', str(self.dimension),
                   '--verbose', '2',
                   '--output-format', 'file:output_latnet.txt,format:gui'
                   ]

        if len(self.filters) > 0:
            command += ['-add-figure', self.filters[-1]]
        
        command.append('--add-figure')
        command.append('/'.join([self.figure_of_merit, '1', self.figure_power, ' '.join(self.weights), str(self.weights_power)]))

        if self.combiner != '':
            command += ['--combiner', self.combiner]
        return command

    def search_type(self):
        return 'digital-' + self.construction