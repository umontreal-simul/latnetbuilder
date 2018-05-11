import ipywidgets as widgets
from bqplot import Axis, LinearScale, Scatter, Figure
import numpy as np
from jinja2 import Environment, PackageLoader

from .common import style_default
from ..parse_output import Result

env = Environment(
    loader=PackageLoader('web_ui', 'code_output'),
    autoescape=True
)

def transform_to_c(List):
    return str(List).replace('[', ' { ').replace(']', ' } ')

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
    if result_obj.search_type == 'polynomial':
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

    if result_obj.search_type == 'ordinary':
        template = env.get_template('ordinary_C.txt')
        code_C = widgets.Textarea(value= 
            template.render(n=result_obj.lattice.size.nb_points, s=result_obj.getDim(), a=transform_to_c(result_obj.lattice.gen)),
            layout=widgets.Layout(width='600px', height='200px'))

        template = env.get_template('ordinary_py.txt')
        code_python = widgets.Textarea(value= 
            template.render(n=result_obj.lattice.size.nb_points, a=result_obj.lattice.gen),
            layout=widgets.Layout(width='600px', height='100px'))

        template = env.get_template('ordinary_matlab.txt')
        code_matlab = widgets.Textarea(value= 
            template.render(n=result_obj.lattice.size.nb_points, a=result_obj.lattice.gen),
            layout=widgets.Layout(width='600px', height='130px'))

    
        gui.output.output.children = [plot, code_C, code_python, code_matlab]
        gui.output.output.set_title(0, 'Plot')
        gui.output.output.set_title(1, 'C code')
        gui.output.output.set_title(2, 'Python code')
        gui.output.output.set_title(3, 'Matlab code')

    # prim_polys = np.zeros((s,2),dtype=np.int)
    # with open("primitive_polynomials.csv") as poly_file:
    # reader = csv.DictReader(poly_file, fieldnames=["degree","representation"])
    # for i,row in enumerate(reader):
    #     prim_polys[i,0] = row["degree"]
    #     prim_polys[i, 1] = row["representation"]
    #     if i == s-1:
    #         break
    
    else:
        if result_obj.lattice.size.power == 1:
            modulus = result_obj.lattice.size.base
        else:
            modulus = np.flip((np.poly1d(np.array(np.flip(result_obj.lattice.size.base, axis=0)))**result_obj.lattice.size.power).c % 2, axis=0)

        template = env.get_template('polynomial_py.txt')
        code_python = widgets.Textarea(value= 
            template.render(mod=modulus, genvec=result_obj.lattice.gen.gen_vector),
            layout=widgets.Layout(width='600px', height='700px'))

        template = env.get_template('polynomial_Cpp.txt')
        code_cpp = widgets.Textarea(value= 
            template.render(mod=transform_to_c(modulus), genvec=transform_to_c(result_obj.lattice.gen.gen_vector)),
            layout=widgets.Layout(width='600px', height='700px'))
        
        gui.output.output.children = [plot, code_cpp, code_python]
        gui.output.output.set_title(0, 'Plot')
        gui.output.output.set_title(1, 'C++ code')
        gui.output.output.set_title(2, 'Python code')
    
    gui.output.output.layout.display = 'flex'
