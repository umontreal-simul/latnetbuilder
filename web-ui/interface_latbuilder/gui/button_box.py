import ipywidgets as widgets
import threading
import time
from IPython.display import display

from ..parse_input import parse_input
from ..parse_output import parse_output
from .output import create_output

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
    gui.output.command_line_out.value = ' '.join(command)
    return command

def abort_process(b, process):
    if b['name'] == 'value' and b['new'] == True:
        process.kill()

def work(process, gui, polynomial):
    result = gui.output.result
    result_obj = gui.output.result_obj
    abort = gui.button_box.abort
    
    while process.poll() is None:
        time.sleep(0.2)
        # stdout_read = open('testfile.txt','r')
        # data = stdout_read.read()
        # # result2.value = data
        # stdout_read.close()
    if process.poll() == 0:
        abort.button_style = ''
        with open('testfile.txt') as f:
            output = f.read()
        ret = parse_output(output, polynomial)
        result_obj.lattice = ret[0].lattice
        result_obj.seconds = ret[0].seconds
        result_obj.polynomial = polynomial
        result_obj.matrices = ret[0].matrices
        result.value = "<p> <b> Lattice Size </b>: %s </p> \
        <p> <b> Generating Vector </b>: %s </p>\
        <p> <b> Merit value </b>: %s </p>\
        <p> <b> CPU Time </b>: %s s </p>" % (str(ret[0].lattice.size), str(ret[0].lattice.gen), str(ret[0].lattice.merit), str(ret[0].seconds))
        create_output(gui)
    else:
        abort.button_style = ''
        with open('errfile.txt') as f:
            output = f.read()
        result.value = '<p style="color:red"> %s </p>' % (output)
        if output == '':
            result.value = 'You aborted the search.'
    abort.disabled = True

def on_click_search(b, gui):
    try:
        s = parse_input(gui)
        command = s.construct_command_line()
    except:
        gui.output.result.value = '<p style="color:red"> Something went wrong in the parsing of the input. Please double-check. </p>'
        return

    gui.output.result.value = ''
    gui.output.output.layout.display = 'none'
    polynomial = ('polynomial' in command)
    gui.button_box.abort.disabled = False
    gui.button_box.abort.button_style = 'warning'
    gui.button_box.abort.value = False
    gui.button_box.abort.observe(lambda b: abort_process(b, process))
    stdout_file = open('testfile.txt', 'w')
    stderr_file = open('errfile.txt', 'w')
    process = s.execute_search(stdout_file, stderr_file)
    thread = threading.Thread(target=work, args=(process, gui, polynomial))
    thread.start()

def display_output(b, gui):
    if b['name'] == 'value' and b['new'] == True:
        create_output(gui)
        display(gui.output.my_output)
    if b['name'] == 'value' and b['new'] == False:
        gui.output.my_output.layout.display = 'none'