import ipywidgets as widgets

def change_lattype(b, gui):
    gui.properties.modulus.value = ''

    if b['new'] == 1:   # polynomial
        gui.construction_method.main.layout.display = 'none'
        gui.properties.modulus_pretty.layout.display = 'flex'
        gui.properties.modulus.placeholder = 'e.g. 01^8 or 000000001'
        gui.properties.modulus.description = 'Modulus n='
        gui.properties.is_multilevel.description = 'Embedded'
        gui.exploration_method.generating_vector.children[1].placeholder = 'e.g. 01^6 or 0000001'
        gui.figure_of_merit.figure_type.options = ['Palpha', 'R']
        gui.figure_of_merit.figure_of_merit_expl.value = '<p> Please note that: </p> \
                     <ul>\
                        <li> The Coordinate-Uniform method is generally faster, but requires \\(q = 2\\) </li> \
                        <li> The \\(P_\\alpha \\) figure requires \\( \\alpha \\in \\{ 2, 4, 6 \\} \\) </li> \
                     </ul>'
        gui.filters.equidistribution_filter.layout.display = 'none'
        gui.filters.equidistribution_filter.value = False
        gui.exploration_method.exploration_choice.options = [
            ('Evaluate/Extend', 'explicit:'), ('All space', 'exhaustive'),
            ('Korobov', 'Korobov'), ('CBC', 'CBC'), ('Fast CBC', 'fast-CBC')]
        gui.exploration_method.exploration_choice.value = 'CBC'

        ### TEMPORARY
        gui.filters.is_normalization.disabled = False
        gui.filters.low_pass_filter.disabled = False
        gui.multi_level.mult_normalization.disabled = False
        gui.multi_level.mult_low_pass_filter.disabled = False

    elif b['new'] == 0:     # ordinary
        gui.construction_method.main.layout.display = 'none'
        gui.properties.modulus_pretty.layout.display = 'none'
        gui.properties.modulus.placeholder = 'e.g. 2^10 or 1024'
        gui.properties.modulus.description = 'Modulus n='
        gui.properties.is_multilevel.description = 'Embedded'
        gui.exploration_method.generating_vector.children[1].placeholder = 'e.g. 2^8 or 256'
        gui.figure_of_merit.figure_type.options = ['Palpha', 'Ralpha', 'Spectral']
        gui.figure_of_merit.figure_of_merit_expl.value = '<p> Please note that: </p> \
                     <ul>\
                        <li> The Coordinate-Uniform method is generally faster, but requires \\(q = 2\\) </li> \
                        <li> The Spectral figure cannot be be evaluated by the Coordinate-Uniform method </li> \
                        <li> The \\(P_\\alpha \\) figure requires \\( \\alpha \\in \\{ 2, 4, 6 \\} \\) </li> \
                     </ul>'
        gui.filters.equidistribution_filter.layout.display = 'none'
        gui.filters.equidistribution_filter.value = False
        gui.exploration_method.exploration_choice.options = [
            ('Evaluate/Extend', 'explicit:'), ('All space', 'exhaustive'),
            ('Korobov', 'Korobov'), ('CBC', 'CBC'), ('Fast CBC', 'fast-CBC')]
        gui.exploration_method.exploration_choice.value = 'CBC'

        ### TEMPORARY
        gui.filters.is_normalization.disabled = False
        gui.filters.low_pass_filter.disabled = False
        gui.multi_level.mult_normalization.disabled = False
        gui.multi_level.mult_low_pass_filter.disabled = False

    elif b['new'] == 2:       # digital
        gui.construction_method.main.layout.display = 'flex'
        gui.properties.modulus_pretty.layout.display = 'none'
        gui.properties.modulus.placeholder = 'e.g. 2^10 or 1024'
        gui.properties.modulus.description = 'Number of points n='
        gui.properties.is_multilevel.description = 'Sequence'
        gui.figure_of_merit.figure_type.options = ['Palpha', 't-value', 'resolution-gap']
        gui.figure_of_merit.figure_of_merit_expl.value = '<p> Please note that: </p> \
                <ul>\
                <li> The \\(P_\\alpha \\) figure can be evaluated by the Coordinate-Uniform method, which is generally faster, but requires \\(q = 2\\) </li> \
                <li> The \\(P_\\alpha \\) method requires \\( \\alpha \\in \\{ 2, 4, 6 \\} \\) </li> \
                </ul>'
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