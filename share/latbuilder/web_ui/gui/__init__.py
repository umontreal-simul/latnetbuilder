import inspect
import ipywidgets as widgets
from IPython.display import display

from .properties import (modulus, is_multilevel, properties_wrapper,
                         dimension, change_modulus, change_dimension, modulus_pretty)
from .filters import is_normalization, low_pass_filter, normalization_options, normalization_box, low_pass_filter_options, filters_wrapper, normalization_warning, equidistribution_options, equidistribution_weight, equidistribution_box, equidistribution_filter
from .figure_of_merit import figure_type, figure_alpha, coord_unif, figure_power, figure_of_merit_expl, figure_of_merit_wrapper, change_figure_type, change_evaluation_method
from .multilevel import mult_normalization, mult_low_pass_filter, mult_combiner, multilevel_wrapper, minimum_level, maximum_level, mult_normalization_options, mult_low_pass_filter_options, combiner_level, combiner_dropdown, combiner_options, embedding, change_combiner_options
from .exploration_method import explr_info, exploration_choice, is_random, number_samples, generating_vector, exploration, change_explr_choice, random_box, mixed_CBC_level, trigger_is_random, generating_numbers_sobol, generating_vector_simple, generating_matrices, generating_numbers_sobol_box, generating_numbers_sobol_button, automatic_generating_numbers_sobol
from .construction_method import constr_info, construction_choice, construction, change_constr_choice, construction_modulus, construction_modulus_pretty, construction_modulus_box, change_constr_modulus
from .weights import add_weight, create_full_weight, weights_wrapper, func_add_weights, weights_button_id, weights_set_all_id, VBOX_of_weights
from .button_box import go, abort, command_line, button_box_wrapper, build_command_line, abort_process, on_click_search, display_output
from .output import result, result_obj, command_line_out, output 
from .progress_bars import progress_bar_dim, progress_bar_nets
from .main_tab import change_lattype
from .common import trigger_display


class GUI():
    def __init__(self,
                 properties,
                 filters,
                 figure_of_merit,
                 weights,
                 construction_method,
                 exploration_method,
                 multi_level,
                 button_box,
                 output,
                 progress_bars):
        self.properties = properties
        self.filters = filters
        self.figure_of_merit = figure_of_merit
        self.weights = weights
        self.construction_method = construction_method
        self.exploration_method = exploration_method
        self.multi_level = multi_level
        self.button_box = button_box
        self.output = output
        self.progress_bars = progress_bars

        self.link_all_callbacks()

    def link_all_callbacks(self):
        self.properties.link_callbacks(self)
        self.filters.link_callbacks(self)
        self.figure_of_merit.link_callbacks(self)
        self.weights.link_callbacks(self)
        self.construction_method.link_callbacks(self)
        self.exploration_method.link_callbacks(self)
        self.exploration_method.link_on_click_callbacks(self)
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
                if type(value) != dict:
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
                            is_multilevel=is_multilevel,
                            dimension=dimension,
                            main=properties_wrapper,
                            modulus_pretty=modulus_pretty,
                            _callbacks={'modulus': change_modulus,
                                        'dimension': change_dimension,
                                        'is_multilevel': trigger_display})

filters = BaseGUIElement(is_normalization=is_normalization,
                         low_pass_filter=low_pass_filter,
                         equidistribution_filter=equidistribution_filter,
                         normalization_options=normalization_options,
                         normalization_warning=normalization_warning,
                         normalization_box=normalization_box,
                         low_pass_filter_options=low_pass_filter_options,
                         equidistribution_box=equidistribution_box,
                         equidistribution_weight=equidistribution_weight,
                         equidistribution_options=equidistribution_options,
                         main=filters_wrapper,
                         _callbacks={'is_normalization': trigger_display,
                                     'low_pass_filter': trigger_display,
                                     'equidistribution_filter': trigger_display})

figure_of_merit = BaseGUIElement(figure_type=figure_type,
                                 figure_alpha=figure_alpha,
                                 coord_unif=coord_unif,
                                 figure_power=figure_power,
                                 figure_of_merit_expl=figure_of_merit_expl,
                                 main=figure_of_merit_wrapper,
                                 _callbacks={'figure_type': change_figure_type,
                                             'coord_unif': change_evaluation_method})


weights = BaseGUIElement(add_weight=add_weight,
                        #  weight_power=weight_power,
                         create_full_weight=create_full_weight,
                         weights_button_id=weights_button_id,
                         weights_set_all_id=weights_set_all_id,
                         VBOX_of_weights=VBOX_of_weights,
                         main=weights_wrapper,
                         _callbacks={'add_weight': func_add_weights})

construction_method = BaseGUIElement(constr_info=constr_info,
                                    construction_choice=construction_choice,
                                    construction_modulus=construction_modulus,
                                    construction_modulus_pretty=construction_modulus_pretty,
                                    construction_modulus_box=construction_modulus_box,
                                    main=construction,
                                    _callbacks={'construction_choice': change_constr_choice,
                                                'construction_modulus': change_constr_modulus})

exploration_method = BaseGUIElement(explr_info=explr_info,
                                     exploration_choice=exploration_choice,
                                     is_random=is_random,
                                     number_samples=number_samples,
                                     generating_vector=generating_vector,
                                     generating_vector_simple=generating_vector_simple,
                                     generating_matrices=generating_matrices,
                                     random_box=random_box,
                                     mixed_CBC_level=mixed_CBC_level,
                                     generating_numbers_sobol=generating_numbers_sobol,
                                     generating_numbers_sobol_box=generating_numbers_sobol_box,
                                     generating_numbers_sobol_button=generating_numbers_sobol_button,
                                     main=exploration,
                                     _callbacks={'exploration_choice': change_explr_choice,
                                                 'is_random': trigger_is_random},
                                     _on_click_callbacks={'generating_numbers_sobol_button':automatic_generating_numbers_sobol})

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
                            main=button_box_wrapper,
                            _callbacks={},
                            _on_click_callbacks={'go': on_click_search,
                                                 'command_line': build_command_line})

output = BaseGUIElement(result=result,
                        # result2=result2,
                        result_obj=result_obj,
                        output=output,
                        command_line_out=command_line_out)

progress_bars = BaseGUIElement(progress_bar_dim=progress_bar_dim,
                                progress_bar_nets=progress_bar_nets)


gui = GUI(properties, filters, figure_of_merit, weights,
          construction_method, exploration_method, multi_level, button_box, output, progress_bars)
func_add_weights({'name':'label', 'new':'Order-Dependent'}, gui)

inside_tab = widgets.VBox([gui.properties.main, 
                gui.multi_level.main,
                gui.construction_method.main,
                gui.exploration_method.main,
                gui.figure_of_merit.main, 
                gui.weights.main,  
                gui.filters.main,
                gui.button_box.main,
                gui.output.command_line_out,
                gui.progress_bars.progress_bar_dim,
                gui.progress_bars.progress_bar_nets,
                gui.output.result, 
                gui.output.output
                ])

gui.main_tab = widgets.Tab([inside_tab, inside_tab, inside_tab])
gui.main_tab.set_title(0, 'Ordinary Lattice')
gui.main_tab.set_title(1, 'Polynomial Lattice')
gui.main_tab.set_title(2, 'Digital Net')
gui.main_tab.observe(lambda b: change_lattype(b, gui), 'selected_index')

# for debug
gui._display_output = display_output
