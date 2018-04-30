import ipywidgets as widgets
import numpy as np

from .common import style_default, parse_polynomial, INITIAL_DIM

modulus = widgets.Text(placeholder='e.g. 2^10 or 1024', description='Modulus n=', 
                    style=style_default, layout=widgets.Layout(width='95%'))

lattice_type = widgets.Checkbox(value=False, description='Embedded')

dimension = widgets.BoundedIntText(value=INITIAL_DIM, min=1, description='Dimension s:',
                                   layout=widgets.Layout(width='20%'), style=style_default)

modulus_pretty = widgets.Label('', layout=widgets.Layout(display='none'))

properties_wrapper = widgets.Accordion(
    [widgets.HBox(
        [widgets.VBox([modulus, modulus_pretty], layout=widgets.Layout(width='50%')), 
        lattice_type, 
        dimension])])
properties_wrapper.set_title(0, 'Basic Lattice properties')


def change_modulus(b, gui):
    if b['name'] != 'value' or gui.main_tab.selected_index != 1:
        return
    poly_str = parse_polynomial(b['new'])
    gui.properties.modulus_pretty.value = '\\(' + poly_str + '\\)'


def update(form, dim, defaut_value):
    new_children = []
    if len(form.children) < dim:
        for k in range(len(form.children)):
            new_children.append(form.children[k])
        for k in range(dim-len(form.children)):
            new_children.append(widgets.Text(
                value=defaut_value, description='', layout=widgets.Layout(width='10%')))
    elif len(form.children) > dim:
        for k in range(dim):
            new_children.append(form.children[k])
    form.children = new_children

# callback for dimension change
def change_dimension(b, gui):
    if b['name'] != 'value':
        return
    dim = b['new']
    VBOX_of_weights = gui.weights.main.children[0].children[2]
    for k in range(len(VBOX_of_weights.children)):
        try:
            form = VBOX_of_weights.children[k].children[1].children[0].children[1]
            update(form, dim, '0.1')
            form = VBOX_of_weights.children[k].children[1].children[2].children[1]
            update(form, dim, '0.1')
        except:
            continue
    update(gui.generating_vector.children[0], dim+1, '1')
