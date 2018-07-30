import ipywidgets as widgets
import numpy as np
import numexpr as ne

from .common import style_default, BaseGUIElement

weight_math = widgets.HTMLMath(value='The weights are of the form: \\(\\gamma_u^q (u \\subseteq \\{1, ... s\\})\\). \
You can specify below type of weights with their values (the actual weights are the sum of these).',
                               layout=widgets.Layout(width='inherit'))

math_strings = {'Order-Dependent': '\\( \\Gamma_k^q \\)', 
                'Product': '\\( \\gamma_j^q \\)'}
title_strings = {'Order-Dependent': 'Set Order Weights \\( \\Gamma_k^q \\):', 
                'Product': 'Set Coordinate Weights \\( \\gamma_j^q \\):'}
math_label = {'Order-Dependent': '$$\\gamma_u=\\Gamma_{|u|}$$', 
            'Product':'$$\\gamma_u=\\prod_{j\\in u} \\gamma_j$$'}
weights_index = {'Order-Dependent': 'k', 'Product': 'j'}
placeholder_set_all = {'Order-Dependent': '0.8^k * (k <= 3)', 'Product': '0.8'}



def remove_button_clicked(change, gui):
    weight_to_remove = gui.weights.weights_button_id[change.model_id]
    gui.weights.VBOX_of_weights.children = [weight for weight in gui.weights.VBOX_of_weights.children if weight != weight_to_remove]

def set_all_weights(change, nb, gui, type_weights):
    if change['name'] != 'value':
        return

    expr = change['new'].replace('^', '**')
    coord = np.arange(1, gui.properties.dimension.value+1)
    try:
        expr_evaluated = ne.evaluate(expr, local_dict={'j': coord, 'k': coord})
        valid = True
    except:
        valid = False

    weights = gui.weights.weights_set_all_id[change['owner']]
    form = weights.children[0].children[1]
    set_all = weights.children[1]
    if valid:
        for k in range(len(form.children)):
            try:
                form.children[k].value = str(expr_evaluated[k])
            except:
                form.children[k].value = str(expr_evaluated)
        set_all.description = 'OR set all at once with (valid Python) expression: %s =' % (math_strings[type_weights])
    else:
        set_all.description = 'NOT VALID, please change expression: %s =' % (math_strings[type_weights])

def create_elem_weights(type_weights, dimension_int, gui):
    weights_set_all_id = gui.weights.weights_set_all_id
    if type_weights == 'Product':
        value = ['0.8']*dimension_int
    elif type_weights == 'Order-Dependent':
        value = [str(round(0.8**(k+1) * (k <= 2), 3)) for k in range(dimension_int)]
    form = widgets.HBox([widgets.Text(value=value[k], description='', layout=widgets.Layout(width='50px')) 
                        for k in range(dimension_int)])
    set_all = widgets.Text(value='', placeholder=placeholder_set_all[type_weights],
                            description='OR set all at once with (valid Python) expression: %s =' %(math_strings[type_weights]),
                            disabled=False, layout=widgets.Layout(margin='0px 0px 0px 80px', width='550px'), style=style_default)
    weights = widgets.VBox([widgets.HBox([widgets.HTMLMath(title_strings[type_weights], layout=widgets.Layout(width='170px')), form]), set_all])
    set_all.observe(lambda change: set_all_weights(change, 1, gui, type_weights))
    weights_set_all_id[set_all] = weights
    return weights

def create_full_weight(type_weights, dimension_int, gui):
    weights_button_id = gui.weights.weights_button_id
    title = widgets.HTML('<b> %s Weights </b>' % type_weights)

    remove_button = widgets.Button(description='Remove', disabled=False, tooltip='Remove this type of weights')
    remove_button.on_click(lambda change: remove_button_clicked(change, gui))
    description = widgets.HBox([], layout=widgets.Layout(justify_content='space-around'))
    main = widgets.VBox([], layout=widgets.Layout(margin='30px 0px 0px 0px'))

    if type_weights in ['Product', 'Order-Dependent']:
        math = widgets.Label(value=math_label[type_weights], layout=widgets.Layout(width='100px', height='50px'))
        weights = create_elem_weights(type_weights, dimension_int, gui)
        main.children = [description, weights]

    elif type_weights == 'POD':
        math = widgets.Label(value=r'$$\gamma_u=\Gamma_{|u|} \prod_{j\in u} \gamma_j$$', layout=widgets.Layout(
            width='100px', height='30px'))
        weights1 = create_elem_weights('Product', dimension_int, gui)
        weights2 = create_elem_weights('Order-Dependent', dimension_int, gui)
        main.children = [description, weights1, weights2]

    elif type_weights == 'Projection-Dependent':
        math = widgets.Label(value="Most general form of weights",
                             layout=widgets.Layout(width='200px', height='30px'))
        weights = widgets.Textarea(placeholder='Enter the mapping between coordinates and weights. \
        \nEach line must be comma-separated list of coordinates followed by a colon and a weight value. \
        \nSpaces are ignored. \
        \nExample line: 1,2,5: 0.7',
                                   layout=widgets.Layout(width='inherit', height='100px'))
        main.children = [description, weights]

    description.children = [title, math, remove_button]
    weights_button_id[remove_button.model_id] = main

    return main

def func_add_weights(change, gui):
    if change['name'] == '_property_lock' and change['new'] == {}:
        change['owner'].value = 'None'
        return
    if change['name'] != 'label':
        return
    name = change['new']
    if name == 'Select type of weights':
        return
    VBOX_of_weights = gui.weights.VBOX_of_weights
    new_list = list(VBOX_of_weights.children)
    new_list.append(gui.weights.create_full_weight(name, int(gui.properties.dimension.value), gui))
    VBOX_of_weights.children = new_list

def weights():    
    add_weight = widgets.Dropdown(
    options=[('Select type of weights', 'None'), ('Product', 'product:'), ('Order-Dependent', 'order-dependent:'),
            ('POD', 'POD'), ('Projection-Dependent', 'projection-dependent')],
    value='None', description='<b> Add new weights: </b>', style=style_default)
    weights_button_id = {}
    weights_set_all_id = {}

    VBOX_of_weights = widgets.VBox([])
    weights_wrapper = widgets.Accordion([widgets.VBox([weight_math, add_weight, VBOX_of_weights])])
    weights_wrapper.set_title(0, 'Weights')

    return BaseGUIElement(add_weight=add_weight,
                          create_full_weight=create_full_weight,
                          weights_button_id=weights_button_id,
                          weights_set_all_id=weights_set_all_id,
                          VBOX_of_weights=VBOX_of_weights,
                          main=weights_wrapper,
                          _callbacks={'add_weight': func_add_weights})
