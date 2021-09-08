import ipywidgets as widgets

# style_default tells the navigator not to crop the description of the ipywidgets if it's too long. 
from .common import style_default, BaseGUIElement

type_data = {
    'ordinary': 'Fully projection-regular rank-1 Ordinary Lattice Rules',
    'polynomial': 'Polynomial Lattice Rules viewed as fully projection-regular rank-1 lattices, and evaluated as such. '
}

def change_lattice_type(change, gui):
    # depending on the new value of the lattice type, we update the GUI
    if change['name'] != 'value':
        return
    new_choice = change['new']
    gui.lattice_type.type_info.value = type_data[new_choice]
    gui.properties.polynomial_modulus.value = ''

    if new_choice == 'ordinary':
        gui.properties.interlacing.disabled = True
        gui.properties.interlacing.value = 1
        gui.properties.size.placeholder = 'e.g. 2^10 or 1024'
        gui.properties.polynomial_modulus.layout.display = 'none'
        gui.exploration_method.generating_vector.children[1].placeholder = 'e.g. 2^8 or 256'
        gui.figure_of_merit.figure_type.options = [('Palpha', 'Palpha'), ('Ralpha', 'Ralpha'), ('Spectral', 'spectral')]

    elif new_choice == 'polynomial':
        gui.properties.interlacing.disabled = False
        gui.properties.size.placeholder = 'e.g. 2^10' # 'e.g. 2^1 (using default polynomial) or 1024 (custom polynomial)'
        gui.properties.polynomial_modulus.layout.display = 'flex'
        gui.exploration_method.generating_vector.children[1].placeholder = 'e.g. 256'
        if gui.properties.interlacing.value == 1:
            gui.figure_of_merit.figure_type.options = [('Palpha', 'Palpha'), ('R', 'R')]
        else:
            gui.figure_of_merit.figure_type.options = [('Balpha,d_1', 'IAalpha') , ('Bd_2', 'IB'), ('Balpha,d_3', 'ICalpha')]


def lattice_type():
    # first create all the individual widgets, and initialize them with default values.
    # The initialization part is important, and the initialization of all widgets must be consistent. 

    type_choice = widgets.ToggleButtons(
        # options are (label, value) pairs
        value='ordinary',
        options=[('Ordinary', 'ordinary'), ('Polynomial', 'polynomial')],
        description='Choose one:',
        style=style_default
    )
    type_info = widgets.HTMLMath(value=type_data[type_choice.value])

    # then wrap the widgets inside containers: the containters are nested, because this allows for a compact
    # visualization for the user. For information about containers and their layout, see:
    # https://ipywidgets.readthedocs.io/en/stable/examples/Widget%20Styling.html#The-Flexbox-layout
    # The outmost container is always an Accordion.
    lattice_type = widgets.Accordion([widgets.VBox(
        [type_choice, type_info])])
    lattice_type.set_title(0, 'Lattice Type')

    # Instanciate and return the BaseGUIElement which encapsulates all the widgets created above.
    # Add each widget as an attribute of the Element. 
    # We almost always use the same name for the widget (local name in the function) and the attribute (global name that will be accessible everywhere)
    # To register callbacks, use the _callbacks dictionary where the key is the widget launching the callback and
    # the value is the callback function itself.
    # See the BaseGUIElement class for more information.
    return BaseGUIElement(type_info=type_info,
                                    main=lattice_type,
                                    type_choice=type_choice,
                                    _callbacks={'type_choice': change_lattice_type})