import subprocess
import os

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

    def construct_command_line(self):
        pass

    def execute_search(self, stdout_file, stderr_file):
        command = self.construct_command_line()
        process = subprocess.Popen(["export LD_LIBRARY_PATH=$HOME/dependencies/lib:$LD_LIBRARY_PATH && exec " +
                                    ' '.join(command)], stdout=stdout_file, stderr=stderr_file, shell=True)
        return process


class SearchLattice(Search):
    def __init__(self):
        self.lattice_type = ''
        self.embedded_lattice = False
        super(SearchLattice, self).__init__()
          
        
    def construct_command_line(self):
        from . import LATBUILDER
        command = [LATBUILDER,
                   '--main-construction', 'lattice',
                   '--lattice-type', self.lattice_type,
                   '--embedded-lattice', str(self.embedded_lattice).lower(),
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
            command += ['--output-poly', 'file:output_poly.txt,format:web']
        return command

    def search_type(self):
        command = self.construct_command_line()
        if 'ordinary' in command:
            return 'ordinary'
        elif 'polynomial' in command:
            return 'polynomial'


class SearchNet(Search):
    def __init__(self):
        self.set_type = ''
        self.construction = ''
        super(SearchNet, self).__init__()
    
    def construct_command_line(self):
        from . import LATBUILDER
        command = [LATBUILDER,
                   '--main-construction', 'net',
                   '--construction', self.construction,
                   '--set-type', self.set_type,
                   '--size', self.modulus,
                   '--exploration-method', self.exploration_method,
                   '--dimension', str(self.dimension),
                   '--verbose', '2',
                   '--GUI'
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