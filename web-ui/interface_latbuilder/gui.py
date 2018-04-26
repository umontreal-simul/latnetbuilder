# from asteval import Interpreter
import numpy as np
import numexpr as ne
import ipywidgets as widgets
from bqplot import Axis, LinearScale, Scatter, Figure
from IPython.display import display

from .parse import Result

style_default = {'description_width': 'initial'}

### Definition of properties VBox
size = widgets.Text(placeholder='e.g. 2^10 or 1024', description='Modulus n=', 
                    layout=widgets.Layout(width='30%'), style = style_default)

lattice_type = widgets.Checkbox(value=False, description='Embedded')                    

dimension = widgets.BoundedIntText(value=3, min=1, description='Dimension s:',
    layout=widgets.Layout(width='20%'), style = style_default)

properties_title = widgets.HTML('<b>Lattice Properties </b>')

properties = widgets.Accordion([widgets.HBox([size, lattice_type, dimension])])
properties.set_title(0, 'Basic Lattice properties')

### Definition of filters VBox
normalization = widgets.Checkbox(description='Normalization', value=False, style = style_default)
low_pass_filter = widgets.Checkbox(description='Low-Pass Filter', value=False, style = style_default)
normalization_options = widgets.HBox([widgets.Dropdown(options=['SL10 Palpha', 'DPW08 Palpha'], description='Normalization Type:', 
    layout=widgets.Layout(width='30%'), style = style_default)
                                    , widgets.HTML('<p style="color:red"> Warning: DPW08 Palpha works only with product weights</p>')], layout=widgets.Layout(display='none'))
low_pass_filter_options = widgets.Text(
    value='1',
    description='Low-Pass Threshold',
    layout=widgets.Layout(display='none', width='25%'),
    style = style_default
)
filters = widgets.Accordion([widgets.VBox([widgets.HBox([normalization, low_pass_filter]),
                                            normalization_options, low_pass_filter_options])])
filters.set_title(0, 'Filters (optional)')
filters.selected_index = None

### Definition of multi-level VBox

mult_normalization = widgets.Checkbox(description='Normalization', value=False, style = style_default)
mult_low_pass_filter = widgets.Checkbox(description='Low-Pass Filter', value=False, style = style_default)
mult_combiner = widgets.Checkbox(description='Combiner', value=False, style = style_default)

multilevel = widgets.HBox([mult_normalization, mult_low_pass_filter, mult_combiner])


### Definition of normalization options VBox
normalization_type = widgets.Dropdown(options=['SL10 Palpha', 'DPW08 Palpha'], description='Normalization Type:', 
    layout=widgets.Layout(width='30%'), style = style_default)

minimum_level = widgets.Text(placeholder='1', description='Min Level (optional):',
    layout=widgets.Layout(width='22%', margin='0px 10px 0px 0px'), style = style_default)

maximum_level = widgets.Text(placeholder='10', description='Max Level (optional):',
    layout=widgets.Layout(width='22%'), style = style_default)

mult_normalization_options = widgets.VBox([
    widgets.HBox([widgets.Dropdown(options=['SL10 Palpha', 'DPW08 Palpha'], description='Normalization Type:', layout=widgets.Layout(width='30%'), style = style_default), 
                widgets.HTML('<p style="color:red"> Warning: DPW08 Palpha works only with product weights</p>')]),
    widgets.HBox([minimum_level, maximum_level], layout=widgets.Layout(margin='0px 0px 0px 80px'))],
    layout=widgets.Layout(display='none'))

### Definition of low pass filter options widget
mult_low_pass_filter_options = widgets.Text(
    value='1',
    description='Low-Pass Threshold',
    layout=widgets.Layout(display='none', width='25%'),
    style = style_default
)

combiner_level = widgets.BoundedIntText(description='Level:', min=1, layout=widgets.Layout(display='none', width='15%'))
combiner_dropdown = widgets.Dropdown(
    # options are (label, value) pairs
    options=[('weighted sum', 'sum'), 
    ('maximum weighted value', 'max'),
    ('weighted value of the highest level', 'level:max'),
    ('weighted value of a specific level', 'level:')],
    value=None, description='Combiner:', style = style_default)
combiner_options = widgets.HBox([combiner_dropdown, combiner_level], layout=widgets.Layout(display='none'))


embedding = widgets.Accordion([widgets.VBox([multilevel, mult_normalization_options, mult_low_pass_filter_options, combiner_options])],
                               layout= widgets.Layout(display='none'))
embedding.set_title(0, 'For Embedded Lattices: multilevel filters and combiners (optional)')
embedding.selected_index = None


### Definition of figure of merits widget

# figure_math = widgets.Label(
#     value=r"$$\left[D_q(P_n)\right]^q=\sum_{\emptyset \neq u \subseteq \{1, ... s\}} \gamma_u^q \left[D_u(P_n)\right]^q$$",
#     layout=widgets.Layout(height='50px'))

# figure_math = widgets.HTMLMath(
#     value=r"$$\left[D_q(P_n)\right]^q=\sum_{\emptyset \neq u \subseteq \{1, ... s\}} \gamma_u^q \left[D_u(P_n)\right]^q$$",
#     layout=widgets.Layout(height='50px'))

figure_type = widgets.Dropdown(
    # options are (label, value) pairs
    options=['Palpha', 'Ralpha', 'Spectral'],
    value='Palpha', description='Figure:',
    layout=widgets.Layout(width='15%'), style = style_default)

alpha = widgets.Text(value='2', description=r'Value of \(\alpha\):',
    layout=widgets.Layout(width='15%'), style = style_default)

# alpha2 = widgets.IntSlider(value=2, min=2, max=6, step=2, description=r'Value of \(\alpha\):',
#     layout=widgets.Layout(width='25%'), style = style_default)

evaluation_method = widgets.Checkbox(value=True, description='Coordinate-Uniform evaluation',
    style = style_default)

norm_type = widgets.Text(value='2', description=r'Norm type \(q>0\) or \(q\) = "inf":',
    layout=widgets.Layout(width='24%'), style = style_default, disabled=True)

figure_of_merit_expl = widgets.HTMLMath(value='<p> Please note that: </p> \
                     <ul>\
                        <li> The Coordinate-Uniform method is generally faster, but requires \\(q = 2\\) </li> \
                        <li> The Spectral figure cannot be be evaluated by the Coordinate-Uniform method </li> \
                        <li> The \\(P_\\alpha \\) method requires \\( \\alpha \\in \\{ 2, 4, 6 \\} \\) </li> \
                     </ul>')

figure_of_merit = widgets.Accordion([
    widgets.VBox([
        figure_of_merit_expl,
        widgets.HBox([figure_type, alpha, evaluation_method, norm_type],
                                    layout = widgets.Layout(justify_content='space-between')
                                    )])
    ])
    
figure_of_merit.set_title(0, 'Figure of Merit')

### Define weights VBox
weight_math = widgets.HTMLMath(value='The weights are of the form: \\(\\gamma_u^p (u \\subseteq \\{1, ... s\\})\\). You can specify below type of weights with their values (the actual weights are the sum of these).',
                           layout=widgets.Layout(height='30px', width='inherit'))
add_weight = widgets.Dropdown(
    options=[('Product', 'product:'), ('Order-Dependent', 'order-dependent:'), ('POD', 'POD'), ('Projection-Dependent', 'projection-dependent')],
    value=None, description='<b> Add new weights: </b>', style = style_default)

weight_power = widgets.Text(
    value='2',
    description='<b> Set Weights Power \\(p\\): </b>',
    layout=widgets.Layout(width='21%'),
    style = style_default
)

def widget_weight(type_weights, dimension_int):
    title = widgets.HTML('<b> %s Weights </b>' % type_weights)
    title_coord = widgets.HTMLMath('Set Coordinate Weights \\( \\gamma_j^p \\):', layout=widgets.Layout(width='30%'))
    title_order = widgets.HTMLMath('Set Order Weights \\( \\Gamma_k^p \\):', layout=widgets.Layout(width='30%'))

    remove_button = widgets.Button(description='Remove', disabled=False, tooltip='Remove this type of weights')
    def remove_button_clicked(b):
        global weights
        VBOX_of_weights = weights.children[0].children[2]
        button_id = b.model_id
        new_list = []
        for k in range(len(VBOX_of_weights.children)):
            weight = VBOX_of_weights.children[k]
            local_button_id = weight.children[0].children[2].model_id
            if local_button_id != button_id:
                new_list.append(weight)
        VBOX_of_weights.children = new_list
    remove_button.on_click(remove_button_clicked)

    def set_all_weights(b, nb):
        if b['name'] != 'value':
            return

        expr = b['new']
        coord = np.arange(1, dimension.value+1)
        try:
            expr_evaluated = ne.evaluate(expr, local_dict={'j':coord, 'k':coord})
            valid = True
        except:
            valid = False

        global weights
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
        math = widgets.Label(value=r'$$\gamma_u=\prod_{j\in u} \gamma_j$$', layout=widgets.Layout(width='100px', height='50px'))
        form = widgets.HBox([widgets.Text(value='0.1', description='', layout=widgets.Layout(width='10%')) for k in range(dimension_int)])
        set_all = widgets.Text(value='', placeholder='e.g.: 0.1**j',
                            description='OR set all at once with (valid Python) expression: \\( \\gamma_j^p \\)=',
                            disabled=False, layout=widgets.Layout(margin='0px 0px 0px 80px', width='550px'), style = style_default)
        weights = widgets.VBox([widgets.HBox([title_coord, form]), set_all])
        set_all.observe(lambda b: set_all_weights(b, 1))
    elif type_weights == 'Order-Dependent':
        math = widgets.Label(value=r'$$\gamma_u=\Gamma_{|u|}$$', layout=widgets.Layout(width='100px', height='50px'))
        form = widgets.HBox([widgets.Text(value='0.1', description='', layout=widgets.Layout(width='10%')) for k in range(dimension_int)])
        set_all = widgets.Text(value='', placeholder='e.g.: 0.1**k',
                    description='OR set all at once with (valid Python) expression: \\( \\Gamma_k^p \\)=',
                    disabled=False, layout=widgets.Layout(margin='0px 0px 0px 80px', width='550px'), style = style_default)
        weights = widgets.VBox([widgets.HBox([title_order, form]), set_all])
        set_all.observe(lambda b: set_all_weights(b, 1))
    elif type_weights == 'POD':
        math = widgets.Label(value=r'$$\gamma_u=\Gamma_{|u|} \prod_{j\in u} \gamma_j$$', layout=widgets.Layout(width='100px', height='30px'))
        form1 = widgets.HBox([widgets.Text(value='0.1', description='', layout=widgets.Layout(width='10%')) for k in range(dimension_int)])
        form2 = widgets.HBox([widgets.Text(value='0.1', description='', layout=widgets.Layout(width='10%')) for k in range(dimension_int)])
        set_all1 = widgets.Text(value='', placeholder='e.g.: 0.1**k',
                    description='OR set all at once with (valid Python) expression: \\( \\Gamma_k^p \\)=',
                    disabled=False, layout=widgets.Layout(margin='0px 0px 0px 80px', width='550px'), style = style_default)
        set_all2 = widgets.Text(value='', placeholder='e.g.: 0.1**j',
                            description='OR set all at once with (valid Python) expression: \\( \\gamma_j^p \\)=',
                            disabled=False, layout=widgets.Layout(margin='0px 0px 0px 80px', width='550px'), style = style_default)
        weights = widgets.VBox([widgets.HBox([title_order, form1]), set_all1, widgets.HBox([title_coord, form2]), set_all2])
        set_all1.observe(lambda b: set_all_weights(b, 1))
        set_all2.observe(lambda b: set_all_weights(b, 3))
    elif type_weights == 'Projection-Dependent':
        math = widgets.Label(value="Most general form of weights", layout=widgets.Layout(width='200px', height='30px'))
        weights = widgets.Textarea(placeholder='Enter the mapping between coordinates and weights. \
        \n Each line must be comma-separated list of coordinates followed by a colon and a weight value. \
        \n Spaces are ignored. \
        \n Example line: 1,2,5: 0.7',
        layout=widgets.Layout(width='inherit', height='100px'))

    description = widgets.HBox([title, math, remove_button], layout=widgets.Layout(justify_content='space-around'))
    main = widgets.VBox([description, weights], layout=widgets.Layout(margin='30px 0px 0px 0px'))
    
    return main

weights = widgets.Accordion([widgets.VBox([weight_math, widgets.HBox([weight_power, add_weight], 
                                            layout = widgets.Layout(justify_content='space-around')),
                        widgets.VBox([widget_weight('Product', int(dimension.value))])])])
weights.set_title(0, 'Weights')

### Define construction VBox
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
    options=[('Evaluate/Extend', 'explicit:'), ('All space', 'exhaustive'), ('Korobov', 'Korobov'), ('CBC', 'CBC'), ('Fast CBC', 'fast-CBC')],
    description='Choose one:',
    style = style_default
)

is_random = widgets.Checkbox(description=r'Random choice of \(r\) points', value=False)
number_samples = widgets.HBox(
                        [is_random, widgets.Text(value='10', description=r'Set \(r\):')]
                        , layout=widgets.Layout(display='none'), style = style_default)

generating_vector = widgets.VBox([
    widgets.HBox(
        [widgets.Label("Generating vector:")] +
        [widgets.Text(value='1', description='', layout=widgets.Layout(width='10%'), style = style_default) for k in range(int(dimension.value))]),
    widgets.Text(placeholder='e.g. 2^8 or 256', description='If you want to extend, please specify the former modulus:', 
                    layout=widgets.Layout(width='65%'), style = style_default)
    ],
    layout=widgets.Layout(display='none'))

constr_info = widgets.HTMLMath('')
construction = widgets.Accordion([widgets.VBox([construction_choice, constr_info, generating_vector, number_samples])])
construction.set_title(0, 'Construction Method')

# Define go and result widgets
go = widgets.Button(description='Search', disabled=False)
abort = widgets.ToggleButton(value=False, description='Abort search', disabled=True, button_style='warning')
command_line = widgets.Button(description='Construct command line', disabled=False, layout=widgets.Layout(width='200px'))
button_box = widgets.HBox([go, abort, command_line], layout=widgets.Layout(margin='30px 0px 0px 0px'))

result = widgets.HTML(description='<b> Result: </b>', value='', layout=widgets.Layout(width='900px'), disabled=False)
result2 = widgets.Textarea(description='Result2', value='', layout=widgets.Layout(width='900px'), disabled=False)

command_line_out = widgets.HTML(description='<b> Command line: </b>', layout=widgets.Layout(width='900px'), disabled=False, style=style_default)
result3 = Result()

display_button = widgets.ToggleButton(value=False, description='Display plot + code to generate points', 
disabled=True, tooltip='Description', layout=widgets.Layout(width='300px'))

internal_gui = widgets.VBox([properties, 
                    embedding,
                    figure_of_merit, 
                    weights, 
                    construction, 
                    filters,
                    button_box,
                    command_line_out,
                    result, 
                    display_button])

main_gui = widgets.Tab([internal_gui, internal_gui])
main_gui.set_title(0, 'Ordinary Lattice')
main_gui.set_title(1, 'Polynomial Lattice')