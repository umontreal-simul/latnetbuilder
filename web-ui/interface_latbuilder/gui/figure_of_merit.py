import ipywidgets as widgets

from .common import style_default

figure_type = widgets.Dropdown(
    # options are (label, value) pairs
    options=['Palpha', 'Ralpha', 'Spectral'],
    value='Palpha', description='Figure:',
    layout=widgets.Layout(width='15%'), style=style_default)

alpha = widgets.Text(value='2', description=r'Value of \(\alpha\):',
                     layout=widgets.Layout(width='15%'), style=style_default)

evaluation_method = widgets.Checkbox(value=True, description='Coordinate-Uniform evaluation',
                                     style=style_default)

norm_type = widgets.Text(value='2', description=r'Norm type \(q>0\) or \(q\) = "inf":',
                         layout=widgets.Layout(width='24%'), style=style_default, disabled=True)

figure_of_merit_expl = widgets.HTMLMath(value='<p> Please note that: </p> \
                     <ul>\
                        <li> The Coordinate-Uniform method is generally faster, but requires \\(q = 2\\) </li> \
                        <li> The Spectral figure cannot be be evaluated by the Coordinate-Uniform method </li> \
                        <li> The \\(P_\\alpha \\) method requires \\( \\alpha \\in \\{ 2, 4, 6 \\} \\) </li> \
                     </ul>')

figure_of_merit_wrapper = widgets.Accordion([
    widgets.VBox([
        figure_of_merit_expl,
        widgets.HBox([figure_type, alpha, evaluation_method, norm_type],
                     layout=widgets.Layout(justify_content='space-between')
                     )])
])

figure_of_merit_wrapper.set_title(0, 'Figure of Merit')

# callback for change of figure_type
def change_figure_type(b, gui):
    if b['new'] == 'Spectral':
        gui.figure_of_merit.alpha.layout.display = 'none'
        gui.figure_of_merit.evaluation_method.disabled = True
        gui.figure_of_merit.evaluation_method.value = False
    elif b['new'] in ['Ralpha', 'Palpha']:
        gui.figure_of_merit.alpha.layout.display = 'flex'
        gui.figure_of_merit.evaluation_method.disabled = False
    elif b['new'] == 'R':
        gui.figure_of_merit.alpha.layout.display = 'none'
        gui.figure_of_merit.evaluation_method.disabled = False

def change_evaluation_method(b, gui):
    if b['new'] == True:
        gui.figure_of_merit.norm_type.disabled = True
        gui.figure_of_merit.norm_type.value = '2'
    elif b['new'] == False:
        gui.figure_of_merit.norm_type.disabled = False