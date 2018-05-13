import ipywidgets as widgets
import threading
import time
from IPython.display import display

from ..parse_input import parse_input
from ..parse_output import parse_output
from .output import create_output
# from .common import 

go = widgets.Button(description='Search', disabled=False)
abort = widgets.ToggleButton(
    value=False, description='Abort search', disabled=True, button_style='warning')
command_line = widgets.Button(description='Construct command line',
                              disabled=False, layout=widgets.Layout(width='200px'))
button_box_wrapper = widgets.HBox([go, abort, command_line],
                          layout=widgets.Layout(margin='30px 0px 0px 0px'))

def build_command_line(b, gui):
    s = parse_input(gui)
    command = s.construct_command_line()
    gui.output.command_line_out.value = ' '.join([s if s is not None else '' for s in command])
    return command

def abort_process(b, process):
    if b['name'] == 'value' and b['new'] == True:
        process.kill()

def parse_progress(line):
    try_split = line.strip('\n').split('-')
    if len(try_split == 1):
        current_nb_nets = int(line.split('/')[0].split(' ')[-1])
        total_nb_nets = int(line.split('/')[1])
        return (0, float(current_nb_nets) / total_nb_nets)
    elif len(try_split == 2):
        current_nb_nets = int(try_split[1].split('/')[0].split(' ')[-1])
        total_nb_nets = int(try_split[1].split('/')[1])
        current_dim = int(try_split[0].split('/')[0].split(' ')[-1])
        total_dim = int(try_split[0].split('/')[1])
        return (float(current_dim) / total_dim, float(current_nb_nets) / total_nb_nets)

def work(process, gui, search_type):
    result_widget = gui.output.result
    result_obj = gui.output.result_obj
    abort = gui.button_box.abort
    
    while process.poll() is None:
        time.sleep(0.2)
        with open('testfile.txt','r') as f:
            data = f.read()
        last_line = data.split('\n')[-1]
        try:
            prog_dimension, prog_net = parse_progress(last_line)
            gui.progress_bars.progress_bar_nets.value = prog_net
            gui.progress_bars.progress_bar_dim.value = prog_dimension
        except:
            pass 
    
    gui.progress_bars.progress_bar_dim.layout.display = 'none'
    gui.progress_bars.progress_bar_nets.layout.display = 'none'

    if process.poll() == 0:
        abort.button_style = ''
        with open('testfile.txt') as f:
            output = f.read()
        parse_output(output, gui, search_type)
        result_widget.value = "<p> <b> Lattice Size </b>: %s </p> \
        <p> <b> Generating Vector </b>: %s </p>\
        <p> <b> Merit value </b>: %s </p>\
        <p> <b> CPU Time </b>: %s s </p>" % (str(result_obj.latnet.size), str(result_obj.latnet.gen), str(result_obj.merit), str(result_obj.seconds))
        if search_type in ['ordinary', 'polynomial', 'digital-sobol']:
            create_output(gui)
    else:
        abort.button_style = ''
        with open('errfile.txt') as f:
            output = f.read()
        result_widget.value = '<p style="color:red"> %s </p>' % (output)
        if output == '':
            result_widget.value = 'You aborted the search.'

    abort.disabled = True


def on_click_search(b, gui):
    try:
        s = parse_input(gui)
    except Exception as e:
        gui.output.result.value = '<p style="color:red"> Something went wrong in the parsing of the input. Please double-check. </p>'
        return

    if 'digital' in s.search_type():    # TEMPORARY
        if 'CBC' in s.exploration_method:
            gui.progress_bars.progress_bar_dim.layout.display = 'flex'
        else:
            gui.progress_bars.progress_bar_dim.layout.display = 'none'
        gui.progress_bars.progress_bar_nets.layout.display = 'flex'

    gui.output.command_line_out.value = ''
    gui.output.result.value = ''
    gui.output.output.layout.display = 'none' 
    gui.button_box.abort.disabled = False
    gui.button_box.abort.button_style = 'warning'
    gui.button_box.abort.value = False
    gui.button_box.abort.observe(lambda b: abort_process(b, process))
    stdout_file = open('testfile.txt', 'w')
    stderr_file = open('errfile.txt', 'w')
    process = s.execute_search(stdout_file, stderr_file)
    thread = threading.Thread(target=work, args=(process, gui, s.search_type()))
    thread.start()

# for debug
def display_output(b, gui):
    if b['name'] == 'value' and b['new'] == True:
        create_output(gui)
        display(gui.output.my_output)
    if b['name'] == 'value' and b['new'] == False:
        gui.output.my_output.layout.display = 'none'