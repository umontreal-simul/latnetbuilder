import ipywidgets as widgets
import numpy as np

from .common import style_default, parse_polynomial, INITIAL_DIM, BaseGUIElement, trigger_display

def change_modulus(b, gui):
    if b['name'] != 'value' or gui.main_tab.selected_index != 0:
        return
    try:
        poly_str = parse_polynomial(b['new'])
        gui.properties.modulus_pretty.value = '\\(' + poly_str + '\\)'
    except:
        return

def update(form, dim, defaut_value):
    new_children = []
    if len(form.children) < dim:
        for k in range(len(form.children)):
            new_children.append(form.children[k])
        for k in range(dim-len(form.children)):
            new_children.append(widgets.Text(
                value=defaut_value, description='', layout=widgets.Layout(width='50px')))
    elif len(form.children) > dim:
        for k in range(dim):
            new_children.append(form.children[k])
    form.children = new_children

# callback for dimension change
def change_dimension(b, gui):
    if b['name'] != 'value':
        return
    dim = b['new']
    VBOX_of_weights = gui.weights.VBOX_of_weights
    for k in range(len(VBOX_of_weights.children)):
        weight = VBOX_of_weights.children[k]
        weight_type = weight.children[0].children[0].value.split(' ')[1]
        if weight_type == 'Product':
            form = weight.children[1].children[0].children[1]
            update(form, dim, '0.8')
        elif weight_type == 'Order-Dependent':
            new_value = str(round(0.8**dim * (dim <= 3), 3))
            form = weight.children[1].children[0].children[1]
            update(form, dim, new_value)
        elif weight_type == 'POD':
            form = weight.children[1].children[0].children[1]
            update(form, dim, '0.8')
            new_value = str(round(0.8**dim * (dim <= 3), 3))
            form = weight.children[2].children[0].children[1]
            update(form, dim, new_value)

    update(gui.exploration_method.generating_vector.children[0], dim+1, '1')
    update(gui.exploration_method.generating_vector_simple, dim+1, '1')
    gui.exploration_method.mixed_CBC_level.max = b['new']


def properties():
    modulus = widgets.Text(placeholder='e.g. 2^10 or 1024', description='Modulus n=', 
                    style=style_default, layout=widgets.Layout(width='95%'))

    is_multilevel = widgets.Checkbox(value=False, description='Embedded')

    dimension = widgets.BoundedIntText(value=INITIAL_DIM, min=1, description='Dimension s:',
                                    layout=widgets.Layout(width='20%'), style=style_default)

    modulus_pretty = widgets.Label('', layout=widgets.Layout(display='none'))

    properties_wrapper = widgets.Accordion(
        [widgets.HBox(
            [widgets.VBox([modulus, modulus_pretty], layout=widgets.Layout(width='50%')), 
            is_multilevel, 
            dimension])])
    properties_wrapper.set_title(0, 'Basic Lattice properties')
    return BaseGUIElement(modulus=modulus,
                          is_multilevel=is_multilevel,
                          dimension=dimension,
                          main=properties_wrapper,
                          modulus_pretty=modulus_pretty,
                          _callbacks={'modulus': change_modulus,
                                      'dimension': change_dimension,
                                      'is_multilevel': trigger_display})
