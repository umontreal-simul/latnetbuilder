import ipywidgets as widgets

def change_lattype(b, gui):
    if b['name'] == 'selected_index' and b['new'] == 1:
        gui.properties.modulus_pretty.layout.display = 'flex'
        gui.properties.modulus.placeholder = 'e.g. 01^8 or 000000001'
        gui.construction_method.generating_vector.children[1].placeholder = 'e.g. 01^6 or 0000001'
        gui.figure_of_merit.figure_type.options = ['Palpha', 'R']
        gui.figure_of_merit.figure_of_merit_expl.value = '<p> Please note that: </p> \
                     <ul>\
                        <li> The Coordinate-Uniform method is generally faster, but requires \\(q = 2\\) </li> \
                        <li> The \\(P_\\alpha \\) method requires \\( \\alpha \\in \\{ 2, 4, 6 \\} \\) </li> \
                     </ul>'
    elif b['name'] == 'selected_index' and b['new'] == 0:
        # gui.properties.modulus.description = 'Modulus n='
        gui.properties.modulus_pretty.layout.display = 'none'
        gui.properties.modulus.placeholder = 'e.g. 2^10 or 1024'
        gui.construction_method.generating_vector.children[1].placeholder = 'e.g. 2^8 or 256'
        gui.figure_of_merit.figure_type.options = ['Palpha', 'Ralpha', 'Spectral']
        gui.figure_of_merit.figure_of_merit_expl.value = '<p> Please note that: </p> \
                     <ul>\
                        <li> The Coordinate-Uniform method is generally faster, but requires \\(q = 2\\) </li> \
                        <li> The Spectral figure cannot be be evaluated by the Coordinate-Uniform method </li> \
                        <li> The \\(P_\\alpha \\) method requires \\( \\alpha \\in \\{ 2, 4, 6 \\} \\) </li> \
                     </ul>'