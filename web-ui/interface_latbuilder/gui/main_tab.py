import ipywidgets as widgets

def build_tabs(gui):
    inside_tab = widgets.VBox([gui.properties.main, 
                    gui.multi_level.main,
                    gui.figure_of_merit.main, 
                    gui.weights.main, 
                    gui.construction_method.main, 
                    gui.filters.main,
                    gui.button_box.main,
                    gui.output.command_line_out,
                    gui.output.result, 
                    gui.button_box.display_button,
                    gui.output.my_output])

    main_tab = widgets.Tab([inside_tab, inside_tab])
    main_tab.set_title(0, 'Ordinary Lattice')
    main_tab.set_title(1, 'Polynomial Lattice')
    main_tab.observe(lambda b: change_lattype(b, gui))
    return (inside_tab, main_tab)

def change_lattype(b, gui):
    if b['name'] == 'selected_index' and b['new'] == 1:
        gui.properties.size.placeholder = 'e.g. 01^8 or 000000001'
        gui.construction_method.generating_vector.children[1].placeholder = 'e.g. 01^6 or 0000001'
        gui.figure_of_merit.figure_type.options = ['Palpha', 'R']
        gui.figure_of_merit.figure_of_merit_expl.value = '<p> Please note that: </p> \
                     <ul>\
                        <li> The Coordinate-Uniform method is generally faster, but requires \\(q = 2\\) </li> \
                        <li> The \\(P_\\alpha \\) method requires \\( \\alpha \\in \\{ 2, 4, 6 \\} \\) </li> \
                     </ul>'
    elif b['name'] == 'selected_index' and b['new'] == 0:
        gui.properties.size.description = 'Modulus n='
        gui.properties.size.placeholder = 'e.g. 2^10 or 1024'
        gui.construction_method.generating_vector.children[1].placeholder = 'e.g. 2^8 or 256'
        gui.figure_of_merit.figure_type.options = ['Palpha', 'Ralpha', 'Spectral']
        gui.figure_of_merit.figure_of_merit_expl.value = '<p> Please note that: </p> \
                     <ul>\
                        <li> The Coordinate-Uniform method is generally faster, but requires \\(q = 2\\) </li> \
                        <li> The Spectral figure cannot be be evaluated by the Coordinate-Uniform method </li> \
                        <li> The \\(P_\\alpha \\) method requires \\( \\alpha \\in \\{ 2, 4, 6 \\} \\) </li> \
                     </ul>'