import inspect
import ipywidgets as widgets

from .properties import (size, lattice_type, properties_wrapper,
                        dimension, change_modulus, change_dimension)
from .filters import normalization, low_pass_filter, normalization_options, low_pass_filter_options, filters_wrapper
from .figure_of_merit import figure_type, alpha, evaluation_method, norm_type, figure_of_merit_expl, figure_of_merit_wrapper, change_figure_type, change_evaluation_method
from .multilevel import mult_normalization, mult_low_pass_filter, mult_combiner, multilevel_wrapper, minimum_level, maximum_level, mult_normalization_options, mult_low_pass_filter_options, combiner_level, combiner_dropdown, combiner_options, embedding, change_combiner_options
from .construction_method import constr_info, construction_choice, is_random, number_samples, generating_vector, construction, change_constr_choice
from .weights import weight_math, add_weight, weight_power, widget_weight, weights_wrapper, func_add_weights
from .button_box import go, abort, command_line, button_box_wrapper, build_command_line, abort_process, execute_search, display_button, display_output
from .output import result, result2, result3, command_line_out
from .main_tab import build_tabs
from .common import trigger_display

class Properties():
    def __init__(self, size, lattice_type, dimension, main):
        frame = inspect.currentframe()
        args, _, _, values = inspect.getargvalues(frame)
        for var_name in args:
            if var_name != 'self':
                setattr(self, var_name, values[var_name])

    def link_callbacks(self, gui):
        self.size.observe(lambda b: change_modulus(b, gui))
        self.dimension.observe(lambda b: change_dimension(b, gui))
        self.lattice_type.observe(lambda b: trigger_display(b, gui, 'lattice_type'))

class Filters():
    def __init__(self, normalization, low_pass_filter, normalization_options, low_pass_filter_options, main):
        frame = inspect.currentframe()
        args, _, _, values = inspect.getargvalues(frame)
        for var_name in args:
            if var_name != 'self':
                setattr(self, var_name, values[var_name])

    def link_callbacks(self, gui):
        self.normalization.observe(lambda b: trigger_display(b, gui, 'normalization'))
        self.low_pass_filter.observe(lambda b: trigger_display(b, gui, 'low_pass_filter'))

class FigureOfMerit():
    def __init__(self, figure_type, alpha, evaluation_method, norm_type, figure_of_merit_expl, main):
        frame = inspect.currentframe()
        args, _, _, values = inspect.getargvalues(frame)
        for var_name in args:
            if var_name != 'self':
                setattr(self, var_name, values[var_name])

    def link_callbacks(self, gui):
        self.figure_type.observe(lambda b: change_figure_type(b, gui))
        self.evaluation_method.observe(lambda b: change_evaluation_method(b, gui))

class Weights():
    def __init__(self, weight_math, add_weight, weight_power, widget_weight, main):
        frame = inspect.currentframe()
        args, _, _, values = inspect.getargvalues(frame)
        for var_name in args:
            if var_name != 'self':
                setattr(self, var_name, values[var_name])

    def link_callbacks(self, gui):
        self.add_weight.observe(lambda b: func_add_weights(b, gui))

class ConstructionMethod():
    def __init__(self, constr_info, construction_choice, is_random, number_samples, generating_vector, main):
        frame = inspect.currentframe()
        args, _, _, values = inspect.getargvalues(frame)
        for var_name in args:
            if var_name != 'self':
                setattr(self, var_name, values[var_name])
    
    def link_callbacks(self, gui):
        self.construction_choice.observe(lambda b:change_constr_choice(b, gui))

class Multilevel():
    def __init__(self, mult_normalization, mult_low_pass_filter, mult_combiner, multilevel, minimum_level, maximum_level, mult_normalization_options, mult_low_pass_filter_options, combiner_level, combiner_dropdown, combiner_options, main):
        frame = inspect.currentframe()
        args, _, _, values = inspect.getargvalues(frame)
        for var_name in args:
            if var_name != 'self':
                setattr(self, var_name, values[var_name])
    
    def link_callbacks(self, gui):
        self.mult_normalization.observe(lambda b: trigger_display(b, gui, 'mult_normalization'))
        self.mult_low_pass_filter.observe(lambda b: trigger_display(b, gui, 'mult_low_pass_filter'))
        self.mult_combiner.observe(lambda b: trigger_display(b, gui, 'mult_combiner'))
        self.combiner_dropdown.observe(lambda b:change_combiner_options(b, gui))

class ButtonBox():
    def __init__(self, go, abort, command_line, display_button, main):
        frame = inspect.currentframe()
        args, _, _, values = inspect.getargvalues(frame)
        for var_name in args:
            if var_name != 'self':
                setattr(self, var_name, values[var_name])

    def link_callbacks(self, gui):
        self.go.on_click(lambda b:execute_search(b, gui))
        self.command_line.on_click(lambda b:build_command_line(b, gui))
        self.display_button.observe(lambda b:display_output(b, gui))

class Output():
    def __init__(self, result, result2, result3, command_line_out):
        frame = inspect.currentframe()
        args, _, _, values = inspect.getargvalues(frame)
        for var_name in args:
            if var_name != 'self':
                setattr(self, var_name, values[var_name])
        self.my_output = widgets.Label('')


class GUI():
    def __init__(self, properties, filters, figure_of_merit, weights, construction_method, multi_level, button_box, output):
        self.properties = properties
        self.filters = filters
        self.figure_of_merit = figure_of_merit
        self.weights = weights
        self.construction_method = construction_method
        self.multi_level = multi_level
        self.button_box = button_box
        self.output = output

        self.link_all_callbacks()

    def link_all_callbacks(self):
        self.properties.link_callbacks(self)
        self.filters.link_callbacks(self)
        self.figure_of_merit.link_callbacks(self)
        self.weights.link_callbacks(self)
        self.construction_method.link_callbacks(self)
        self.multi_level.link_callbacks(self)
        self.button_box.link_callbacks(self)

properties = Properties(size, lattice_type, dimension, properties_wrapper)
filters = Filters(normalization, low_pass_filter, normalization_options, low_pass_filter_options, filters_wrapper)
figure_of_merit = FigureOfMerit(figure_type, alpha, evaluation_method, norm_type, figure_of_merit_expl, figure_of_merit_wrapper)
weights = Weights(weight_math, add_weight, weight_power, widget_weight, weights_wrapper)
construction_method = ConstructionMethod(constr_info, construction_choice, is_random, number_samples, generating_vector, construction)
multi_level = Multilevel(mult_normalization, mult_low_pass_filter, mult_combiner, multilevel_wrapper, minimum_level, maximum_level, mult_normalization_options, mult_low_pass_filter_options, combiner_level, combiner_dropdown, combiner_options, embedding)
button_box = ButtonBox(go, abort, command_line, display_button, button_box_wrapper)
output = Output(result, result2, result3, command_line_out)

gui = GUI(properties, filters, figure_of_merit, weights, construction_method, multi_level, button_box, output)
(gui.inside_tab, gui.main_tab) = build_tabs(gui)