import numpy as np

from .gui.common import _primPolyRaw

def generate_points_ordinary_lattice(gen_vector, nb_points, coordinate=None):
    if coordinate is not None:
        return np.mod(gen_vector[coordinate] * np.arange(nb_points) / nb_points, 1)
    else:
        return np.mod(np.outer(np.arange(nb_points), gen_vector) / nb_points, 1)


def generate_points_polynomial_lattice(modulus, gen_vector, interlacing, coord, level):
    matrices = generatingMatrices(modulus, gen_vector)
    return generate_points_digital_net(matrices, interlacing, coord, level)

def bin_array(num, m):
    """Convert a positive integer num into an m-bit bit vector"""
    return np.array(list(np.binary_repr(num).zfill(m))).astype(np.int8)

def sobol_generating_matrix(coord, init_numbers, m, prim_polys):
    C = np.eye(m,dtype = np.int)
    if coord==1:
        return C
    degree = prim_polys[coord-2,0]
    a = np.zeros(degree,dtype=np.int)
    a[:-1] = bin_array(prim_polys[coord-2,1],degree-1)
    a[-1] = 1
    exp = np.array([2<<k for k in range(degree)])
    a = a*exp
    assert len(init_numbers)==degree
    for k in range(min(degree,m)):
        assert init_numbers[k] < (2<<k)
        assert init_numbers[k] % 2 == 1
        C[:k, k] = bin_array(init_numbers[k],k+1)[:k]
    tmp = np.flipud(np.copy(init_numbers))
    for k in range(degree,m):
        new_num = np.bitwise_xor.reduce(tmp*a) ^ tmp[-1]
        tmp[1:]=tmp[:-1]
        tmp[0] = new_num
        C[:k,k] = bin_array(new_num,k+1)[:k]
    return C

def generate_points_sobol_net(dim, m, gen_vector, interlacing, coord, level):
    prim_polys = np.zeros((dim * interlacing, 2),dtype=np.int)
    polys_list = _primPolyRaw.split('\r\n')
    for i in range(dim * interlacing):
        prim_polys[i,0] = int(polys_list[i].split(',')[0])
        prim_polys[i,1] = int(polys_list[i].split(',')[1])
    matrices = np.array([sobol_generating_matrix(j, gen_vector[j-1], m, prim_polys) for j in range(1, dim*interlacing+1)])
    return generate_points_digital_net(matrices, interlacing, coord, level)

def expandSeries(P, h, m):
    expansion = []
    for l in range(1, 2*m):
        res = 1 if (m-l >=0 and m-l <= int(np.log2(h)) and (h >> (m-l)) & 1) else 0
        start = l-m if (l-m > 1) else 1
        for p in range(start, l):
            res = (res + expansion[p-1] * ((P >> (m-l+p)) & 1)) % 2
        expansion.append(res)
    return expansion

def generatingMatrices(modulus, genVector):
    matrices = []
    m = int(np.log2(modulus))
    for j in range(len(genVector)):
        matrix = np.zeros((m, m), dtype=np.int32)
        expansion = expandSeries(modulus, genVector[j], m)
        for i in range(m):
            for j in range(m):
                matrix[i][j] = expansion[i+j]
        matrices.append(matrix)
    return np.array(matrices)


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

