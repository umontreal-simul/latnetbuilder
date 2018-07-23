import ipywidgets as widgets

from .common import style_default, parse_polynomial, BaseGUIElement

constr_data = {
    'sobol': '<p> The nets follow the Sobol construction, and the direction numbers are optimized. The first matrix is always the same (reversed identity), so the first direction number is arbitrarly set to 0.</p>',
    'polynomial': '<p> Polynomial Lattice Rules viewed as digital nets, and evaluated as such. </p>',
    'explicit': '<p> Most general construction of digital nets, defined by their generating matrices. </p>'
}

def change_constr_choice(change, gui):
    if change['name'] != 'value':
        return
    new_choice = change['new']
    gui.construction_method.constr_info.value = constr_data[new_choice]

    if new_choice == 'explicit':
        gui.exploration_method.exploration_choice.options = [
            ('Evaluate', 'evaluation'), ('All space', 'exhaustive')]
        gui.exploration_method.exploration_choice.value = 'exhaustive'
        gui.exploration_method.is_random.value = True
        gui.exploration_method.is_random.disabled = True
    else:
        old_value = gui.exploration_method.exploration_choice.value
        gui.exploration_method.exploration_choice.options = [
            ('Evaluate', 'evaluation'), ('All space', 'exhaustive'), ('CBC', 'full-CBC')]
        if change['old'] == 'explicit':
            gui.exploration_method.exploration_choice.value = old_value
        gui.exploration_method.is_random.disabled = False

    if new_choice == 'polynomial':
        gui.properties.modulus.placeholder = 'e.g. 2^10 (default) or 01^10 or 00000000001'
    else:
        gui.properties.modulus.placeholder = 'e.g. 2^10 (default) or 1024'

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

    constr_info = widgets.HTMLMath(
        value=constr_data[construction_choice.value])
    construction = widgets.Accordion([widgets.VBox(
        [construction_choice, constr_info])],
        layout=widgets.Layout(display="none"))
    construction.set_title(0, 'Construction Method')
    return BaseGUIElement(constr_info=constr_info,
                          construction_choice=construction_choice,
                          main=construction,
                          _callbacks={'construction_choice': change_constr_choice})
