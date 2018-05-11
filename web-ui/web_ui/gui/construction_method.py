import ipywidgets as widgets

from .common import style_default, parse_polynomial

constr_data = {
    'sobol': '<p> Sobol construction method. TBC </p>',
    'polynomial': '<p> Polynomial construction method. TBC </p>',
    'explicit': '<p> Explicit construction method. TBC </p>'
}

construction_choice = widgets.ToggleButtons(
    value='sobol',
    options=[('Sobol', 'sobol'), ('Polynomial', 'polynomial'), ('Explicit', 'explicit')],
    description='Choose one:',
    style=style_default
)

construction_modulus = widgets.Text(placeholder='e.g. 01^8 or 000000001', description='Modulus n=', 
    style=style_default, layout=widgets.Layout(width='50%'))
construction_modulus_pretty = widgets.Label('')
construction_modulus_box = widgets.VBox([construction_modulus, construction_modulus_pretty], layout=widgets.Layout(display='none'))

constr_info = widgets.HTMLMath('')
construction = widgets.Accordion([widgets.VBox(
    [construction_choice, constr_info, construction_modulus_box])], 
    layout = widgets.Layout(display="none"))
construction.set_title(0, 'Construction Method')

def change_constr_modulus(b, gui):
    if b['name'] != 'value':
        return
    try:
        poly_str = parse_polynomial(b['new'])
        gui.construction_method.construction_modulus_pretty.value = '\\(' + poly_str + '\\)'
    except:
        return

def change_constr_choice(b, gui):
    if b['name'] != 'value':
        return
    new_choice = b['new']
    gui.construction_method.constr_info.value = constr_data[new_choice]
    if gui.exploration_method.exploration_choice.value == 'net-explicit:':
        if new_choice == 'sobol':
            gui.exploration_method.generating_numbers_sobol_box.layout.display = 'flex'
            gui.exploration_method.generating_vector_simple.layout.display = 'none'
            gui.exploration_method.generating_matrices.layout.display = 'none'
        elif new_choice == 'polynomial':
            gui.exploration_method.generating_numbers_sobol_box.layout.display = 'none'
            gui.exploration_method.generating_vector_simple.layout.display = 'flex'
            gui.exploration_method.generating_matrices.layout.display = 'none'
        elif new_choice == 'explicit':
            gui.exploration_method.generating_numbers_sobol_box.layout.display = 'none'
            gui.exploration_method.generating_vector_simple.layout.display = 'none'
            gui.exploration_method.generating_matrices.layout.display = 'flex'
            

    if new_choice == 'polynomial':
        gui.construction_method.construction_modulus_box.layout.display = 'flex'
    else:
        gui.construction_method.construction_modulus_box.layout.display = 'none'

    if new_choice == 'explicit':
        gui.exploration_method.exploration_choice.options = [('Evaluate', 'net-explicit:')]
    else:
        gui.exploration_method.exploration_choice.options = [
            ('Evaluate', 'net-explicit:'), ('All space', 'exhaustive'), ('CBC', 'full-CBC')]