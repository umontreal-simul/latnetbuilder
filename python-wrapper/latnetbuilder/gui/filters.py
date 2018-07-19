import ipywidgets as widgets

from .common import style_default, BaseGUIElement, trigger_display

def change_combiner_options(change, gui):
    if change['name'] == 'value':
        if change['new'] == 'level:':
            gui.filters.combiner_level.layout.display = 'flex'
        else:
            gui.filters.combiner_level.layout.display = 'none'
        if change['new'] in ['level:', 'level:max']:
            gui.filters.minimum_level.layout.display = 'none'
            gui.filters.maximum_level.layout.display = 'none'
        else:
            gui.filters.minimum_level.layout.display = 'flex'
            gui.filters.maximum_level.layout.display = 'flex'

def filters():
    is_normalization = widgets.Checkbox(description='Normalization', value=False, style=style_default)
    low_pass_filter = widgets.Checkbox(description='Low-Pass Filter', value=False, style=style_default)

    # Definition of mult is_normalization options VBox
    minimum_level = widgets.Text(placeholder='1', description='Min Level (optional):',
                                layout=widgets.Layout(width='22%', margin='0px 10px 0px 0px'), style=style_default)

    maximum_level = widgets.Text(placeholder='10', description='Max Level (optional):',
                                layout=widgets.Layout(width='22%'), style=style_default)

    # Definition of combiner options HBox
    combiner_level = widgets.BoundedIntText(
        description='Level:', min=1, layout=widgets.Layout(display='none', width='15%'))
    combiner_dropdown = widgets.Dropdown(
        # options are (label, value) pairs
        options=[('weighted sum', 'sum'),
                ('maximum weighted value', 'max'),
                ('weighted value of the highest level', 'level:max'),
                ('weighted value of a specific level', 'level:')],
        value='sum', description='Combiner:', style=style_default)
    combiner_options = widgets.HBox(
        [combiner_dropdown, combiner_level], layout=widgets.Layout(display='none'))    
    
    low_pass_filter_options = widgets.Text(value='1', description='Low-Pass Threshold',
                                        layout=widgets.Layout(display='none', width='25%'), style=style_default)

    filters_wrapper = widgets.Accordion([widgets.VBox(
        [combiner_options, 
        widgets.HBox([is_normalization, low_pass_filter], layout=widgets.Layout(justify_content='space-around')), 
        low_pass_filter_options])])
    filters_wrapper.set_title(0, 'Filters and combiner')
    filters_wrapper.selected_index = None

    return BaseGUIElement(is_normalization=is_normalization,
                          low_pass_filter=low_pass_filter,
                          minimum_level=minimum_level,
                          maximum_level=maximum_level,
                          combiner_level=combiner_level,
                          combiner_dropdown=combiner_dropdown,
                          combiner_options=combiner_options,                          
                          low_pass_filter_options=low_pass_filter_options,
                          main=filters_wrapper,
                          _callbacks={'is_normalization': trigger_display,
                                      'low_pass_filter': trigger_display,
                                      'combiner_dropdown': change_combiner_options})
