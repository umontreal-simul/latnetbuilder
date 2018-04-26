import ipywidgets as widgets
import numpy as np
# from .gui import lattice_type, normalization, low_pass_filter, low_pass_filter_options, multilevel, normalization_options,\
# alpha, alpha2, figure_type, weights, dimension, style_default, add_weight
from .gui import *

trigger_dic = {lattice_type: embedding, 
               normalization: normalization_options,
              low_pass_filter: low_pass_filter_options,
              mult_normalization: mult_normalization_options,
              mult_low_pass_filter: mult_low_pass_filter_options,
              mult_combiner: combiner_options}

def change_modulus(b):
    if b['name'] != 'value' or main_gui.selected_index != 1:
        return

    poly_str = ''    
    try:
        p = b['new'].split('^')
        if len(p) == 2:
            base = np.array([int(y) for y in p[0]])
            power = int(p[1])
            modulus = ((np.poly1d(np.flip(base, axis=0)))**power).c % 2
        else:
            base = np.array([int(y) for y in b['new']])
            modulus = np.poly1d(np.flip(base, axis=0)).c
        poly_str = ' '
        for k in range(len(modulus)):
            if modulus[k] == 1:
                if poly_str != ' ':
                    poly_str += '+'
                poly_str += ' z^' + str(len(modulus)-k-1) 
    except:
        pass
    if poly_str == ' ':
        poly_str = '0'
    size.description = "Mod n=" + poly_str +'='
    size_width = str(10 + len(size.description)) + '%'
    size.layout = widgets.Layout(width=size_width)
    

def change_combiner_options(b):
    # print(b)
    if b['name'] == 'value':
        if b['new'] == 'level:':
            combiner_level.layout.display = 'flex'
        else:
            combiner_level.layout.display = 'none'

def change_evaluation_method(b):
    if b['new'] == True:
        norm_type.disabled = True
        norm_type.value = '2'
    elif b['new'] == False:
        norm_type.disabled = False

def change_lattype(b):
    if b['name'] == 'selected_index' and b['new'] == 1:
        size.placeholder = 'e.g. 01^8 or 000000001'
        generating_vector.children[1].placeholder = 'e.g. 01^6 or 0000001'
        figure_type.options = ['Palpha', 'R']
        figure_of_merit_expl.value = '<p> Please note that: </p> \
                     <ul>\
                        <li> The Coordinate-Uniform method is generally faster, but requires \\(q = 2\\) </li> \
                        <li> The \\(P_\\alpha \\) method requires \\( \\alpha \\in \\{ 2, 4, 6 \\} \\) </li> \
                     </ul>'
    elif b['name'] == 'selected_index' and b['new'] == 0:
        size.description = 'Modulus n='
        size.placeholder = 'e.g. 2^10 or 1024'
        generating_vector.children[1].placeholder = 'e.g. 2^8 or 256'
        figure_type.options = ['Palpha', 'Ralpha', 'Spectral']
        figure_of_merit_expl.value = '<p> Please note that: </p> \
                     <ul>\
                        <li> The Coordinate-Uniform method is generally faster, but requires \\(q = 2\\) </li> \
                        <li> The Spectral figure cannot be be evaluated by the Coordinate-Uniform method </li> \
                        <li> The \\(P_\\alpha \\) method requires \\( \\alpha \\in \\{ 2, 4, 6 \\} \\) </li> \
                     </ul>'


def changed(b):
   # print(b)
    if b['new'] == False:
        trigger_dic[b['owner']].layout.display = 'none'
    elif b['new'] == True:
        trigger_dic[b['owner']].layout.display = 'flex'
        
def changed2(b):
   # print(b)
    if b['new'] == 'Spectral':
        alpha.layout.display = 'none'
        evaluation_method.disabled = True
        evaluation_method.value = False
    elif b['new'] in ['Ralpha', 'Palpha']:
        alpha.layout.display = 'flex'
        evaluation_method.disabled = False
    elif b['new'] == 'R':
        alpha.layout.display = 'none'
        evaluation_method.disabled = False

def func_add_weights(b):
    if b['name'] != 'label':
        return
    # print(b)
    name = b['new']
    global weights
    VBOX_of_weights = weights.children[0].children[2]
    new_list = []
    for k in range(len(VBOX_of_weights.children)):
        weight = VBOX_of_weights.children[k]
        new_list.append(weight)
    new_list.append(widget_weight(name, int(dimension.value)))
    VBOX_of_weights.children = new_list
    # b.owner.label = 'Select type'

def update(form, dim, defaut_value):
    new_children = []
    if len(form.children) < dim:
        for k in range(len(form.children)):
            new_children.append(form.children[k])
        for k in range(dim-len(form.children)):
            new_children.append(widgets.Text(value=defaut_value, description='', layout=widgets.Layout(width='10%')))
    elif len(form.children) > dim:
        for k in range(dim):
            new_children.append(form.children[k])
    form.children = new_children

def change_dimension(b):
    if b['name'] != 'value':
        return
    dim = b['new']
    global weights
    VBOX_of_weights = weights.children[0].children[2]
    for k in range(len(VBOX_of_weights.children)):
        try:
            form = VBOX_of_weights.children[k].children[1].children[0].children[1]
            update(form, dim, '0.1')
            form = VBOX_of_weights.children[k].children[1].children[2].children[1]
            update(form, dim, '0.1')
        except:
           continue
    update(generating_vector.children[0], dim+1, '1')

def change_constr_choice(b):
    # print(b)
    if b['name'] != 'index':
        return
    new_choice = b['new']
    global constr_info, constr_data
    constr_info.value = ' '.join(constr_data[new_choice].split('&\\'))
    if new_choice == 0:
        generating_vector.layout.display = 'flex'
        number_samples.layout.display = 'none'
    elif new_choice in [1, 2, 3]:
        number_samples.layout.display = 'flex'
        generating_vector.layout.display = 'none'
    else:
        number_samples.layout.display = 'none'
        generating_vector.layout.display = 'none'

def create_output(result3):
    sc_x = LinearScale(min=0., max=1.)
    sc_y = LinearScale(min=0., max=1.)

    scatt = Scatter(x=result3.getNet(0), 
                    y=result3.getNet(1), 
                    scales={'x': sc_x, 'y': sc_y},
                    default_size=20)
    ax_x = Axis(scale=sc_x, label='Coordinate 1')
    ax_y = Axis(scale=sc_y, label='Coordinate 2', orientation='vertical')

    fig = Figure(marks=[scatt], axes=[ax_x, ax_y], layout=widgets.Layout(width='650px', height='650px'))
    # fig.min_height = '700px'
    # fig.min_width = '700px'

    b1 = widgets.BoundedIntText(max=result3.getDim(), min=1, value=1, layout=widgets.Layout(width='150px'), description='x-axis')
    b2 = widgets.BoundedIntText(max=result3.getDim(), min=1, value=2, layout=widgets.Layout(width='150px'), description='y-axis')
    b3 = widgets.BoundedIntText(max=result3.getMaxLevel(), min=1, value=result3.getMaxLevel(), layout=widgets.Layout(width='150px'), description='Level')
    if result3.polynomial:
        b3.disabled = True
    def change_graph(b):
        if b['name'] == 'value':
            if b['owner'] == b1:
                scatt.x = result3.getNet(b['new']-1, b3.value)
                ax_x.label = 'Coordinate ' + str(b['new'])
            if b['owner'] == b2:
                scatt.y = result3.getNet(b['new']-1, b3.value)
                ax_y.label = 'Coordinate ' + str(b['new'])
            if b['owner'] == b3:
                scatt.x = result3.getNet(b1.value-1, b['new'])
                scatt.y = result3.getNet(b2.value-1, b['new'])
    b1.observe(change_graph)
    b2.observe(change_graph)
    b3.observe(change_graph)

    plot = widgets.HBox([fig, widgets.VBox([b1, b2, b3])], 
                        layout=widgets.Layout(align_items='center'))

    output = widgets.Tab()

    if not result3.polynomial:
        code_C = widgets.Textarea(value='\
    int n = %i;\n\
    int s = %i;\n\
    int a[] = %s;\n\
    double points[n][s];\n\
    int i, j;\n\
    for (i = 0; i < n; i++)\n\
        for (j = 0; j < s; j++)\n\
            points[i][j] = ((long long)i * a[j]) %% n / (double)n;' \
    % (result3.lattice.size.points, result3.getDim(), str(result3.lattice.gen).replace('[', '{').replace(']', '}')),
            layout=widgets.Layout(width='600px', height='200px'))

        code_python = widgets.Textarea(value='\
    n = %i;\n\
    a = %s;\n\
    points = [[(i * aj %% n) / float(n) for aj in a] for i in range(n)]' \
    % (result3.lattice.size.points, str(result3.lattice.gen)),
            layout=widgets.Layout(width='600px', height='100px'))

        code_matlab = widgets.Textarea(value='\
    n = %i;\n\
    a = %s;\n\
    points = zeros(n,length(a));\n\
    for i = 1:n\n\
        points(i,:) = mod((i - 1) * a, n) / n;\n\
    end' \
    % (result3.lattice.size.points, str(result3.lattice.gen)),
            layout=widgets.Layout(width='600px', height='150px'))

    
        output.children = [plot, code_C, code_python, code_matlab]
        output.set_title(0, 'Plot')
        output.set_title(1, 'C code')
        output.set_title(2, 'Python code')
        output.set_title(3, 'Matlab code')
    
    else:
        if result3.lattice.size.power == 1:
            modulus = result3.lattice.size.base
        else:
            modulus = np.flip((np.poly1d(np.array(np.flip(result3.lattice.size.base, axis=0)))**result3.lattice.size.power).c % 2, axis=0)
        code_python = widgets.Textarea(value='\
import numpy as np\n\
\n\
modulus = %s\n\
genVector = %s\n\
\n\
def expandSeries(P, h):\n\
    expansion = []\n\
    m = len(P)-1\n\
    for l in range(1, 2*m):\n\
        res = 1 if (m-l >=0 and m-l < len(h) and h[m-l]) else 0\n\
        start = l-m if (l-m > 1) else 1\n\
        for p in range(start, l):\n\
            res = (res + expansion[p-1]*P[m-l+p]) %% 2\n\
        expansion.append(res)\n\
    return expansion\n\
\n\
def generatingMatrices(modulus, genVector):\n\
    matrices = []\n\
    m = len(modulus)-1\n\
    for j in range(len(genVector)):\n\
        matrix = np.zeros((m, m), dtype=np.int32)\n\
        expansion = expandSeries(modulus, genVector[j])\n\
        for i in range(m):\n\
            for j in range(m):\n\
                matrix[i][j] = expansion[i+j]\n\
        matrices.append(matrix)\n\
    return matrices\n\
\n\
points = []\n\
matrices = generatingMatrices(modulus, genVector)\n\
width = matrices[0].shape[0]\n\
nb_points = 2**width\n\
mult = np.array([2**(-k) for k in range(1, width+1)])\n\
\n\
for matrice in matrices:\n\
    list_in_coord = []\n\
    for x in range(nb_points):\n\
        binary_repr = np.array([((x>>i)&1) for i in range(width)])\n\
        prod = np.mod(matrice.dot(binary_repr), 2)\n\
        list_in_coord.append(prod.dot(mult))\n\
    points.append(list_in_coord)' % (str(modulus), str(result3.lattice.gen)),
            layout=widgets.Layout(width='600px', height='700px'))
        
        output.children = [plot, code_python]
        output.set_title(0, 'Plot')
        output.set_title(1, 'Python code')

    return output

def display_output(b):
    global my_output
    if b['name'] == 'value' and b['new'] == True:
        try:
            # my_output
            # my_output.layout.display = 'flex'
            del my_output
        except:
            pass
        my_output = create_output(result3)
        display(my_output)
            # pass
        # my_output.layout.display = 'flex'
    if b['name'] == 'value' and b['new'] == False:
        my_output.layout.display = 'none'