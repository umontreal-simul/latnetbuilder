import ipywidgets as widgets

from .common import style_default, INITIAL_DIM, JoeKuoSobolNets

explr_data = {
    'explicit:': '<p> A given generating vector \\(a = (a_1, ..., a_s)\\) is specified. </p>\
    <p> Two possibilities are given: </p> \
    <ul>\
    <li> simply evaluate the figure of merit for the lattice defined by this generating vector </li>\
    <li> extend the lattice defined by this generating vector and a given number of points </li>\
    </ul>',
    'exhaustive': '<p> All generating vectors are examined and the best one is selected. </p>\
    <p> If you tick "random choice \\(r\\) of points", a number \\(r\\) of randomly selected generating vectors \\(a\\) will be examined instead of all vectors. </p>',
    'Korobov': '<p> All generating vectors of the form \\(a=(1,a \\text{ mod } n,a^2 \\text{ mod } n,…,a^s \\text{ mod } n)\\) are examined and the best one is selected. </p>\
    <p> If you tick "random choice \\(r\\) of points a number \\(r\\) of randomly selected generating vectors \\(a\\) will be examined instead of all Korobov vectors. </p>',
    'CBC': '<p> All possible values of the components \\(a_j\\) of the generating vector \\(a=(a_1,…,a_s)\\) are examined the best ones are selected, one coordinate at a time. </p>\
    <p> If you tick "random choice \\(r\\) of points", a number \\(r\\) of randomly selected selected values for each component \\(a_j\\) will be examined instead of all possibilities.</p>',
    'fast-CBC': '<p> All possible values of the components components \\(a_j\\) of the generating vector \\(a=(a_1,…,a_s)\\) are examined and the best ones are selected, one coordinate at a time. </p>\
    <p> Computation is accelerated by using fast Fourier transforms. </p>\
    <p style="color:red"> Requires the Coordinate-Uniform evaluation method.</p>',
    'full-CBC': '<p> All possible values of the components \\(a_j\\) of the generating vector \\(a=(a_1,…,a_s)\\) are examined the best ones are selected, one coordinate at a time. </p>\
    <p> If you tick "random choice \\(r\\) of points", a number \\(r\\) of randomly selected selected values for each component \\(a_j\\) will be examined instead of all possibilities.\
    If the minimal random dimension \\(d\\) equals one, the search is entirely random. Else, the search is exhaustive up to dimension \\(d-1\\), and random from dimension \\(d\\). </p>',
    'net-explicit:' : '<p> Explicit net evaluation. Enter below the net characteristics:\
        <ul>\
            <li> direction numbers for Sobol construction (please respect the format given as example)\
            <li> generating vector for the polynomial construction \
            <li> generating matrices for the explicit construction (please respect the format given as example)\
        </ul>\
    </p>'
}

exploration_choice = widgets.ToggleButtons(
    value='CBC',
    options=[('Evaluate/Extend', 'explicit:'), ('All space', 'exhaustive'),
             ('Korobov', 'Korobov'), ('CBC', 'CBC'), ('Fast CBC', 'fast-CBC')],
    description='Choose one:',
    style=style_default
)

is_random = widgets.Checkbox(description=r'Random choice of \(r\) points', value=False)
number_samples = widgets.Text(value='10', description=r'Set \(r\):', layout=widgets.Layout(display='none'))

random_box = widgets.HBox([is_random, number_samples], style=style_default)


generating_numbers_sobol = widgets.Textarea(placeholder='0\n1\n1,1\n1,3,5',
                                   layout=widgets.Layout(width='inherit', height='150px'))
generating_numbers_sobol_button = widgets.Button(description='Joe and Kuo direction numbers', layout=widgets.Layout(width='270px'))
generating_numbers_sobol_box = widgets.VBox([generating_numbers_sobol_button, generating_numbers_sobol],
                                layout=widgets.Layout(display='none'))

generating_matrices = widgets.Textarea(placeholder='1 0 0\n0 1 0\n0 0 1\n\n1 1 0\n1 0 1\n0 1 1',
                                   layout=widgets.Layout(width='inherit', height='150px', display='none'))


generating_vector_simple = widgets.HBox(
        [widgets.Label("Generating vector:")] +
        [widgets.Text(value='1', description='', 
        layout=widgets.Layout(width='10%'), style=style_default) for k in range(INITIAL_DIM)],
        layout=widgets.Layout(display='none'))

generating_vector = widgets.VBox([
    widgets.HBox(
        [widgets.Label("Generating vector:")] +
        [widgets.Text(value='1', description='', 
        layout=widgets.Layout(width='10%'), style=style_default) for k in range(INITIAL_DIM)]),
    widgets.Text(placeholder='e.g. 2^8 or 256', description='If you want to extend, please specify the former modulus:',
                 layout=widgets.Layout(width='65%'), style=style_default)
],
    layout=widgets.Layout(display='none'))

mixed_CBC_level = widgets.BoundedIntText(value=1, min=1, description='Minimal random dimension:',
                    layout=widgets.Layout(width='250px', display='none', margin='0px 0px 0px 80px'), style=style_default, disabled=True)

explr_info = widgets.HTMLMath(explr_data[exploration_choice.value])
exploration = widgets.Accordion([widgets.VBox(
    [exploration_choice, explr_info, generating_vector_simple, generating_vector, generating_matrices, random_box, mixed_CBC_level, generating_numbers_sobol_box])])
exploration.set_title(0, 'Exploration Method')

def change_explr_choice(b, gui):
    if b['name'] != 'value':
        return
    new_choice = b['new']
    gui.exploration_method.explr_info.value = explr_data[new_choice]
    if new_choice == 'explicit:':     
        gui.exploration_method.generating_vector.layout.display = 'flex'
        gui.exploration_method.random_box.layout.display = 'none'
        gui.exploration_method.mixed_CBC_level.layout.display = 'none'
        gui.exploration_method.generating_numbers_sobol_box.layout.display = 'none'
        gui.exploration_method.generating_vector_simple.layout.display = 'none'
        gui.exploration_method.generating_matrices.layout.display = 'none'
    elif new_choice in ['exhaustive', 'Korobov', 'CBC']:
        gui.exploration_method.generating_vector.layout.display = 'none'
        gui.exploration_method.random_box.layout.display = 'flex'
        gui.exploration_method.mixed_CBC_level.layout.display = 'none'
        gui.exploration_method.generating_numbers_sobol_box.layout.display = 'none'
        gui.exploration_method.generating_vector_simple.layout.display = 'none'
        gui.exploration_method.generating_matrices.layout.display = 'none'
    elif new_choice == 'fast-CBC':
        gui.exploration_method.generating_vector.layout.display = 'none'
        gui.exploration_method.random_box.layout.display = 'none'
        gui.exploration_method.mixed_CBC_level.layout.display = 'none'
        gui.exploration_method.generating_numbers_sobol_box.layout.display = 'none'
        gui.exploration_method.generating_vector_simple.layout.display = 'none'
        gui.exploration_method.generating_matrices.layout.display = 'none'
    if new_choice == 'full-CBC':
        gui.exploration_method.generating_vector.layout.display = 'none'
        gui.exploration_method.random_box.layout.display = 'flex'
        gui.exploration_method.mixed_CBC_level.layout.display = 'flex'
        gui.exploration_method.generating_numbers_sobol_box.layout.display = 'none'
        gui.exploration_method.generating_vector_simple.layout.display = 'none'
        gui.exploration_method.generating_matrices.layout.display = 'none'
    elif new_choice == 'net-explicit:' :
        gui.exploration_method.random_box.layout.display = 'none'
        gui.exploration_method.mixed_CBC_level.layout.display = 'none'
        gui.exploration_method.generating_vector.layout.display = 'none'
        if gui.construction_method.construction_choice.value == 'sobol':
            gui.exploration_method.generating_numbers_sobol_box.layout.display = 'flex'
            gui.exploration_method.generating_vector_simple.layout.display = 'none'
            gui.exploration_method.generating_matrices.layout.display = 'none'
        elif gui.construction_method.construction_choice.value == 'polynomial':
            gui.exploration_method.generating_numbers_sobol_box.layout.display = 'none'
            gui.exploration_method.generating_vector_simple.layout.display = 'flex'
            gui.exploration_method.generating_matrices.layout.display = 'none'
        elif gui.construction_method.construction_choice.value == 'explicit':
            gui.exploration_method.generating_numbers_sobol_box.layout.display = 'none'
            gui.exploration_method.generating_vector_simple.layout.display = 'none'
            gui.exploration_method.generating_matrices.layout.display = 'flex'

def trigger_is_random(b, gui):
    if b['name'] != 'value':
        return
    new_choice = b['new']
    gui.exploration_method.mixed_CBC_level.disabled = not new_choice
    if new_choice:
        gui.exploration_method.number_samples.layout.display = 'flex'
    else:
        gui.exploration_method.number_samples.layout.display = 'none'

def automatic_generating_numbers_sobol(b, gui):
    gui.exploration_method.generating_numbers_sobol.value = '\n'.join(JoeKuoSobolNets[:gui.properties.dimension.value])