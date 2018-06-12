import ipywidgets as widgets

def change_lattype(b, gui):
    gui.properties.modulus.value = ''

    if b['new'] == 0:   # lattice
        gui.lattice_type.main.layout.display = 'flex'    
        gui.construction_method.main.layout.display = 'none'      
        gui.properties.is_multilevel.description = 'Embedded'
        gui.filters.equidistribution_filter.layout.display = 'none'
        gui.filters.equidistribution_filter.value = False
        gui.exploration_method.exploration_choice.options = [
            ('Evaluate/Extend', 'explicit:'), ('All space', 'exhaustive'),
            ('Korobov', 'Korobov'), ('CBC', 'CBC'), ('Fast CBC', 'fast-CBC')]
        gui.exploration_method.exploration_choice.value = 'CBC'
        gui.figure_of_merit.coord_unif.disabled = False

        ### TEMPORARY
        gui.filters.is_normalization.disabled = False
        gui.filters.low_pass_filter.disabled = False
        gui.multi_level.mult_normalization.disabled = False
        gui.multi_level.mult_low_pass_filter.disabled = False

        gui.lattice_type._callbacks[gui.lattice_type.type_choice]({'name':'value', 'new':gui.lattice_type.type_choice.value}, gui)

    elif b['new'] == 1:       # net
        gui.lattice_type.main.layout.display = 'none' 
        gui.construction_method.main.layout.display = 'flex'
        gui.properties.modulus_pretty.layout.display = 'none'
        gui.properties.modulus.placeholder = 'e.g. 2^10 or 1024'
        gui.properties.modulus.description = 'Number of points n='
        gui.properties.is_multilevel.description = 'Sequence'
        gui.figure_of_merit.figure_type.options = ['Palpha', 'R', 't-value', 'resolution-gap']
        gui.figure_of_merit.figure_of_merit_expl.value = '<p> Please note that: </p> \
                <ul>\
                <li> The \\(P_\\alpha \\) and the R figures are evaluated by the Coordinate-Uniform method, which is generally faster, but requires \\(q = 2\\) </li> \
                <li> The \\(P_\\alpha \\) method requires \\( \\alpha \\in \\{ 2, 4, 6 \\} \\) </li> \
                </ul>'
        gui.figure_of_merit.coord_unif.value = True
        gui.figure_of_merit.coord_unif.disabled = True
        gui.filters.equidistribution_filter.layout.display = 'flex'
        gui.exploration_method.exploration_choice.options = [
            ('Evaluate', 'net-explicit:'), ('All space', 'exhaustive'), ('CBC', 'full-CBC')]
        gui.exploration_method.exploration_choice.value = 'full-CBC'

        ### TEMPORARY
        gui.filters.is_normalization.value = False
        gui.filters.is_normalization.disabled = True
        gui.filters.low_pass_filter.value = False
        gui.filters.low_pass_filter.disabled = True
        gui.multi_level.mult_normalization.value = False
        gui.multi_level.mult_normalization.disabled = True
        gui.multi_level.mult_low_pass_filter.value = False
        gui.multi_level.mult_low_pass_filter.disabled = True