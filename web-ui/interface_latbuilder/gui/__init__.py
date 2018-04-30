import inspect
import ipywidgets as widgets
from IPython.display import display

from .properties import (modulus, lattice_type, properties_wrapper,
                         dimension, change_modulus, change_dimension, modulus_pretty)
from .filters import is_normalization, low_pass_filter, normalization_options, normalization_box, low_pass_filter_options, filters_wrapper, trigger_normalization_warning, normalization_warning
from .figure_of_merit import figure_type, figure_alpha, coord_unif, figure_power, figure_of_merit_expl, figure_of_merit_wrapper, change_figure_type, change_evaluation_method
from .multilevel import mult_normalization, mult_low_pass_filter, mult_combiner, multilevel_wrapper, minimum_level, maximum_level, mult_normalization_options, mult_low_pass_filter_options, combiner_level, combiner_dropdown, combiner_options, embedding, change_combiner_options
from .construction_method import constr_info, construction_choice, is_random, number_samples, generating_vector, construction, change_constr_choice, random_box
from .weights import add_weight, weight_power, widget_weight, weights_wrapper, func_add_weights
from .button_box import go, abort, command_line, button_box_wrapper, build_command_line, abort_process, execute_search, display_button, display_output
from .output import result, result2, result3, command_line_out
from .main_tab import build_tabs
from .common import trigger_display


class GUI():
    def __init__(self,
                 properties,
                 filters,
                 figure_of_merit,
                 weights,
                 construction_method,
                 multi_level,
                 button_box,
                 output):
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
        self.button_box.link_on_click_callbacks(self)

    def display(self):
        display(self.main_tab)


class BaseGUIElement():
    def __init__(self, **kwargs):
        self._name = {}
        for (key, value) in kwargs.items():
            if key != '_callbacks' and key != '_on_click_callbacks':
                setattr(self, key, value)
                self._name[value] = key
        if '_callbacks' in kwargs.keys():
            self._callbacks = {}  
            for (key, value) in kwargs['_callbacks'].items():
                self._callbacks[getattr(self, key)] = value
                if value == trigger_display:        # exception due to the structure of trigger_display
                    self._callbacks[getattr(self, key)] = lambda b, gui: trigger_display(b, gui, self._name[b['owner']])
        if '_on_click_callbacks' in kwargs.keys():
            self._on_click_callbacks = {}
            for (key, value) in kwargs['_on_click_callbacks'].items():
                self._on_click_callbacks[getattr(self, key)] = value

    def link_callbacks(self, gui):
        callbacks = self._callbacks
        for key in callbacks.keys():
            key.observe(lambda b: self._callbacks[b['owner']](b, gui))

    def link_on_click_callbacks(self, gui):
        callbacks = self._on_click_callbacks
        for key in callbacks.keys():
            key.on_click(lambda b: self._on_click_callbacks[b](b, gui))


properties = BaseGUIElement(modulus=modulus,
                            lattice_type=lattice_type,
                            dimension=dimension,
                            main=properties_wrapper,
                            modulus_pretty=modulus_pretty,
                            _callbacks={'modulus': change_modulus,
                                        'dimension': change_dimension,
                                        'lattice_type': trigger_display})

filters = BaseGUIElement(is_normalization=is_normalization,
                         low_pass_filter=low_pass_filter,
                         normalization_options=normalization_options,
                         normalization_warning=normalization_warning,
                         normalization_box=normalization_box,
                         low_pass_filter_options=low_pass_filter_options,
                         main=filters_wrapper,
                         _callbacks={'is_normalization': trigger_display,
                                     'low_pass_filter': trigger_display,
                                     'normalization_options': trigger_normalization_warning})

figure_of_merit = BaseGUIElement(figure_type=figure_type,
                                 figure_alpha=figure_alpha,
                                 coord_unif=coord_unif,
                                 figure_power=figure_power,
                                 figure_of_merit_expl=figure_of_merit_expl,
                                 main=figure_of_merit_wrapper,
                                 _callbacks={'figure_type': change_figure_type,
                                             'coord_unif': change_evaluation_method})


weights = BaseGUIElement(add_weight=add_weight,
                         weight_power=weight_power,
                         widget_weight=widget_weight,
                         main=weights_wrapper,
                         _callbacks={'add_weight': func_add_weights})

construction_method = BaseGUIElement(constr_info=constr_info,
                                     construction_choice=construction_choice,
                                     is_random=is_random,
                                     number_samples=number_samples,
                                     generating_vector=generating_vector,
                                     random_box=random_box,
                                     main=construction,
                                     _callbacks={'construction_choice': change_constr_choice,
                                                 'is_random': trigger_display})

multi_level = BaseGUIElement(mult_normalization=mult_normalization,
                             mult_low_pass_filter=mult_low_pass_filter,
                             mult_combiner=mult_combiner,
                             multilevel_wrapper=multilevel_wrapper,
                             minimum_level=minimum_level,
                             maximum_level=maximum_level,
                             mult_normalization_options=mult_normalization_options,
                             mult_low_pass_filter_options=mult_low_pass_filter_options,
                             combiner_level=combiner_level,
                             combiner_dropdown=combiner_dropdown,
                             combiner_options=combiner_options,
                             main=embedding,
                             _callbacks={'mult_normalization': trigger_display,
                                         'mult_low_pass_filter': trigger_display,
                                         'mult_combiner': trigger_display,
                                         'combiner_dropdown': change_combiner_options})

button_box = BaseGUIElement(go=go,
                            abort=abort,
                            command_line=command_line,
                            display_button=display_button,
                            main=button_box_wrapper,
                            _callbacks={'display_button': display_output},
                            _on_click_callbacks={'go': execute_search,
                                                 'command_line': build_command_line})

output = BaseGUIElement(result=result,
                        result2=result2,
                        result3=result3,
                        command_line_out=command_line_out,
                        my_output=widgets.Label(''))


gui = GUI(properties, filters, figure_of_merit, weights,
          construction_method, multi_level, button_box, output)
(gui.inside_tab, gui.main_tab) = build_tabs(gui)
