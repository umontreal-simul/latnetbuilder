import subprocess

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
        command = [LATBUILDER,
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

    def execute_search(self, stdout_file, stderr_file):
        command = self.construct_command_line()
        process = subprocess.Popen(["export LD_LIBRARY_PATH=$HOME/dependencies/lib:$LD_LIBRARY_PATH && " +
                                    ' '.join(command)], stdout=stdout_file, stderr=stderr_file, shell=True)
        return process
