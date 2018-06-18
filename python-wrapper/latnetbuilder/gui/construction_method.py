import ipywidgets as widgets

from .common import style_default, parse_polynomial, BaseGUIElement

constr_data = {
    'sobol': '<p> The nets follow the Sobol construction, and the software optimizes the direction numbers according to the figure of merit. The first matrix is always the same (reversed identity), so the first direction number is arbitrarly set to 0.</p>\
    <p style="color:red"> Warning: Sobol construction associated with CBC exploration method and a Kernel figure of merit (\\(P_\\alpha\\) or R) does not make sense. </p>',
    'polynomial': '<p> Polynomial Lattice Rules viewed as digital nets, and evaluated as such. </p>\
    <p style="color:red"> The modulus below must be coherent with the number of points entered above.</p>',
    'explicit': '<p> Explicit construction method: this is used to evaluate a net based on explicit matrices you can provide below. </p>'
}


def change_constr_modulus(b, gui):
    if b['name'] != 'value':
        return
    try:
        poly_str = parse_polynomial(b['new'])
        gui.construction_method.construction_modulus_pretty.value = '\\(' + \
            poly_str + '\\)'
    except:
        return


def change_constr_choice(b, gui):
    if b['name'] != 'value':
        return
    new_choice = b['new']
    gui.construction_method.constr_info.value = constr_data[new_choice]

    if new_choice == 'polynomial':
        gui.construction_method.construction_modulus_box.layout.display = 'flex'
    else:
        gui.construction_method.construction_modulus_box.layout.display = 'none'

    if new_choice == 'explicit':
        gui.exploration_method.exploration_choice.options = [
            ('Evaluate', 'net-explicit:')]
    else:
        gui.exploration_method.exploration_choice.options = [
            ('Evaluate', 'net-explicit:'), ('All space', 'exhaustive'), ('CBC', 'full-CBC')]

    gui.exploration_method._callbacks[gui.exploration_method.exploration_choice](
        {'name': 'value', 'new': gui.exploration_method.exploration_choice.value}, gui
    )


def construction_method():
    construction_choice = widgets.ToggleButtons(
        value='sobol',
        options=[('Sobol', 'sobol'), ('Polynomial', 'polynomial'),
                 ('Explicit', 'explicit')],
        description='Choose one:',
        style=style_default
    )

    construction_modulus = widgets.Text(placeholder='e.g. 01^10 or 00000000001', description='Modulus n=',
                                        style=style_default, layout=widgets.Layout(width='400px'))
    construction_modulus_pretty = widgets.Label('')
    construction_modulus_box = widgets.VBox(
        [construction_modulus, construction_modulus_pretty], layout=widgets.Layout(display='none'))

    constr_info = widgets.HTMLMath(
        value=constr_data[construction_choice.value])
    construction = widgets.Accordion([widgets.VBox(
        [construction_choice, constr_info, construction_modulus_box])],
        layout=widgets.Layout(display="none"))
    construction.set_title(0, 'Construction Method')
    return BaseGUIElement(constr_info=constr_info,
                          construction_choice=construction_choice,
                          construction_modulus=construction_modulus,
                          construction_modulus_pretty=construction_modulus_pretty,
                          construction_modulus_box=construction_modulus_box,
                          main=construction,
                          _callbacks={'construction_choice': change_constr_choice,
                                      'construction_modulus': change_constr_modulus})
