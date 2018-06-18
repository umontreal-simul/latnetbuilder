import inspect
import ipywidgets as widgets
from IPython.display import display

from .properties import properties
from .filters import filters
from .figure_of_merit import figure_of_merit
from .multilevel import multilevel
from .exploration_method import exploration_method
from .construction_method import construction_method
from .weights import weights, func_add_weights
from .button_box import button_box
from .output import output
from .progress_bars import progress_bars
from .main_tab import change_lattype
from .common import trigger_display, BaseGUIElement
from .lattice_type import lattice_type


class GUI():
    def __init__(self,
                 lattice_type,
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
        self.lattice_type = lattice_type
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
        self.search = None

        self.link_all_callbacks()

    def link_all_callbacks(self):
        self.lattice_type.link_callbacks(self)
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

    def _ipython_display_(self):
        display(self.main_tab)

gui = GUI(lattice_type(), properties(), filters(), figure_of_merit(), weights(),
          construction_method(), exploration_method(), multilevel(), button_box(), output(), progress_bars())
func_add_weights({'name':'label', 'new':'Order-Dependent'}, gui)

inside_tab = widgets.VBox([gui.lattice_type.main,
                gui.construction_method.main,
                gui.properties.main, 
                gui.multi_level.main,
                gui.exploration_method.main,
                gui.figure_of_merit.main, 
                gui.weights.main,  
                gui.filters.main,
                gui.button_box.main,
                gui.output.command_line_out,
                gui.progress_bars.progress_bar_dim,
                gui.progress_bars.progress_bar_nets,
                gui.output.result_html, 
                gui.output.output
                ])

gui.main_tab = widgets.Tab([inside_tab, inside_tab])
gui.main_tab.set_title(0, 'Ordinary Lattice')
gui.main_tab.set_title(1, 'Digital Net')
gui.main_tab.observe(lambda b: change_lattype(b, gui), 'selected_index')

# for debug
# gui._display_output = display_output
