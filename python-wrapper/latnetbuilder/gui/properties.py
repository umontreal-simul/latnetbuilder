import ipywidgets as widgets
import numpy as np

from .common import style_default, parse_polynomial, INITIAL_DIM, BaseGUIElement

def change_modulus(change, gui):
    if change['name'] != 'value':
        return
    try:
        poly_str = parse_polynomial(change['new'])
        if len(poly_str) > 0 and \
        ((gui.main_tab.selected_index == 0 and gui.lattice_type.type_choice.value == 'polynomial') or (gui.main_tab.selected_index == 1 and gui.construction_method.construction_choice.value == 'polynomial')):
            gui.properties.modulus_pretty.layout.display = 'flex'
            gui.properties.modulus_pretty.value = '\\(' + poly_str + '\\)'
        else:
            gui.properties.modulus_pretty.layout.display = 'none'
    except:
        gui.properties.modulus_pretty.layout.display = 'none'
        return

def change_multilevel(change, gui):
    if change['name'] != 'value':
        return
    if change['new'] == True:
        gui.figure_of_merit.combiner_options.layout.display = 'flex'
        if gui.figure_of_merit.is_normalization.value and gui.figure_of_merit.combiner_dropdown.value in ['sum', 'max']:
            gui.figure_of_merit.minimum_level.layout.display = 'flex'
            gui.figure_of_merit.maximum_level.layout.display = 'flex'
    else:
        gui.figure_of_merit.combiner_options.layout.display = 'none'
        gui.figure_of_merit.minimum_level.layout.display = 'none'
        gui.figure_of_merit.maximum_level.layout.display = 'none'        

def change_interlacing(change, gui):
    if change['name'] != 'value':
        return
    if change['new'] == 1:
        if gui.main_tab.selected_index == 0:
            gui.figure_of_merit.figure_type.options = ['Palpha', 'R']
            gui.figure_of_merit.figure_type.value = 'Palpha'
        else:
            gui.figure_of_merit.figure_type.options = [('Palpha', 'Palpha'), ('R', 'R'), ('t-value', 't-value'), ('proj dep t-value', 'projdep:t-value'), ('star discr t-value', 'projdep:t-value:starDisc'),  ('resolution-gap', 'projdep:resolution-gap')]
            gui.figure_of_merit.figure_type.value = 'Palpha'
    else:
        gui.figure_of_merit.figure_type.options = [('Balpha,d_1', 'IAalpha'), ('Bd_2', 'IB'), ('Balpha,d_3', 'ICalpha')]    
        gui.figure_of_merit.figure_type.value = 'IAalpha'
    update(gui.exploration_method.generating_vector.children[0], change['new']*gui.properties.dimension.value, '1', '100px')
    update(gui.exploration_method.generating_vector_simple, change['new']*gui.properties.dimension.value, '1', '100px')

def update(form, dim, defaut_value, width):
    new_children = []
    if len(form.children) < dim:
        for k in range(len(form.children)):
            new_children.append(form.children[k])
        for k in range(dim-len(form.children)):
            new_children.append(widgets.Text(
                value=defaut_value, description='', layout=widgets.Layout(width=width)))
    elif len(form.children) > dim:
        for k in range(dim):
            new_children.append(form.children[k])
    form.children = new_children

# callback for dimension change
def change_dimension(change, gui):
    if change['name'] != 'value':
        return
    dim = change['new']
    VBOX_of_weights = gui.weights.VBOX_of_weights
    for k in range(len(VBOX_of_weights.children)):
        weight = VBOX_of_weights.children[k]
        weight_type = weight.children[0].children[0].value.split(' ')[1]
        if weight_type == 'Product':
            form = weight.children[1].children[0].children[1]
            update(form, dim, '0.8', '50px')
        elif weight_type == 'Order-Dependent':
            new_value = str(round(0.8**dim * (dim <= 3), 3))
            form = weight.children[1].children[0].children[1]
            update(form, dim, new_value, '50px')
        elif weight_type == 'POD':
            form = weight.children[1].children[0].children[1]
            update(form, dim, '0.8', '50px')
            new_value = str(round(0.8**dim * (dim <= 3), 3))
            form = weight.children[2].children[0].children[1]
            update(form, dim, new_value, '50px')
    gui.exploration_method.mixed_CBC_level.max = dim

    dim = change['new'] * gui.properties.interlacing.value
    update(gui.exploration_method.generating_vector.children[0], dim, '1', '100px')
    update(gui.exploration_method.generating_vector_simple, dim, '1', '100px')


def properties():
    modulus = widgets.Text(placeholder='e.g. 2^10 (default) or 1024', description='Modulus n=', 
                    style=style_default, layout=widgets.Layout(width='420px'))

    is_multilevel = widgets.Checkbox(value=False, description='Multilevel')

    dimension = widgets.BoundedIntText(value=INITIAL_DIM, min=1, description='Dimension s:',
                                       style=style_default, layout=widgets.Layout(width='160px'))
    interlacing = widgets.BoundedIntText(value=1, min=1, description='Interlacing d:',
                                       style=style_default, layout=widgets.Layout(width='160px'), disabled=True)

    modulus_pretty = widgets.Label('', layout=widgets.Layout(display='none'))

    properties_wrapper = widgets.Accordion(
        [widgets.HBox(
            [widgets.VBox([modulus, modulus_pretty], layout=widgets.Layout(width='460px')), 
            is_multilevel, 
            widgets.VBox([dimension, interlacing], layout=widgets.Layout(width='180px'))],
            layout=widgets.Layout(align_items='center')
        )])
    properties_wrapper.set_title(0, 'Basic Lattice properties')
    return BaseGUIElement(modulus=modulus,
                          is_multilevel=is_multilevel,
                          dimension=dimension,
                          interlacing=interlacing,
                          main=properties_wrapper,
                          modulus_pretty=modulus_pretty,
                          _callbacks={'modulus': change_modulus,
                                      'dimension': change_dimension,
                                      'interlacing': change_interlacing,
                                      'is_multilevel': change_multilevel})
