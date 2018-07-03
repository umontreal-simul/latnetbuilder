import ipywidgets as widgets

from .common import style_default, BaseGUIElement

type_data = {
    'ordinary': 'TBC',
    'polynomial': 'TBC'
}

def change_lattice_type(change, gui):
    if change['name'] != 'value':
        return
    new_choice = change['new']
    gui.lattice_type.type_info.value = type_data[new_choice]

    if new_choice == 'ordinary':
        gui.properties.modulus_pretty.layout.display = 'none'
        gui.properties.modulus.placeholder = 'e.g. 2^10 or 1024'
        gui.properties.modulus.description = 'Modulus n='
        gui.exploration_method.generating_vector.children[1].placeholder = 'e.g. 2^8 or 256'
        gui.figure_of_merit.figure_type.options = ['Palpha', 'Ralpha', 'Spectral']
        gui.figure_of_merit.figure_of_merit_expl.value = '<p> Please note that: </p> \
            <ul>\
            <li> The Coordinate-Uniform method is generally faster, but requires \\(q = 2\\) </li> \
            <li> The Spectral figure cannot be be evaluated by the Coordinate-Uniform method </li> \
            <li> The \\(P_\\alpha \\) figure requires \\( \\alpha \\in \\{ 2, 4, 6 \\} \\) </li> \
            </ul>'

    elif new_choice == 'polynomial':
        gui.properties.modulus_pretty.layout.display = 'flex'
        gui.properties.modulus.placeholder = 'e.g. 01^8 or 000000001'
        gui.properties.modulus.description = 'Modulus n='
        gui.exploration_method.generating_vector.children[1].placeholder = 'e.g. 01^6 or 0000001'
        gui.figure_of_merit.figure_type.options = ['Palpha', 'R']
        gui.figure_of_merit.figure_of_merit_expl.value = '<p> Please note that: </p> \
            <ul>\
            <li> The Coordinate-Uniform method is generally faster, but requires \\(q = 2\\) </li> \
            <li> The \\(P_\\alpha \\) figure requires \\( \\alpha \\in \\{ 2, 4, 6 \\} \\) </li> \
            </ul>'

def lattice_type():
    type_choice = widgets.ToggleButtons(
        value='ordinary',
        options=[('Ordinary', 'ordinary'), ('Polynomial', 'polynomial')],
        description='Choose one:',
        style=style_default
    )

    type_info = widgets.HTMLMath(value=type_data[type_choice.value])
    lattice_type = widgets.Accordion([widgets.VBox(
        [type_choice, type_info])])
    lattice_type.set_title(0, 'Lattice Type')

    return BaseGUIElement(type_info=type_info,
                                    main=lattice_type,
                                    type_choice=type_choice,
                                    _callbacks={'type_choice': change_lattice_type})