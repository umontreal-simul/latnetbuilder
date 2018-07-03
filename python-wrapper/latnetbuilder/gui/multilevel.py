import ipywidgets as widgets

from .common import style_default, BaseGUIElement, trigger_display

def change_combiner_options(change, gui):
    # print(change)
    if change['name'] == 'value':
        if change['new'] == 'level:':
            gui.multi_level.combiner_level.layout.display = 'flex'
        else:
            gui.multi_level.combiner_level.layout.display = 'none'

def multilevel():
    mult_normalization = widgets.Checkbox(
        description='Normalization', value=False, style=style_default)
    mult_low_pass_filter = widgets.Checkbox(
        description='Low-Pass Filter', value=False, style=style_default)
    mult_combiner = widgets.Checkbox(
        description='Combiner', value=False, style=style_default)

    multilevel_wrapper = widgets.HBox(
        [mult_normalization, mult_low_pass_filter, mult_combiner])

    # Definition of mult is_normalization options VBox
    minimum_level = widgets.Text(placeholder='1', description='Min Level (optional):',
                                layout=widgets.Layout(width='22%', margin='0px 10px 0px 0px'), style=style_default)

    maximum_level = widgets.Text(placeholder='10', description='Max Level (optional):',
                                layout=widgets.Layout(width='22%'), style=style_default)

    mult_normalization_options = widgets.VBox([
        widgets.HBox([widgets.Dropdown(options=['SL10 Palpha', 'DPW08 Palpha'], description='Normalization Type:', layout=widgets.Layout(width='30%'), style=style_default),
                    widgets.HTML('<span style="color:red"> Warning: SL10 Palpha and DPW08 Palpha only work with Palpha figure of merit.</span>\
                                        <br><span style="color:red"> DPW08 only works for product weights.</span>')]),
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
        value='sum', description='Combiner:', style=style_default)
    combiner_options = widgets.HBox(
        [combiner_dropdown, combiner_level], layout=widgets.Layout(display='none'))

    # Wrapping
    embedding = widgets.Accordion([widgets.VBox([multilevel_wrapper, mult_normalization_options, mult_low_pass_filter_options, combiner_options])],
                                layout=widgets.Layout(display='none'))
    embedding.set_title(
        0, 'For Multilevel Point Sets: Multilevel Filters and Combiners')

    return BaseGUIElement(mult_normalization=mult_normalization,
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
