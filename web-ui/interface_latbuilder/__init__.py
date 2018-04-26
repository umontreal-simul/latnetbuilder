from IPython.display import display, HTML

import ipywidgets as widgets
import os, subprocess
import time

from .gui import *
from .execute import execute_search, build_command_line
from .callbacks import changed, changed2, func_add_weights, change_constr_choice, display_output, change_lattype, change_combiner_options, change_evaluation_method, change_dimension, change_modulus

# path to the latbuilder executable
LATBUILDER = 'latbuilder'

lattice_type.observe(changed)
normalization.observe(changed)
low_pass_filter.observe(changed)
mult_normalization.observe(changed)
mult_low_pass_filter.observe(changed)
figure_type.observe(changed2)
add_weight.observe(func_add_weights)
construction_choice.observe(change_constr_choice)
go.on_click(execute_search)
display_button.observe(display_output)
mult_combiner.observe(changed)
combiner_dropdown.observe(change_combiner_options)
evaluation_method.observe(change_evaluation_method)
dimension.observe(change_dimension)
main_gui.observe(change_lattype)
command_line.on_click(build_command_line)
size.observe(change_modulus)