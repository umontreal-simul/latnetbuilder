import ipywidgets as widgets

from .common import style_default

is_normalization = widgets.Checkbox(description='Normalization', value=False, style=style_default)

low_pass_filter = widgets.Checkbox(description='Low-Pass Filter', value=False, style=style_default)

normalization_options = widgets.Dropdown(options=['SL10 Palpha', 'DPW08 Palpha'], description='Normalization Type:',
                     layout=widgets.Layout(width='30%'), style=style_default)
normalization_warning = widgets.HTML('<p style="color:red"> Warning: DPW08 Palpha works only with product weights</p>',
                                    layout=widgets.Layout(display='none'))
normalization_box = widgets.HBox([normalization_options, normalization_warning], layout=widgets.Layout(display='none'))

low_pass_filter_options = widgets.Text(value='1', description='Low-Pass Threshold',
                                       layout=widgets.Layout(display='none', width='25%'), style=style_default)

filters_wrapper = widgets.Accordion([widgets.VBox([widgets.HBox([is_normalization, low_pass_filter]),
                                           normalization_box, low_pass_filter_options])])
filters_wrapper.set_title(0, 'Filters (optional)')
filters_wrapper.selected_index = None

def trigger_normalization_warning(b, gui):
    if b['name'] != 'value':
        return
    new_choice = b['new']
    if new_choice == 'DPW08 Palpha':
        gui.filters.normalization_warning.layout.display = 'flex'
    else:
        gui.filters.normalization_warning.layout.display = 'none'