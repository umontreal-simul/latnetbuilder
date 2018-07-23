import ipywidgets as widgets
import threading
import os
import signal

from .parse_input import parse_input
from .common import ParsingException, BaseGUIElement

def build_command_line(change, gui):
    try:
        s = parse_input(gui)
        command = s.construct_command_line()
        gui.output.command_line_out.value = ' '.join([s if s is not None else '' for s in command])
    except ParsingException as e:
        gui.output.command_line_out.value = '<span style="color:red"> PARSING ERROR: ' + str(e) + '</span>'
    except Exception as e:
        gui.output.command_line_out.value = '<span style="color:red"> ERROR: ' + str(e) + '<br>Please contact the developers to report this error.</span>'

def abort_process(change, process):
    '''Callback fired when the user clicks the Abort button.
    
    The second argument is the subprocess instance which is terminated if the Abort toggle button is set to True.'''

    if change['name'] == 'value' and change['new'] == True:
        process.kill()

def on_click_search(change, gui):
    '''Callback fired when the user clicks the Search button.'''

    try:
        s = parse_input(gui)    # returns a Search instance
    except ParsingException as e:
        gui.output.result_html.value = '<span style="color:red"> PARSING ERROR: ' + str(e) + '</span>'
        return
    except Exception as e:    # unknown error, should not happen (means there is a bug in the Python code)
        gui.output.result_html.value = '<span style="color:red"> ERROR: ' + str(e) + '<br>Please contact the developers to report this error.</span>'
        return

    gui.search = s

    # display the progress bars
    if 'CBC' in s.exploration_method:
        gui.progress_bars.progress_bar_dim.value = 0
        gui.progress_bars.progress_bar_dim.layout.display = 'flex'
    else:
        gui.progress_bars.progress_bar_dim.layout.display = 'none'
    if 'evaluation' not in s.exploration_method:
        gui.progress_bars.progress_bar_nets.value = 0
        gui.progress_bars.progress_bar_nets.layout.display = 'flex'

    # reset output and button box to sensible values
    gui.output.command_line_out.value = ''
    gui.output.result_html.value = ''
    gui.output.output.layout.display = 'none' 
    gui.button_box.abort.disabled = False
    gui.button_box.abort.button_style = 'warning'
    gui.button_box.abort.value = False

    if not os.path.exists(s.output_folder):
        os.makedirs(s.output_folder)
    stdout_filepath = os.path.join(s.output_folder, 'cpp_outfile.txt')
    stderr_filepath = os.path.join(s.output_folder, 'cpp_errfile.txt')
    stdout_file = open(stdout_filepath, 'w')
    stderr_file = open(stderr_filepath, 'w')

    # call the LatNetBuilder executable and returns a subprocess instance
    process = s._launch_subprocess(stdout_file, stderr_file)
    gui.process = process

    # register the callback to abort the process
    gui.button_box.abort.observe(lambda change: abort_process(change, process))

    # launch the thread that will monitor the process, and update the GUI accordingly
    thread = threading.Thread(target=s._monitor_process, args=(process, stdout_filepath, stderr_filepath, gui, False, True))
    thread.start()


def button_box():
    go = widgets.Button(description='Search', disabled=False)
    abort = widgets.ToggleButton(
        value=False, description='Abort search', disabled=True, button_style='warning')
    command_line = widgets.Button(description='Construct command line',
                                disabled=False, layout=widgets.Layout(width='200px'))
    button_box_wrapper = widgets.HBox([go, abort, command_line],
                            layout=widgets.Layout(margin='30px 0px 0px 0px'))

    return BaseGUIElement(go=go,
                            abort=abort,
                            command_line=command_line,
                            main=button_box_wrapper,
                            _callbacks={},
                            _on_click_callbacks={'go': on_click_search,
                                                 'command_line': build_command_line})
