import ipywidgets as widgets

from .common import style_default

mult_normalization = widgets.Checkbox(
    description='Normalization', value=False, style=style_default)
mult_low_pass_filter = widgets.Checkbox(
    description='Low-Pass Filter', value=False, style=style_default)
mult_combiner = widgets.Checkbox(
    description='Combiner', value=False, style=style_default)

multilevel_wrapper = widgets.HBox(
    [mult_normalization, mult_low_pass_filter, mult_combiner])

# Definition of mult normalization options VBox
minimum_level = widgets.Text(placeholder='1', description='Min Level (optional):',
                             layout=widgets.Layout(width='22%', margin='0px 10px 0px 0px'), style=style_default)

maximum_level = widgets.Text(placeholder='10', description='Max Level (optional):',
                             layout=widgets.Layout(width='22%'), style=style_default)

mult_normalization_options = widgets.VBox([
    widgets.HBox([widgets.Dropdown(options=['SL10 Palpha', 'DPW08 Palpha'], description='Normalization Type:', layout=widgets.Layout(width='30%'), style=style_default),
                  widgets.HTML('<p style="color:red"> Warning: DPW08 Palpha works only with product weights</p>')]),
    widgets.HBox([minimum_level, maximum_level], layout=widgets.Layout(margin='0px 0px 0px 80px'))],
    layout=widgets.Layout(display='none'))

# Definition of mult low pass filter options widget
mult_low_pass_filter_options = widgets.Text(
    value='1',
    description='Low-Pass Threshold',
    layout=widgets.Layout(display='none', width='25%'),
    style=style_default
)

# Definition of combiner options HBox
combiner_level = widgets.BoundedIntText(
    description='Level:', min=1, layout=widgets.Layout(display='none', width='15%'))
combiner_dropdown = widgets.Dropdown(
    # options are (label, value) pairs
    options=[('weighted sum', 'sum'),
             ('maximum weighted value', 'max'),
             ('weighted value of the highest level', 'level:max'),
             ('weighted value of a specific level', 'level:')],
    value=None, description='Combiner:', style=style_default)
combiner_options = widgets.HBox(
    [combiner_dropdown, combiner_level], layout=widgets.Layout(display='none'))

# Wrapping
embedding = widgets.Accordion([widgets.VBox([multilevel_wrapper, mult_normalization_options, mult_low_pass_filter_options, combiner_options])],
                              layout=widgets.Layout(display='none'))
embedding.set_title(
    0, 'For Embedded Lattices: multilevel filters and combiners (optional)')
embedding.selected_index = None

def change_combiner_options(b, gui):
    # print(b)
    if b['name'] == 'value':
        if b['new'] == 'level:':
            gui.multi_level.combiner_level.layout.display = 'flex'
        else:
            gui.multi_level.combiner_level.layout.display = 'none'