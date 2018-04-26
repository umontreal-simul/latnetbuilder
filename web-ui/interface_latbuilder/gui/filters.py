import ipywidgets as widgets

from .common import style_default

normalization = widgets.Checkbox(
    description='Normalization', value=False, style=style_default)

low_pass_filter = widgets.Checkbox(
    description='Low-Pass Filter', value=False, style=style_default)

normalization_options = widgets.HBox([
    widgets.Dropdown(options=['SL10 Palpha', 'DPW08 Palpha'], description='Normalization Type:',
                     layout=widgets.Layout(width='30%'), style=style_default),
    widgets.HTML('<p style="color:red"> Warning: DPW08 Palpha works only with product weights</p>')],
    layout=widgets.Layout(display='none'))

low_pass_filter_options = widgets.Text(value='1', description='Low-Pass Threshold',
                                       layout=widgets.Layout(display='none', width='25%'), style=style_default)

filters_wrapper = widgets.Accordion([widgets.VBox([widgets.HBox([normalization, low_pass_filter]),
                                           normalization_options, low_pass_filter_options])])
filters_wrapper.set_title(0, 'Filters (optional)')
filters_wrapper.selected_index = None
