import ipywidgets as widgets

from .common import style_default, INITIAL_DIM, JoeKuoSobolNets, BaseGUIElement

explr_data = {
    'lat-eval': '<p> A given generating vector \\(a = (a_1, ..., a_s)\\) is specified in the boxes below. </p>\
    <p> Two possibilities are given: </p> \
    <ul>\
    <li> simply evaluate the figure of merit for the lattice defined by this generating vector </li>\
    <li> extend the lattice defined by this generating vector and modulus </li>\
    </ul>',
    'exhaustive': '<p> All generating vectors are examined and the best one is selected. </p>\
    <p> If you tick "random choice r of samples", a number r of randomly selected generating vectors \\(a\\) will be examined instead of all vectors. </p>',
    'Korobov': '<p> All generating vectors of the form \\(a=(1,a \\text{ mod } n,a^2 \\text{ mod } n,…,a^{s-1} \\text{ mod } n)\\) are examined and the best one is selected. </p>\
    <p> If you tick "random choice r of samples a number r of randomly selected generating vectors \\(a\\) will be examined instead of all Korobov vectors. </p>',
    'CBC': '<p> All possible values of the components \\(a_j\\) of the generating vector \\(a=(a_1,…,a_s)\\) are examined, and the best ones are selected, one coordinate at a time. </p>\
    <p> If you tick "random choice r of samples", a number r of randomly selected values for each component \\(a_j\\) will be examined instead of all possibilities.</p>',
    'fast-CBC': '<p> All possible values of the components components \\(a_j\\) of the generating vector \\(a=(a_1,…,a_s)\\) are examined and the best ones are selected, one coordinate at a time. </p>\
    <p> Computation is accelerated by using Fast Fourier Transforms. </p>\
    <p style="color:red"> Requires the Coordinate-Uniform evaluation method.</p>',
    'full-CBC': '<p> All possible values of the components \\(a_j\\) of the generating vector \\(a=(a_1,…,a_s)\\) are examined the best ones are selected, one coordinate at a time. </p>\
    <p> If you tick "random choice r of samples", a number r of randomly selected values for each component \\(a_j\\) will be examined instead of all possibilities.\
    If the minimal random dimension \\(d\\) equals one, the search is entirely random. Else, the search is exhaustive up to coordinate \\(d-1\\), and random from coordinate \\(d\\). </p>',
    'net-eval' : '<p> Explicit net evaluation. Enter below the net characteristics:\
        <ul>\
            <li> direction numbers for Sobol construction (please respect the format given as example)\
            <li> generating vector for the polynomial construction \
            <li> generating matrices for the explicit construction (one matrix per line, each integer encodes a column)\
        </ul>\
    </p>'
}

def change_explr_choice(change, gui):
    if change['name'] != 'value':
        return
    new_choice = change['new']
    if new_choice == None:
        return

    if new_choice == 'evaluation':
        if gui.main_tab.selected_index == 0:    
            gui.exploration_method.explr_info.value = explr_data['lat-eval']
        else:
            gui.exploration_method.explr_info.value = explr_data['net-eval']
        gui.exploration_method.from_previous_search.layout.display = 'flex'
    else:
        gui.exploration_method.explr_info.value = explr_data[new_choice]
        gui.exploration_method.from_previous_search.layout.display = 'none'
        gui.exploration_method.from_previous_search_warning.layout.display = 'none'

    if new_choice == 'evaluation' and gui.main_tab.selected_index == 0:     
        gui.exploration_method.generating_vector.layout.display = 'flex'
    else:
        gui.exploration_method.generating_vector.layout.display = 'none'

    if new_choice in ['exhaustive', 'Korobov', 'full-CBC']:
        gui.exploration_method.random_box.layout.display = 'flex'
    else:
        gui.exploration_method.random_box.layout.display = 'none'

    if new_choice == 'full-CBC' and gui.main_tab.selected_index == 1:
        gui.exploration_method.mixed_CBC_level.layout.display = 'flex'
    else:
        gui.exploration_method.mixed_CBC_level.layout.display = 'none'

    gui.exploration_method.generating_numbers_sobol_box.layout.display = 'none'
    gui.exploration_method.generating_vector_simple.layout.display = 'none'
    gui.exploration_method.generating_matrices.layout.display = 'none'
    if new_choice == 'evaluation'  and gui.main_tab.selected_index == 1:
        if gui.construction_method.construction_choice.value == 'sobol':
            gui.exploration_method.generating_numbers_sobol_box.layout.display = 'flex'
        elif gui.construction_method.construction_choice.value == 'polynomial':
            gui.exploration_method.generating_vector_simple.layout.display = 'flex'
        elif gui.construction_method.construction_choice.value == 'explicit':
            gui.exploration_method.generating_matrices.layout.display = 'flex'
            

def trigger_is_random(change, gui):
    if change['name'] != 'value':
        return
    new_choice = change['new']
    gui.exploration_method.mixed_CBC_level.disabled = not new_choice
    if new_choice:
        gui.exploration_method.number_samples.layout.display = 'flex'
    else:
        gui.exploration_method.number_samples.layout.display = 'none'

def automatic_generating_numbers_sobol(change, gui):
    gui.exploration_method.generating_numbers_sobol.value = '\n'.join(JoeKuoSobolNets[:(gui.properties.dimension.value * gui.properties.interlacing.value)])

def fill_from_previous_search(change, gui):
    done = False
    if gui.output.result_obj is not None:
        result = gui.output.result_obj
        if 'digital' in gui.search.search_type() and gui.main_tab.selected_index == 1:
            if gui.search.search_type() == 'digital-polynomial' and gui.construction_method.construction_choice.value == 'polynomial':
                done = True
                for k in range(int(result.dim * result.interlacing)):
                    gui.exploration_method.generating_vector_simple.children[k].value = str(result.gen_vector[k])

            elif gui.search.search_type() == 'digital-sobol' and gui.construction_method.construction_choice.value == 'sobol':
                done = True
                gui.exploration_method.generating_numbers_sobol.value = '\n'.join([','.join(list(map(str, result.gen_vector[k]))) for k in range(result.dim * result.interlacing)])
            
            elif gui.search.search_type() == 'digital-explicit' and gui.construction_method.construction_choice.value == 'explicit':
                done = True
                gui.exploration_method.generating_matrices.value = '\n'.join([' '.join(map(str, [result.matrices_cols[coord, j] for j in range(result.nb_cols)])) for coord in range(result.dim * result.interlacing)])

        if gui.main_tab.selected_index == 0 and gui.lattice_type.type_choice.value == gui.search.search_type():
            done = True
            for k in range(int(result.dim * result.interlacing)):
                gui.exploration_method.generating_vector.children[0].children[k].value = str(result.gen_vector[k])
    if done:
        gui.exploration_method.from_previous_search_warning.layout.display = 'none'
    else:
        gui.exploration_method.from_previous_search_warning.layout.display = 'flex'


def exploration_method():
    exploration_choice = widgets.ToggleButtons(
        value='fast-CBC',
        options=[('Evaluate/Extend', 'evaluation'), ('All space', 'exhaustive'),
                ('Korobov', 'Korobov'), ('CBC', 'full-CBC'), ('Fast CBC', 'fast-CBC')],
        description='Choose one:',
        style=style_default
    )

    is_random = widgets.Checkbox(description='Random choice of r points', value=False)
    number_samples = widgets.Text(value='10', description='Set r:', layout=widgets.Layout(display='none'))

    random_box = widgets.HBox([is_random, number_samples], style=style_default, layout=widgets.Layout(display='none'))


    generating_numbers_sobol = widgets.Textarea(placeholder='0\n1\n1,1\n1,3,5',
                                    layout=widgets.Layout(width='inherit', height='150px'))
    generating_numbers_sobol_button = widgets.Button(description='Joe and Kuo direction numbers', layout=widgets.Layout(width='270px'))
    generating_numbers_sobol_box = widgets.VBox([generating_numbers_sobol_button, generating_numbers_sobol],
                                    layout=widgets.Layout(display='none'))

    generating_matrices = widgets.Textarea(placeholder='895483904 868220928 1402994688\n819986432 1296039936 1757413376\n308281344 1266679808 58720256\n278921216 1228931072 838860800',
                                    layout=widgets.Layout(width='inherit', height='150px', display='none'))

    from_previous_search = widgets.Button(description='Evaluate from previous search', style=style_default, layout=widgets.Layout(width='200px', display='none', margin='40px 0px 0px 0px'))
    from_previous_search_warning = widgets.HTML(value='<span style="color:red"> The search result and the current point set type /<br> construction method do not match. </span>', layout=widgets.Layout(display='none'))

    generating_vector_simple = widgets.HBox(
            [widgets.Text(value='1', description='', 
            layout=widgets.Layout(width='100px'), style=style_default) for k in range(INITIAL_DIM)],
            layout=widgets.Layout(display='none'))

    generating_vector = widgets.VBox([
        widgets.HBox(
            [widgets.Text(value='1', description='', 
            layout=widgets.Layout(width='100px'), style=style_default) for k in range(INITIAL_DIM)]),
        widgets.Text(placeholder='e.g. 2^8 or 256', description='If you want to extend, please specify the former modulus:',
                    layout=widgets.Layout(width='600px'), style=style_default)
    ],
        layout=widgets.Layout(display='none'))

    mixed_CBC_level = widgets.BoundedIntText(value=1, min=1, description='Minimal random dimension:',
                        layout=widgets.Layout(width='250px', display='none', margin='0px 0px 0px 80px'), style=style_default, disabled=True)

    explr_info = widgets.HTMLMath(explr_data[exploration_choice.value])
    exploration = widgets.Accordion([widgets.VBox(
        [exploration_choice, widgets.HBox([explr_info, widgets.VBox([from_previous_search, from_previous_search_warning])], layout=widgets.Layout(justify_content='space-around', align_items='center')),
        generating_vector_simple, generating_vector, generating_matrices, random_box, mixed_CBC_level, generating_numbers_sobol_box])])
    exploration.set_title(0, 'Exploration Method')

    return BaseGUIElement(explr_info=explr_info,
                          exploration_choice=exploration_choice,
                          from_previous_search=from_previous_search,
                          from_previous_search_warning=from_previous_search_warning,
                          is_random=is_random,
                          number_samples=number_samples,
                          generating_vector=generating_vector,
                          generating_vector_simple=generating_vector_simple,
                          generating_matrices=generating_matrices,
                          random_box=random_box,
                          mixed_CBC_level=mixed_CBC_level,
                          generating_numbers_sobol=generating_numbers_sobol,
                          generating_numbers_sobol_box=generating_numbers_sobol_box,
                          generating_numbers_sobol_button=generating_numbers_sobol_button,
                          main=exploration,
                          _callbacks={'exploration_choice': change_explr_choice,
                                      'is_random': trigger_is_random},
                          _on_click_callbacks={'generating_numbers_sobol_button': automatic_generating_numbers_sobol,
                                               'from_previous_search': fill_from_previous_search})
