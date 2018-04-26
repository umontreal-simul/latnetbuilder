import ipywidgets as widgets
import numpy as np
import numexpr as ne

from .common import style_default

# Define weights VBox
weight_math = widgets.HTMLMath(value='The weights are of the form: \\(\\gamma_u^p (u \\subseteq \\{1, ... s\\})\\). You can specify below type of weights with their values (the actual weights are the sum of these).',
                               layout=widgets.Layout(height='30px', width='inherit'))
add_weight = widgets.Dropdown(
    options=[('Product', 'product:'), ('Order-Dependent', 'order-dependent:'),
             ('POD', 'POD'), ('Projection-Dependent', 'projection-dependent')],
    value=None, description='<b> Add new weights: </b>', style=style_default)

weight_power = widgets.Text(
    value='2',
    description='<b> Set Weights Power \\(p\\): </b>',
    layout=widgets.Layout(width='21%'),
    style=style_default
)


def widget_weight(type_weights, dimension_int, gui):
    title = widgets.HTML('<b> %s Weights </b>' % type_weights)
    title_coord = widgets.HTMLMath(
        'Set Coordinate Weights \\( \\gamma_j^p \\):', layout=widgets.Layout(width='30%'))
    title_order = widgets.HTMLMath(
        'Set Order Weights \\( \\Gamma_k^p \\):', layout=widgets.Layout(width='30%'))

    remove_button = widgets.Button(
        description='Remove', disabled=False, tooltip='Remove this type of weights')

    def remove_button_clicked(b, gui):
        weights = gui.weights.main
        VBOX_of_weights = weights.children[0].children[2]
        button_id = b.model_id
        new_list = []
        for k in range(len(VBOX_of_weights.children)):
            weight = VBOX_of_weights.children[k]
            local_button_id = weight.children[0].children[2].model_id
            if local_button_id != button_id:
                new_list.append(weight)
        VBOX_of_weights.children = new_list
    remove_button.on_click(lambda b: remove_button_clicked(b, gui))

    def set_all_weights(b, nb, gui):
        if b['name'] != 'value':
            return

        expr = b['new']
        coord = np.arange(1, gui.properties.dimension.value+1)
        try:
            expr_evaluated = ne.evaluate(
                expr, local_dict={'j': coord, 'k': coord})
            valid = True
        except:
            valid = False

        weights = gui.weights.main
        VBOX_of_weights = weights.children[0].children[2]

        for k in range(len(VBOX_of_weights.children)):
            weight = VBOX_of_weights.children[k]
            try:
                set_all = weight.children[1].children[nb]
            except:
                continue
            if b.owner == set_all:
                form = weight.children[1].children[nb-1].children[1]
                if valid:
                    for k in range(len(form.children)):
                        try:
                            form.children[k].value = str(expr_evaluated[k])
                        except:
                            form.children[k].value = str(expr_evaluated)
                    set_all.description = 'OR set all at once with (valid Python) expression: \\( \\Gamma_k^p \\)='
                else:
                    set_all.description = 'NOT VALID, please change expression: \\( \\Gamma_k^p \\)='
                return

    if type_weights == 'Product':
        math = widgets.Label(value=r'$$\gamma_u=\prod_{j\in u} \gamma_j$$', layout=widgets.Layout(
            width='100px', height='50px'))
        form = widgets.HBox([widgets.Text(value='0.1', description='', layout=widgets.Layout(
            width='10%')) for k in range(dimension_int)])
        set_all = widgets.Text(value='', placeholder='e.g.: 0.1**j',
                               description='OR set all at once with (valid Python) expression: \\( \\gamma_j^p \\)=',
                               disabled=False, layout=widgets.Layout(margin='0px 0px 0px 80px', width='550px'), style=style_default)
        weights = widgets.VBox([widgets.HBox([title_coord, form]), set_all])
        set_all.observe(lambda b: set_all_weights(b, 1, gui))
    elif type_weights == 'Order-Dependent':
        math = widgets.Label(value=r'$$\gamma_u=\Gamma_{|u|}$$', layout=widgets.Layout(
            width='100px', height='50px'))
        form = widgets.HBox([widgets.Text(value='0.1', description='', layout=widgets.Layout(
            width='10%')) for k in range(dimension_int)])
        set_all = widgets.Text(value='', placeholder='e.g.: 0.1**k',
                               description='OR set all at once with (valid Python) expression: \\( \\Gamma_k^p \\)=',
                               disabled=False, layout=widgets.Layout(margin='0px 0px 0px 80px', width='550px'), style=style_default)
        weights = widgets.VBox([widgets.HBox([title_order, form]), set_all])
        set_all.observe(lambda b: set_all_weights(b, 1, gui))
    elif type_weights == 'POD':
        math = widgets.Label(value=r'$$\gamma_u=\Gamma_{|u|} \prod_{j\in u} \gamma_j$$', layout=widgets.Layout(
            width='100px', height='30px'))
        form1 = widgets.HBox([widgets.Text(value='0.1', description='', layout=widgets.Layout(
            width='10%')) for k in range(dimension_int)])
        form2 = widgets.HBox([widgets.Text(value='0.1', description='', layout=widgets.Layout(
            width='10%')) for k in range(dimension_int)])
        set_all1 = widgets.Text(value='', placeholder='e.g.: 0.1**k',
                                description='OR set all at once with (valid Python) expression: \\( \\Gamma_k^p \\)=',
                                disabled=False, layout=widgets.Layout(margin='0px 0px 0px 80px', width='550px'), style=style_default)
        set_all2 = widgets.Text(value='', placeholder='e.g.: 0.1**j',
                                description='OR set all at once with (valid Python) expression: \\( \\gamma_j^p \\)=',
                                disabled=False, layout=widgets.Layout(margin='0px 0px 0px 80px', width='550px'), style=style_default)
        weights = widgets.VBox([widgets.HBox(
            [title_order, form1]), set_all1, widgets.HBox([title_coord, form2]), set_all2])
        set_all1.observe(lambda b: set_all_weights(b, 1, gui))
        set_all2.observe(lambda b: set_all_weights(b, 3, gui))
    elif type_weights == 'Projection-Dependent':
        math = widgets.Label(value="Most general form of weights",
                             layout=widgets.Layout(width='200px', height='30px'))
        weights = widgets.Textarea(placeholder='Enter the mapping between coordinates and weights. \
        \n Each line must be comma-separated list of coordinates followed by a colon and a weight value. \
        \n Spaces are ignored. \
        \n Example line: 1,2,5: 0.7',
                                   layout=widgets.Layout(width='inherit', height='100px'))

    description = widgets.HBox([title, math, remove_button], layout=widgets.Layout(
        justify_content='space-around'))
    main = widgets.VBox([description, weights],
                        layout=widgets.Layout(margin='30px 0px 0px 0px'))

    return main


weights_wrapper = widgets.Accordion([widgets.VBox([weight_math, widgets.HBox([weight_power, add_weight],
                                                                     layout=widgets.Layout(justify_content='space-around')),
                                           widgets.VBox([])])])
weights_wrapper.set_title(0, 'Weights')

# add weight observe
def func_add_weights(b, gui):
    if b['name'] != 'label':
        return
    name = b['new']
    weights = gui.weights.main
    VBOX_of_weights = weights.children[0].children[2]
    new_list = []
    for k in range(len(VBOX_of_weights.children)):
        weight = VBOX_of_weights.children[k]
        new_list.append(weight)
    new_list.append(widget_weight(name, int(gui.properties.dimension.value), gui))
    VBOX_of_weights.children = new_list