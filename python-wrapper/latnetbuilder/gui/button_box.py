import ipywidgets as widgets
import threading
import os
import signal

from .parse_input import parse_input
from .output import create_output
from .common import ParsingException, BaseGUIElement

def build_command_line(b, gui):
    try:
        s = parse_input(gui)
        command = s.construct_command_line()
        gui.output.command_line_out.value = ' '.join([s if s is not None else '' for s in command])
    except ParsingException as e:
        gui.output.command_line_out.value = '<span style="color:red"> PARSING ERROR: ' + str(e) + '</span>'
    except Exception as e:
        gui.output.command_line_out.value = '<span style="color:red"> ERROR: ' + str(e) + '</span>'

def abort_process(b, process):
    if b['name'] == 'value' and b['new'] == True:
        process.kill()
        # os.killpg(os.getpgid(process.pid), signal.SIGTERM)

def on_click_search(b, gui):
    try:
        s = parse_input(gui)
    except ParsingException as e:
        gui.output.result_html.value = '<span style="color:red"> PARSING ERROR: ' + str(e) + '</span>'
        return
    except:
        gui.output.result_html.value = '<span style="color:red"> ERROR: Something went wrong in the parsing of the input. Please double-check. </span>'
        return

    gui.search = s

    if 'digital' in s.search_type():    # TEMPORARY
        if 'CBC' in s.exploration_method:
            gui.progress_bars.progress_bar_dim.value = 0
            gui.progress_bars.progress_bar_dim.layout.display = 'flex'
        else:
            gui.progress_bars.progress_bar_dim.layout.display = 'none'
        if 'evaluation' not in s.exploration_method:
            gui.progress_bars.progress_bar_nets.value = 0
            gui.progress_bars.progress_bar_nets.layout.display = 'flex'

    gui.output.command_line_out.value = ''
    gui.output.result_html.value = ''
    gui.output.output.layout.display = 'none' 
    gui.button_box.abort.disabled = False
    gui.button_box.abort.button_style = 'warning'
    gui.button_box.abort.value = False
    stdout_file = open('cpp_outfile.txt', 'w')
    stderr_file = open('cpp_errfile.txt', 'w')
    process = s._launch_subprocess(stdout_file, stderr_file)
    gui.process = process
    gui.button_box.abort.observe(lambda b: abort_process(b, process))
    thread = threading.Thread(target=s._monitor_process, args=(process, gui))
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

# # for debug
# def display_output(b, gui):
#     if b['name'] == 'value' and b['new'] == True:
#         create_output(gui)