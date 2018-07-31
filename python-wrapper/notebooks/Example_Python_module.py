import latnetbuilder

search = latnetbuilder.SearchLattice()

search.modulus = '2^10'
search.construction = 'polynomial'
search.dimension = 5
search.exploration_method = 'fast-CBC'
search.figure_of_merit = 'CU:P2'
search.weights = ['product:1']

# the path to the output folder can be relative or absolute
search.execute(output_folder='test_latnetbuilder', stdout_filename='cpp_out.txt', stderr_filename='cpp_err.txt', display_progress_bar=False, delete_files=False)

print(search.points())