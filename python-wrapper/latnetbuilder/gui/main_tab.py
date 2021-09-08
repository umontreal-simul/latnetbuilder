import ipywidgets as widgets

def change_maintab(change, gui):
    '''Callback when the selected index of the main_tab attribute of the GUI changes'''
    gui.properties.size.value = ''

    if change['new'] == 0:   # new index = lattice
        gui.properties.main.set_title(0, 'Basic Lattice properties')
        gui.lattice_type.main.layout.display = 'flex'    
        gui.construction_method.main.layout.display = 'none'
        gui.properties.size.placeholder = 'e.g. 2^10 or 1024'
        gui.exploration_method.exploration_choice.options = [
            ('Evaluate/Extend', 'evaluation'), ('All space', 'exhaustive'),
            ('Korobov', 'Korobov'), ('CBC', 'full-CBC'), ('Fast CBC', 'fast-CBC')]
        gui.exploration_method.exploration_choice.value = 'fast-CBC'
        gui.exploration_method.is_random.disabled = False
        gui.figure_of_merit.coord_unif.disabled = False

        gui.figure_of_merit.is_normalization.layout.display = 'flex'
        gui.figure_of_merit.low_pass_filter.layout.display = 'flex'

        # call the change_lattice_type callback with current value of gui.lattice_type.type_choice
        # in order to update the properties and figure of merit elements.
        gui.lattice_type._callbacks[gui.lattice_type.type_choice]({'name':'value', 'new':gui.lattice_type.type_choice.value}, gui)

    elif change['new'] == 1:   # new index = net
        gui.properties.main.set_title(0, 'Basic Net properties')
        gui.properties.interlacing.disabled = False
        gui.lattice_type.main.layout.display = 'none' 
        gui.construction_method.main.layout.display = 'flex'
        gui.properties.size.placeholder = 'e.g. 2^10'
        if gui.properties.interlacing.value == 1:
            gui.figure_of_merit.figure_type.options = [('Palpha', 'Palpha'), ('R', 'R'), ('t-value', 't-value'), ('proj dep t-value', 'projdep:t-value'), ('star discr t-value', 'projdep:t-value:starDisc'),  ('resolution-gap', 'projdep:resolution-gap')]
        gui.figure_of_merit.coord_unif.value = True
        gui.figure_of_merit.coord_unif.disabled = True
        gui.exploration_method.exploration_choice.options = [
            ('Evaluate', 'evaluation'), ('All space', 'exhaustive'), ('CBC', 'full-CBC')]
        gui.exploration_method.exploration_choice.value = 'full-CBC'

        gui.figure_of_merit.is_normalization.value = False
        gui.figure_of_merit.is_normalization.layout.display = 'none'
        gui.figure_of_merit.low_pass_filter.value = False
        gui.figure_of_merit.low_pass_filter.layout.display = 'none'

        # call the change_constr_choice callback with current value of gui.construction_method.construction_choice
        # in order to update the properties and figure of merit elements.
        gui.construction_method._callbacks[gui.construction_method.construction_choice]({'name':'value', 'new':gui.construction_method.construction_choice.value}, gui)
