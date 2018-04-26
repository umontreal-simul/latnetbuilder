import ipywidgets as widgets

from .common import style_default

constr_data = [
    '<p> A given generating vector \\(a = (a_1, ..., a_s)\\) is specified. </p>\
    <p> Two possibilities are given: </p> \
    <ul>\
    <li> simply evaluate the figure of merit for the lattice defined by this generating vector </li>\
    <li> extend the lattice defined by this generating vector and a given number of points </li>\
    </ul>',
    '<p> All generating vectors are examined and the best one is selected. </p>\
    <p> If you tick "random choice \\(r\\) of points", a number \\(r\\) of randomly selected generating vectors \\(a\\) will be examined instead of all vectors. </p>',
    '<p> All generating vectors of the form \\(a=(1,a \\text{ mod } n,a^2 \\text{ mod } n,…,a^s \\text{ mod } n)\\) are examined and the best one is selected. </p>\
    <p> If you tick "random choice \\(r\\) of points a number \\(r\\) of randomly selected generating vectors \\(a\\) will be examined instead of all Korobov vectors. </p>',
    '<p> All possible values of the components \\(a_j\\) of the generating vector \\(a=(a_1,…,a_s)\\) are examined the best ones are selected, one coordinate at a time. </p>\
    <p> If you tick "random choice \\(r\\) of points", a number \\(r\\) of randomly selected selected values for each component \\(a_j\\) will be examined instead of all possibilities.</p>',
    '<p> All possible values of the components components \\(a_j\\) of the generating vector \\(a=(a_1,…,a_s)\\) are examined and the best ones are selected, one coordinate at a time. </p>\
    <p> Computation is accelerated by using fast Fourier transforms. </p>\
    <p style="color:red"> Requires the Coordinate-Uniform evaluation method.</p>'
]

construction_choice = widgets.ToggleButtons(
    value=None,
    options=[('Evaluate/Extend', 'explicit:'), ('All space', 'exhaustive'),
             ('Korobov', 'Korobov'), ('CBC', 'CBC'), ('Fast CBC', 'fast-CBC')],
    description='Choose one:',
    style=style_default
)

is_random = widgets.Checkbox(
    description=r'Random choice of \(r\) points', value=False)
number_samples = widgets.HBox(
    [is_random, widgets.Text(value='10', description=r'Set \(r\):')], layout=widgets.Layout(display='none'), style=style_default)

generating_vector = widgets.VBox([
    widgets.HBox(
        [widgets.Label("Generating vector:")] +
        [widgets.Text(value='1', description='', layout=widgets.Layout(width='10%'), style=style_default) for k in range(3)]),  ## 3 = initial dim
    widgets.Text(placeholder='e.g. 2^8 or 256', description='If you want to extend, please specify the former modulus:',
                 layout=widgets.Layout(width='65%'), style=style_default)
],
    layout=widgets.Layout(display='none'))

constr_info = widgets.HTMLMath('')
construction = widgets.Accordion([widgets.VBox(
    [construction_choice, constr_info, generating_vector, number_samples])])
construction.set_title(0, 'Construction Method')

def change_constr_choice(b, gui):
    if b['name'] != 'index':
        return
    new_choice = b['new']
    gui.construction_method.constr_info.value = ' '.join(constr_data[new_choice].split('&\\'))
    if new_choice == 0:
        gui.construction_method.generating_vector.layout.display = 'flex'
        gui.construction_method.number_samples.layout.display = 'none'
    elif new_choice in [1, 2, 3]:
        gui.construction_method.number_samples.layout.display = 'flex'
        gui.construction_method.generating_vector.layout.display = 'none'
    else:
        gui.construction_method.number_samples.layout.display = 'none'
        gui.construction_method.generating_vector.layout.display = 'none'