import subprocess
import time
import os
import sys
import shutil
import logging
import traceback
import tarfile
from IPython.display import display, FileLink
import numpy as np

from .parse_output import parse_output, Result
from .gui.output import output, create_output
from .gui.progress_bars import progress_bars
from .generate_points import generate_points_digital_net, generate_points_ordinary_lattice

DEFAULT_OUTPUT_FOLDER = 'latnetbuilder_results'

class Search():
    def __init__(self):
        self.modulus = ''
        self.construction = ''
        self.dimension = 0
        self.interlacing = 1
        self.multilevel = False
        self.combiner = ''
        self.exploration_method = ''
        self.figure_of_merit = ''
        self.norm_type = '2'
        self.weights = []
        self.filters = []
        self.my_output = None
        self.set_type_name = ''
        self._output_folder = DEFAULT_OUTPUT_FOLDER

    def __repr__(self):
        return ("Construction method: %s\n" + \
    "Modulus: %s\n" + \
    "Multilevel: %s\n" + \
    "Dimension: %i\n" + \
    "Interlacing: %i\n" + \
    "Exploration method: %s\n" + \
    "Figure of merit: %s\n" + \
    "Norm type: %s\n" + \
    "Combiner: %s\n" + \
    "Filters: %s\n" + \
    "Weights: %s\n" + \
    "Output folder: %s\n") % (self.construction, self.modulus, str(self.multilevel), self.dimension, self.interlacing, self.exploration_method, self.figure_of_merit, self.norm_type, self.combiner, str(self.filters), str(self.weights), self._output_folder)

    def construct_command_line(self):
        '''Construct and return the command line to call LatNetBuilder as a list of strings'''

        # default value for modulus
        if self.modulus == '""':
            modulus = '"2^10"'
        else:
            modulus = self.modulus

        from . import PATH_TO_LATNETBUILDER
        command = [PATH_TO_LATNETBUILDER,
                   '--set-type', self.set_type_name,
                   '--construction', self.construction,
                   '--multilevel', str(self.multilevel).lower(),
                   '--size-parameter', modulus,
                   '--figure-of-merit', self.figure_of_merit,
                   '--norm-type', self.norm_type,
                   '--exploration-method', self.exploration_method,
                   '--verbose', '2',
                   '--dimension', str(self.dimension),
                   '--interlacing', str(self.interlacing),
                   '--output-folder', self._output_folder
                   ]
        command += ['--weights'] + self.weights
        if self.filters != []:
            command += ['--filters'] + self.filters
        if self.combiner != '':
            command += ['--combiner', self.combiner]
        return command

    def search_type(self):
        pass

    def _launch_subprocess(self, stdout_file, stderr_file):
        '''Call the C++ process using the Python module subprocess.
        
        This function is used by the GUI, but should NOT be called directly by the end user.'''

        command = self.construct_command_line()
        if sys.platform.startswith('win'):
            process = subprocess.Popen(command, stdout=stdout_file, stderr=stderr_file, shell=True)
        else:
            process = subprocess.Popen(['exec ' + ' '.join(command)], stdout=stdout_file, stderr=stderr_file, shell=True)
        # The exec keyword is essential as it allows to kill the latnetbuilder process using process.kill()
        # This syntax may not work without the exec keyword.
        return process

    def _parse_progress(self, line):
        '''Parse the progress information delivered by the C++ stdout. Useful for progress bars.'''
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

    def execute(self, output_folder=None, delete_files=True, stdout_filename='cpp_outfile.txt', stderr_filename='cpp_errfile.txt', display_progress_bar=False):
        '''Call the C++ process and monitor it.

        Arguments (all optional):
            + output_folder: set the path to the output folder, which will contain information about the inputs and outputs of the call to LatNetBuilder. By default, no output folder is created. 
            The folders contents may be overwritten. If the folder does not exist, it is created.
            + delete_files: if set to False, the C++ output files are not deleted at the end of the process
            + stdout_filename: name of the file which will contain the std output of the C++ executable
            + stdout_filename: name of the file which will contain the error output of the C++ executable
            + display_progress_bars: if set to True, ipywidgets progress bars are displayed (should be used only in the notebook)
        
        This function should be used by the end user if he instanciates a Search object.'''
        
        if output_folder is not None:
            self._output_folder = output_folder
            
        try:
            if not os.path.exists(self._output_folder):
                os.makedirs(self._output_folder)
            stdout_filepath = os.path.join(self._output_folder, stdout_filename)
            stderr_filepath = os.path.join(self._output_folder, stderr_filename)
            stdout_file = open(stdout_filepath, 'w')
            stderr_file = open(stderr_filepath, 'w')
        except Exception as e:
            print('ERROR: ' + str(e))
            return

        process = self._launch_subprocess(stdout_file, stderr_file)
        self._monitor_process(process, stdout_filepath, stderr_filepath, display_progress_bar=display_progress_bar, delete_files=delete_files)

    def _monitor_process(self, process, stdout_filepath, stderr_filepath, gui=None, display_progress_bar=False, delete_files=True):
        '''Monitor the C++ process.
        
        This function is called inside a thread by the GUI (with gui containing the gui object).
        It is called outside of any thread by the execute method.
        
        The function deals the monitoring both with and without a GUI interface. Thus it is a bit lenghty
        because the same information has to be treated in two different ways.'''

        try:
            if gui is not None:
                abort = gui.button_box.abort
                my_progress_bars = gui.progress_bars
                display_progress_bar = True
            else:
                if display_progress_bar:
                    # create and display the progress bars
                    my_progress_bars = progress_bars()
                    my_progress_bars.progress_bar_dim.layout.display = 'flex'
                    my_progress_bars.progress_bar_nets.layout.display = 'flex'
                    display(my_progress_bars.progress_bar_nets)
                    display(my_progress_bars.progress_bar_dim)

            self.my_output = output()
            
            while process.poll() is None:   # while the process is not finished
                time.sleep(0.1)
                with open(stdout_filepath,'r') as f:
                    data = f.read()
                try:
                    last_line = data.split('\n')[-2]    # read the before last line (the last line is always blank)
                    prog_dimension, prog_net = self._parse_progress(last_line)
                    if display_progress_bar:    # update progress bars
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

            if process.poll() == 0:     # the C++ process has finished normally
                with open(os.path.join(self._output_folder, 'output.txt')) as f:
                    file_output = f.read()
                result_obj = parse_output(file_output)

                if gui is not None:
                    gui.output.result_html.value = result_obj._repr_html_()
                    gui.output.result_obj = result_obj
                    create_output(gui.output)
                else:
                    print(result_obj)
                
                self.my_output.result_obj = result_obj

            else:   # an error occured in the C++ process
                with open(stdout_filepath) as f:
                    std_output = f.read()
                with open(stderr_filepath) as f:
                    err_output = f.read()
                
                if gui is not None:
                    gui.output.result_html.value = '<span style="color:red"> %s </span>' % (err_output)
                    if err_output == '':    # no error message
                        if abort.value == True:
                            gui.output.result_html.value = 'You aborted the search.'
                        else:
                            gui.output.result_html.value = '<span style="color:red"> The C++ process crashed without returning an error message (for example due to a segmentation fault).<br>Please contact the developers to report this error.</span>'
                
                else:
                    if err_output == '':
                        print("The C++ process crashed without returning an error message (for example due to a segmentation fault). Please contact the developers to report this error.")
                    else:
                        print(err_output)

            if delete_files:
                os.remove(os.path.join(self._output_folder, 'cpp_outfile.txt'))
                os.remove(os.path.join(self._output_folder, 'cpp_errfile.txt'))
            
        except Exception as e:
            error_file = os.path.join(self._output_folder, 'stderr.txt')
            # logging.basicConfig(filename = error_file)
            # logging.warn(e)
            # logging.warn(traceback.format_exc())
            # logging.close()
            with open(error_file, 'w') as g:
                g.write(traceback.format_exc())
            if gui is not None:
                gui.output.result_html.value += '<span style="color:red"> An error happened in the Python interface. In result archive, see file: stderr.txt </span>'
            else:
                print('An error happened in the Python interface. In result folder, see file: ' + error_file)
        
        finally:
            process.kill()
            try:
                if gui is not None:
                    with tarfile.open('latnetbuilder-results.tar.gz', "w:gz") as tar:
                        tar.add(self._output_folder, arcname=os.path.basename(self._output_folder))
                    shutil.make_archive('latnetbuilder-results', 'zip', self._output_folder)
                    gui.output.file_link.children[1].children[0].clear_output()
                    gui.output.file_link.children[1].children[1].clear_output()
                    with gui.output.file_link.children[1].children[0]:
                        display(FileLink('latnetbuilder-results.tar.gz'))
                    with gui.output.file_link.children[1].children[1]:
                        display(FileLink('latnetbuilder-results.zip'))
            
            except:
                gui.output.result_html.value += '<span style="color:red"> An error happened while trying to create the result archives. Please check that you have write permissions in the folder where this notebook runs. </span>'
            

    def rich_output(self):
        '''Print a rich output (plot and code) for the Search result'''
        if self.my_output is None:
            print("Run self.execute() before outputing")
        else:
            display(self.my_output.result_obj)
            create_output(self.my_output)
            display(self.my_output.output)


    def points(self, coordinate=None, level=None):
        '''Compute and return the QMC points of the Search result.
        
        The points are returned as a 2-dimensional numpy array, the first index corresponds to the index of the point,
        and the second corresponds to the coordinate.'''

        if self.my_output is None or self.my_output.result_obj is None:
            print("Run self.execute() before using points")
        else:
            result_obj = self.my_output.result_obj
            
            if self.construction == 'ordinary':
                if level == None:
                    return generate_points_ordinary_lattice(result_obj.gen_vector, result_obj.nb_points, coordinate)
                else:
                    return generate_points_ordinary_lattice(result_obj.gen_vector, result_obj.base ** level, coordinate)
            
            else:
                return generate_points_digital_net(result_obj.matrices, result_obj.interlacing, coordinate=coordinate, level=level)


class SearchLattice(Search):
    '''Specialization of the Search class to search for lattices'''

    def __init__(self):
        super(SearchLattice, self).__init__()   # calls the constructor of the parent class Search
        self.set_type_name = 'lattice'

    def __repr__(self):
        return "Point Set Type: Lattice\n" + super(SearchLattice, self).__repr__()
          
    def search_type(self):
        return self.construction


class SearchNet(Search):
    '''Specialization of the Search class to search for nets'''

    def __init__(self):
        super(SearchNet, self).__init__()       # calls the constructor of the parent class Search
        self.set_type_name = 'net'

    def __repr__(self):
        return "Point Set Type: Net\n" + super(SearchNet, self).__repr__()

    def search_type(self):
        return 'digital-' + self.construction