import ipywidgets as widgets

from .common import style_default, BaseGUIElement

def change_low_pass_filter(change, gui):
    if change['name'] != 'value':
        return
    if change['new'] == True:
        gui.figure_of_merit.low_pass_filter_options.layout.display = 'flex'
    else:
        gui.figure_of_merit.low_pass_filter_options.layout.display = 'none'

def change_normalization(change, gui):
    if change['name'] != 'value':
        return
    if change['new'] == True and gui.properties.is_multilevel.value and gui.figure_of_merit.combiner_dropdown.value in ['sum', 'max']:
        gui.figure_of_merit.minimum_level.layout.display = 'flex'
        gui.figure_of_merit.maximum_level.layout.display = 'flex'
    elif change['new'] == False:
        gui.figure_of_merit.minimum_level.layout.display = 'none'
        gui.figure_of_merit.maximum_level.layout.display = 'none'                 

def change_combiner_options(change, gui):
    if change['name'] == 'value':
        if change['new'] == 'level:':
            gui.figure_of_merit.combiner_level.layout.display = 'flex'
        else:
            gui.figure_of_merit.combiner_level.layout.display = 'none'
        if change['new'] in ['level:', 'level:max']:
            gui.figure_of_merit.minimum_level.layout.display = 'none'
            gui.figure_of_merit.maximum_level.layout.display = 'none'
        elif gui.figure_of_merit.is_normalization.value:
            gui.figure_of_merit.minimum_level.layout.display = 'flex'
            gui.figure_of_merit.maximum_level.layout.display = 'flex'

def change_figure_type(change, gui):
    if change['name'] != 'value':
        return
    if change['new'] in ['spectral', 't-value', 'projdep:resolution-gap', 'projdep:t-value', 'projdep:t-value:starDisc']:
        gui.figure_of_merit.coord_unif.value = False
        gui.figure_of_merit.coord_unif.disabled = True
    elif gui.main_tab.selected_index == 0:
            gui.figure_of_merit.coord_unif.disabled = False
    elif gui.main_tab.selected_index == 1:
        gui.figure_of_merit.coord_unif.value = True

    if change['new'] in ['spectral', 'R', 't-value', 'projdep:resolution-gap', 'projdep:t-value', 'projdep:t-value:starDisc', 'IB']:
        gui.figure_of_merit.figure_alpha.layout.display = 'none'
    elif change['new'] in ['Ralpha', 'Palpha', 'IAalpha', 'ICalpha']:
        gui.figure_of_merit.figure_alpha.layout.display = 'flex'

    if change['new'] in ['IAalpha', 'IB', 'ICalpha']:
        gui.figure_of_merit.norm_type.disabled = True
        gui.figure_of_merit.norm_type.value = '1'
    elif change['new'] in ['projdep:resolution-gap', 'projdep:t-value', 'projdep:t-value:starDisc', 'spectral']:
        gui.figure_of_merit.norm_type.disabled = False
        gui.figure_of_merit.norm_type.value = 'inf'    
    else:
        change_coord_unif({'new': gui.figure_of_merit.coord_unif.value}, gui)

    if change['new'] == 't-value':
        gui.weights.main.selected_index = None
        gui.figure_of_merit.norm_type.layout.display = 'none'
        gui.figure_of_merit.coord_unif.layout.display = 'none'
    else:
        gui.weights.main.selected_index = 0
        gui.figure_of_merit.norm_type.layout.display = 'flex'
        gui.figure_of_merit.coord_unif.layout.display = 'flex'


def change_coord_unif(change, gui):
    if ('name' in change and change['name'] != 'value') or gui.properties.interlacing.value > 1:
        return
    if change['new'] == True:
        gui.figure_of_merit.norm_type.disabled = True
        gui.figure_of_merit.norm_type.value = '2'
    elif change['new'] == False:
        gui.figure_of_merit.norm_type.disabled = False


def figure_of_merit():
    figure_type = widgets.Dropdown(
        options=[('Palpha', 'Palpha'), ('Ralpha', 'Ralpha'), ('Spectral', 'spectral')],
        value='Palpha', description='Figure:',
        layout=widgets.Layout(width='180px'), style=style_default)

    figure_alpha = widgets.Text(value='2', description='Value of \\(\\alpha\\):',
                        layout=widgets.Layout(width='130px'), style=style_default)

    coord_unif = widgets.Checkbox(value=True, description='Coordinate-Uniform evaluation',
                                        style=style_default)

    norm_type = widgets.Text(value='2', description='Norm type q > 0 or q = "inf":',
                            layout=widgets.Layout(width='220px'), style=style_default, disabled=True)

    figure_of_merit_expl = widgets.HTMLMath(value='<p> Please note (and see the documentation for more details) that: </p> \
                        <ul>\
                            <li> The list of available figures of merit depend on the point set type and the interlacing factor. </li>\
                            <li> The \\(P_\\alpha \\) method requires \\( \\alpha \\in \\{ 2, 4, 6 \\} \\), \\(R_\\alpha \\) requires \\( \\alpha > 0 \\) and \\(B_{d, \\alpha, (1)} \\) requires \\( \\alpha > 0 \\). </li> \
                            <li> The Coordinate-Uniform (CU) evaluation method is generally faster, but requires a specific norm type.<br>Depending on the point set type and the figure, this option is mandatory, optional or impossible.</li> \
                            <li> The possibility to choose the norm type depends on the point set type, the figure, and the CU option.</li>\
                            <li> For the t-value figure, weights, norm type and CU evaluation do not make sense.</li>\
                        </ul>')

    is_normalization = widgets.Checkbox(description='Normalization', value=False, style=style_default)
    minimum_level = widgets.Text(placeholder='1', description='Min Level (optional):',
                                layout=widgets.Layout(width='180px', margin='0px 10px 0px 0px', display='none'), style=style_default)
    maximum_level = widgets.Text(placeholder='10', description='Max Level (optional):',
                                layout=widgets.Layout(width='180px', display='none'), style=style_default)

    combiner_level = widgets.BoundedIntText(
        description='Level:', min=1, layout=widgets.Layout(display='none', width='130px'))
    combiner_dropdown = widgets.Dropdown(
        options=[('weighted sum', 'sum'),
                ('maximum weighted value', 'max'),
                ('weighted value of the highest level', 'level:max'),
                ('weighted value of a specific level', 'level:')],
        value='sum', description='Combiner:', style=style_default)
    combiner_options = widgets.HBox(
        [combiner_dropdown, combiner_level], layout=widgets.Layout(display='none'))    
    
    low_pass_filter = widgets.Checkbox(description='Low-Pass Filter', value=False, style=style_default)
    low_pass_filter_options = widgets.Text(value='1', description='Low-Pass Threshold',
                                        layout=widgets.Layout(display='none', width='200px'), style=style_default)

    figure_of_merit_wrapper = widgets.Accordion([
        widgets.VBox([
            figure_of_merit_expl,
            widgets.HBox([figure_type, figure_alpha, coord_unif, norm_type],
                        layout=widgets.Layout(justify_content='space-between')
                        ),
            combiner_options,
            widgets.HBox([is_normalization, minimum_level, maximum_level]),
            widgets.HBox([low_pass_filter, low_pass_filter_options])])
    ])

    figure_of_merit_wrapper.set_title(0, 'Figure of Merit')

    return BaseGUIElement(figure_type=figure_type,
                          figure_alpha=figure_alpha,
                          coord_unif=coord_unif,
                          norm_type=norm_type,
                          is_normalization=is_normalization,
                          low_pass_filter=low_pass_filter,
                          minimum_level=minimum_level,
                          maximum_level=maximum_level,
                          combiner_level=combiner_level,
                          combiner_dropdown=combiner_dropdown,
                          combiner_options=combiner_options,                          
                          low_pass_filter_options=low_pass_filter_options,
                          main=figure_of_merit_wrapper,
                          _callbacks={'figure_type': change_figure_type,
                                      'coord_unif': change_coord_unif,
                                      'low_pass_filter': change_low_pass_filter,
                                      'combiner_dropdown': change_combiner_options,
                                      'is_normalization': change_normalization})
