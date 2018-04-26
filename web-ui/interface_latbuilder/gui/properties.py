import ipywidgets as widgets
import numpy as np

from .common import style_default

size = widgets.Text(placeholder='e.g. 2^10 or 1024', description='Modulus n=',
                    layout=widgets.Layout(width='30%'), style=style_default)

lattice_type = widgets.Checkbox(value=False, description='Embedded')

dimension = widgets.BoundedIntText(value=3, min=1, description='Dimension s:',
                                   layout=widgets.Layout(width='20%'), style=style_default)

properties_wrapper = widgets.Accordion([widgets.HBox([size, lattice_type, dimension])])
properties_wrapper.set_title(0, 'Basic Lattice properties')

# callback for size change
def change_modulus(b, gui):
    if b['name'] != 'value' or gui.main_tab.selected_index != 1:
        return

    poly_str = ''
    try:
        p = b['new'].split('^')
        if len(p) == 2:
            base = np.array([int(y) for y in p[0]])
            power = int(p[1])
            modulus = ((np.poly1d(np.flip(base, axis=0)))**power).c % 2
        else:
            base = np.array([int(y) for y in b['new']])
            modulus = np.poly1d(np.flip(base, axis=0)).c
        poly_str = ' '
        for k in range(len(modulus)):
            if modulus[k] == 1:
                if poly_str != ' ':
                    poly_str += '+'
                poly_str += ' z^' + str(len(modulus)-k-1)
    except:
        pass
    if poly_str == ' ':
        poly_str = '0'
    gui.properties.size.description = "Mod n=" + poly_str + '='
    size_width = str(10 + len(gui.properties.size.description)) + '%'
    gui.properties.size.layout = widgets.Layout(width=size_width)


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
