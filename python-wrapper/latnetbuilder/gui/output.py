import csv
import time
import ipywidgets as widgets
from matplotlib import pyplot as plt
import numpy as np
from jinja2 import Environment, PackageLoader

from .common import style_default, _primPolyRaw, BaseGUIElement
from ..parse_output import Result

env = Environment(
    loader=PackageLoader(__name__.split('.')[0], 'code_output'),    # first argument is package name
    autoescape=True
)

def transform_to_c(List):
    return str(List).replace('[', ' { ').replace(']', ' } ')

def create_output(output, create_graph=True, in_thread=False):
    output.output.layout.display = 'flex'
    result_obj = output.result_obj
    if create_graph and not ('polynomial' in result_obj.search_type and result_obj.getMaxLevel() > 15):

        b1 = widgets.BoundedIntText(max=result_obj.getDim(), min=1, value=1, layout=widgets.Layout(width='180px'), description='Coordinate on x-axis', style=style_default)
        b2 = widgets.BoundedIntText(max=result_obj.getDim(), min=1, value=2, layout=widgets.Layout(width='180px'), description='Coordinate on y-axis', style=style_default)
        b3 = widgets.BoundedIntText(max=min(result_obj.getMaxLevel(), 15), min=1, value=min(result_obj.getMaxLevel(), 15), layout=widgets.Layout(width='150px'), description='Level')
        
        if 'polynomial' in result_obj.search_type:
            b3.disabled = True

        pt_x = result_obj.getNet(0, b3.value)
        pt_y = result_obj.getNet(1, b3.value)
        fig = widgets.Output(layout=widgets.Layout(width='600px', height='500px'))
        # if in_thread:
        time.sleep(1)
        with fig:
            plt.figure(figsize=(8,8))
            plt.xlim(0, 1)
            plt.ylim(0, 1)
            plt.scatter(pt_x, pt_y, s=0.8)
            plt.show()

        plot = widgets.HBox([fig, widgets.VBox([b1, b2, b3, widgets.HTML('<p>Warning: the plot lags for 2^15 points and more. </p> <p>Level restricted to be less than 15.</p>')])], 
                layout=widgets.Layout(align_items='center'))

        def change_graph(change):
            if change['name'] == 'value':
                if change['owner'] == b1:
                    pt_x = result_obj.getNet(change['new']-1, b3.value)
                    pt_y = result_obj.getNet(b2.value-1, b3.value)
                if change['owner'] == b2:
                    pt_x = result_obj.getNet(b1.value-1, b3.value)
                    pt_y = result_obj.getNet(change['new']-1, b3.value)
                if change['owner'] == b3:
                    pt_x = result_obj.getNet(b1.value-1, change['new'])
                    pt_y = result_obj.getNet(b2.value-1, change['new'])
                fig.clear_output()
                with fig:
                    plt.figure(figsize=(8,8))
                    plt.xlim(0, 1)
                    plt.ylim(0, 1)
                    plt.scatter(pt_x, pt_y, s=0.8)
                    plt.show()
        b1.observe(change_graph)
        b2.observe(change_graph)
        b3.observe(change_graph)
        # 
    else:
        plot = widgets.HTML('No plot to show.')



    if result_obj.search_type == 'ordinary':
        template = env.get_template('ordinary_C.txt')
        code_C = widgets.Textarea(value= 
            template.render(n=result_obj.latnet.size.nb_points, s=result_obj.getDim(), a=transform_to_c(result_obj.latnet.gen)),
            layout=widgets.Layout(width='600px', height='200px'))

        template = env.get_template('ordinary_py.txt')
        code_python = widgets.Textarea(value= 
            template.render(n=result_obj.latnet.size.nb_points, a=result_obj.latnet.gen),
            layout=widgets.Layout(width='600px', height='100px'))

        template = env.get_template('ordinary_matlab.txt')
        code_matlab = widgets.Textarea(value= 
            template.render(n=result_obj.latnet.size.nb_points, a=result_obj.latnet.gen),
            layout=widgets.Layout(width='600px', height='130px'))

    
        output.output.children = [plot, code_C, code_python, code_matlab]
        output.output.set_title(0, 'Plot')
        output.output.set_title(1, 'C code')
        output.output.set_title(2, 'Python code')
        output.output.set_title(3, 'Matlab code')

    
    elif 'polynomial' in result_obj.search_type:
        if result_obj.latnet.size.power == 1:
            modulus = result_obj.latnet.size.base
        else:
            modulus = np.flip((np.poly1d(np.array(np.flip(result_obj.latnet.size.base, axis=0)))**result_obj.latnet.size.power).c % 2, axis=0)

        template = env.get_template('polynomial_py.txt')
        code_python = widgets.Textarea(value= 
            template.render(mod=modulus, genvec=result_obj.latnet.gen.gen_vector),
            layout=widgets.Layout(width='600px', height='700px'))

        template = env.get_template('polynomial_Cpp.txt')
        code_cpp = widgets.Textarea(value= 
            template.render(mod=transform_to_c(modulus), genvec=transform_to_c(result_obj.latnet.gen.gen_vector)),
            layout=widgets.Layout(width='600px', height='700px'))
        
        output.output.children = [plot, code_cpp, code_python]
        output.output.set_title(0, 'Plot')
        output.output.set_title(1, 'C++11 code')
        output.output.set_title(2, 'Python code')

    elif result_obj.search_type == 'digital-sobol':
        
        prim_polys = np.zeros((result_obj.getDim(),2),dtype=np.int)
        polys_list = _primPolyRaw.split('\r\n')
        for i in range(result_obj.getDim()):
            prim_polys[i,0] = int(polys_list[i].split(',')[0])
            prim_polys[i,1] = int(polys_list[i].split(',')[1])

        template = env.get_template('sobol_py.txt')
        code_python = widgets.Textarea(value= 
            template.render(s=result_obj.getDim(), 
                            m=len(result_obj.matrix(0)), 
                            init_numbers=result_obj.latnet.gen.gen_vector,
                            prim_polys=prim_polys.tolist()),   
            layout=widgets.Layout(width='600px', height='700px'))

        template = env.get_template('sobol_Cpp.txt')
        code_cpp = widgets.Textarea(value= 
            template.render(s=result_obj.getDim(), 
                            m=len(result_obj.matrix(0)),
                            genvec=transform_to_c(result_obj.latnet.gen.gen_vector),
                            degrees=str(prim_polys[:, 0].tolist()).strip('[]'),
                            representations=str(prim_polys[:, 1].tolist()).strip('[]')),
            layout=widgets.Layout(width='600px', height='700px'))
        
        output.output.children = [plot, code_cpp, code_python]
        output.output.set_title(0, 'Plot')
        output.output.set_title(1, 'C++11 code')
        output.output.set_title(2, 'Python code')

    elif 'digital-explicit' in result_obj.search_type:

        template = env.get_template('explicit_py.txt')
        code_python = widgets.Textarea(value= 
            template.render(matrices = str(result_obj.matrices).replace('array', '\n np.array')),   
            layout=widgets.Layout(width='600px', height='700px'))

        template = env.get_template('explicit_Cpp.txt')
        code_cpp = widgets.Textarea(value= 
            template.render(matrices = transform_to_c([m.tolist() for m in result_obj.matrices])),   
            layout=widgets.Layout(width='600px', height='700px'))

        output.output.children = [plot, code_cpp, code_python]
        output.output.set_title(0, 'Plot')
        output.output.set_title(1, 'C++11 code')
        output.output.set_title(2, 'Python code')
    


def output():
    result_html = widgets.HTML(description='<b> Result: </b>', value='', layout=widgets.Layout(width='900px'), disabled=False)
    # result2 = widgets.Textarea(description='Result2', value='', layout=widgets.Layout(width='900px'), disabled=False)

    command_line_out = widgets.HTML(description='<b> Command line: </b>', layout=widgets.Layout(width='900px'), disabled=False, style=style_default)
    result_obj = Result()
    output = widgets.Tab(layout=widgets.Layout(display='none'))
    return BaseGUIElement(result_html=result_html,
                          # result2=result2,
                          result_obj=result_obj,
                          output=output,
                          command_line_out=command_line_out)
