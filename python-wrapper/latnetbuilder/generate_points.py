import numpy as np

def generate_points_ordinary_lattice(gen_vector, nb_points, coordinate=None):
    if coordinate is not None:
        return np.mod(gen_vector[coordinate] * np.arange(nb_points) / nb_points, 1)
    else:
        return np.mod(np.outer(np.arange(nb_points), gen_vector) / nb_points, 1)


def all_binary_vectors(m):
    arrays = [np.array([0, 1]) for i in range(m)]
    la = len(arrays)
    dtype = np.result_type(*arrays)
    arr = np.empty([len(a) for a in arrays] + [la], dtype=dtype)
    for i, a in enumerate(np.ix_(*arrays)):
        arr[...,i] = a
    return np.transpose(arr.reshape(-1, la))

def generate_points_digital_net(matrices, interlacing, coordinate=None, level=None):
    dim = len(matrices) // interlacing
    m = len(matrices[0])
    mult = np.logspace(-1, -(m-1)*interlacing - 1, num=m, endpoint=True, base=2)
    mult2 = np.logspace(0, -interlacing+1, num=interlacing, base=2)
    powers_of_2 = np.outer(mult2, mult)

    if level is not None:
        m = level
    
    if coordinate is None:
        binary_decomp_points = np.mod(np.dot(matrices[:,:,:m], all_binary_vectors(m)[::-1,:]), 2)
        points = np.zeros(shape=(2**m, dim))
        for coord in range(0, dim):
            projected_binary_decomp_points = binary_decomp_points[coord*interlacing : (coord+1)*interlacing]
            points[:, coord] = np.einsum('ijk,ij->k', projected_binary_decomp_points, powers_of_2)
        return points
    
    else:
        binary_decomp_points = np.mod(np.dot(matrices[coordinate*interlacing : (coordinate+1)*interlacing,:,:m], all_binary_vectors(m)[::-1,:]), 2)
        return np.einsum('ijk,ij->k', binary_decomp_points, powers_of_2)

