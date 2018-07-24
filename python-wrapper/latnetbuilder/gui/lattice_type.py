import ipywidgets as widgets

from .common import style_default, BaseGUIElement

type_data = {
    'ordinary': 'Fully projection-regular rank-1 Ordinary Lattice Rules',
    'polynomial': 'Polynomial Lattice Rules viewed as fully projection-regular rank-1 lattices, and evaluated as such. '
}

def change_lattice_type(change, gui):
    if change['name'] != 'value':
        return
    new_choice = change['new']
    gui.lattice_type.type_info.value = type_data[new_choice]

    if new_choice == 'ordinary':
        gui.properties.interlacing.disabled = True
        gui.properties.interlacing.value = 1
        gui.properties.modulus.placeholder = 'e.g. 2^10 (default) or 1024'
        gui.exploration_method.generating_vector.children[1].placeholder = 'e.g. 2^8 or 256'
        gui.figure_of_merit.figure_type.options = [('Palpha', 'Palpha'), ('Ralpha', 'Ralpha'), ('Spectral', 'spectral')]

    elif new_choice == 'polynomial':
        gui.properties.interlacing.disabled = False
        gui.properties.modulus.placeholder = 'e.g. 2^10 (default) or 01^10 or 00000000001'
        gui.exploration_method.generating_vector.children[1].placeholder = 'e.g. 01^6 or 0000001'
        if gui.properties.interlacing.value == 1:
            gui.figure_of_merit.figure_type.options = [('Palpha', 'Palpha'), ('R', 'R')]
        else:
            gui.figure_of_merit.figure_type.options = [('Balpha,d_1', 'IAalpha') , ('Bd_2', 'IB')]


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