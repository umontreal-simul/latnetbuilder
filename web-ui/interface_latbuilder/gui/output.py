import ipywidgets as widgets
from bqplot import Axis, LinearScale, Scatter, Figure
import numpy as np

from .common import style_default
from ..parse_output import Result

result = widgets.HTML(description='<b> Result: </b>', value='', layout=widgets.Layout(width='900px'), disabled=False)
# result2 = widgets.Textarea(description='Result2', value='', layout=widgets.Layout(width='900px'), disabled=False)

command_line_out = widgets.HTML(description='<b> Command line: </b>', layout=widgets.Layout(width='900px'), disabled=False, style=style_default)
result_obj = Result()
output = widgets.Tab(layout=widgets.Layout(display='none'))

def create_output(gui):
    result_obj = gui.output.result_obj
    sc_x = LinearScale(min=0., max=1.)
    sc_y = LinearScale(min=0., max=1.)

    scatt = Scatter(x=result_obj.getNet(0), 
                    y=result_obj.getNet(1), 
                    scales={'x': sc_x, 'y': sc_y},
                    default_size=20)
    ax_x = Axis(scale=sc_x, label='Coordinate 1')
    ax_y = Axis(scale=sc_y, label='Coordinate 2', orientation='vertical')

    fig = Figure(marks=[scatt], axes=[ax_x, ax_y], layout=widgets.Layout(width='650px', height='650px'))

    b1 = widgets.BoundedIntText(max=result_obj.getDim(), min=1, value=1, layout=widgets.Layout(width='150px'), description='x-axis')
    b2 = widgets.BoundedIntText(max=result_obj.getDim(), min=1, value=2, layout=widgets.Layout(width='150px'), description='y-axis')
    b3 = widgets.BoundedIntText(max=result_obj.getMaxLevel(), min=1, value=result_obj.getMaxLevel(), layout=widgets.Layout(width='150px'), description='Level')
    if result_obj.polynomial:
        b3.disabled = True
    def change_graph(b):
        if b['name'] == 'value':
            if b['owner'] == b1:
                scatt.x = result_obj.getNet(b['new']-1, b3.value)
                ax_x.label = 'Coordinate ' + str(b['new'])
            if b['owner'] == b2:
                scatt.y = result_obj.getNet(b['new']-1, b3.value)
                ax_y.label = 'Coordinate ' + str(b['new'])
            if b['owner'] == b3:
                scatt.x = result_obj.getNet(b1.value-1, b['new'])
                scatt.y = result_obj.getNet(b2.value-1, b['new'])
    b1.observe(change_graph)
    b2.observe(change_graph)
    b3.observe(change_graph)

    plot = widgets.HBox([fig, widgets.VBox([b1, b2, b3])], 
                        layout=widgets.Layout(align_items='center'))

    # output = widgets.Tab()

    if not result_obj.polynomial:
        code_C = widgets.Textarea(value='\
    int n = %i;\n\
    int s = %i;\n\
    int a[] = %s;\n\
    double points[n][s];\n\
    int i, j;\n\
    for (i = 0; i < n; i++)\n\
        for (j = 0; j < s; j++)\n\
            points[i][j] = ((long long)i * a[j]) %% n / (double)n;' \
    % (result_obj.lattice.size.points, result_obj.getDim(), str(result_obj.lattice.gen).replace('[', '{').replace(']', '}')),
            layout=widgets.Layout(width='600px', height='200px'))

        code_python = widgets.Textarea(value='\
    n = %i;\n\
    a = %s;\n\
    points = [[(i * aj %% n) / float(n) for aj in a] for i in range(n)]' \
    % (result_obj.lattice.size.points, str(result_obj.lattice.gen)),
            layout=widgets.Layout(width='600px', height='100px'))

        code_matlab = widgets.Textarea(value='\
    n = %i;\n\
    a = %s;\n\
    points = zeros(n,length(a));\n\
    for i = 1:n\n\
        points(i,:) = mod((i - 1) * a, n) / n;\n\
    end' \
    % (result_obj.lattice.size.points, str(result_obj.lattice.gen)),
            layout=widgets.Layout(width='600px', height='150px'))

    
        gui.output.output.children = [plot, code_C, code_python, code_matlab]
        gui.output.output.set_title(0, 'Plot')
        gui.output.output.set_title(1, 'C code')
        gui.output.output.set_title(2, 'Python code')
        gui.output.output.set_title(3, 'Matlab code')
    
    else:
        if result_obj.lattice.size.power == 1:
            modulus = result_obj.lattice.size.base
        else:
            modulus = np.flip((np.poly1d(np.array(np.flip(result_obj.lattice.size.base, axis=0)))**result_obj.lattice.size.power).c % 2, axis=0)
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
    points.append(list_in_coord)' % (str(modulus), str(result_obj.lattice.gen)),
            layout=widgets.Layout(width='600px', height='700px'))
        
        gui.output.output.children = [plot, code_python]
        gui.output.output.set_title(0, 'Plot')
        gui.output.output.set_title(1, 'Python code')
    
    gui.output.output.layout.display = 'flex'
