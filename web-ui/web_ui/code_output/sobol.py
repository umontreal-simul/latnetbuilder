import csv
import numpy as np

prim_polys = np.zeros((21201,2),dtype=np.int)

def bin_array(num, m):
    """Convert a positive integer num into an m-bit bit vector"""
    return np.array(list(np.binary_repr(num).zfill(m))).astype(np.int8)

with open("primitive_polynomials.csv") as poly_file:
    reader = csv.DictReader(poly_file, fieldnames=["degree","representation"])
    for i,row in enumerate(reader):
        prim_polys[i,0] = row["degree"]
        prim_polys[i, 1] = row["representation"]

def sobol_generating_matrix(coord,init_numbers,m):
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