import pytest
import ctypes
from ctypes import CDLL
from typing import List, Union
import numpy as np
import torch

@pytest.fixture(scope="module")
def tlib():
    tlib = CDLL("./bin/tensor.so")

    class TensorFP32(ctypes.Structure): pass
    TensorFP32._fields_ = [
        ("size", ctypes.c_size_t),
        ("ndims", ctypes.c_size_t),
        ("dims", ctypes.POINTER(ctypes.c_size_t)),
        ("data", ctypes.POINTER(ctypes.c_float)),
        ("op", ctypes.c_int),
        ("gradient", ctypes.POINTER(TensorFP32)),
        ("children", ctypes.POINTER(ctypes.POINTER(TensorFP32))),
        ("requires_grad", ctypes.c_bool)
    ]

    TPOINTER = ctypes.POINTER(TensorFP32)
    tlib.init_tensor.restype = TPOINTER
    tlib.op_fp32conv2d.argtypes = [TPOINTER, TPOINTER, TPOINTER, ctypes.c_int, ctypes.c_int]
    tlib.op_fp32conv2d.restype = TPOINTER
    tlib.op_fp32sigmoid.restype = TPOINTER
    tlib.exp.restype = ctypes.c_double
    tlib.op_fp32avgpool2d.restype = TPOINTER
    tlib.op_fp32flatten.restype = TPOINTER
    tlib.op_fp32linear.argtypes = [TPOINTER, TPOINTER, TPOINTER]
    tlib.op_fp32linear.restype = TPOINTER
    tlib.op_fp32add.argtypes = [TPOINTER,TPOINTER]
    tlib.op_fp32add.restype = TPOINTER
    tlib.op_fp32sub.argtypes = [TPOINTER,TPOINTER]
    tlib.op_fp32sub.restype = TPOINTER

    tlib.op_fp32total.argtypes = [TPOINTER]
    tlib.op_fp32total.restype = TPOINTER

    tlib.backward.argtypes = [TPOINTER]

    return tlib

@pytest.fixture(scope="module")
def create_ctensor(tlib):
    def create(inp: Union[np.ndarray, torch.Tensor, List, torch.nn.Module], *shape, **kwargs):
        requires_grad = kwargs.pop("requires_grad", False)

        if isinstance(inp, torch.nn.Module):
            ret = {}
            for key, val in inp.state_dict().items():
                ret[key] = create(val, *val.shape, requires_grad=requires_grad)
            return ret

        if isinstance(inp, np.ndarray):
            inp = inp.reshape(-1).tolist()

        elif isinstance(inp, torch.Tensor):
            requires_grad = requires_grad or inp.requires_grad
            inp = inp.detach().numpy().reshape(-1).tolist()
            
        if not isinstance(inp, list):
            inp = [inp]

        size = np.prod(shape).item()
        Shape = ctypes.c_size_t * len(shape)
        Data = ctypes.c_float * size
        shap = Shape(*shape)
        data = Data(*inp)
        tensor = tlib.init_tensor(len(shape), shap, data)
        tensor.contents.requires_grad = requires_grad

        return tensor
    return create

@pytest.fixture(scope="module")
def check():
    def matches(a,b):
        # a must be a tlib.Tensor pointer
        # quick test to confirm
        assert a.contents

        # b can be a list, np array or pytorch Tensor
        if isinstance(b, torch.Tensor):
            b = b.detach().numpy().reshape(-1).tolist()
        if isinstance(b, np.ndarray):
            b = b.reshape(-1).tolist()

        assert a.contents.size == len(b)
        for i in range(len(b)):
            assert np.allclose(a.contents.data[i], b[i], atol=1e-6)

    return matches

@pytest.fixture(scope="module")
def check_network(check):
    def matches(a, b, check_gradient: bool=False):
        assert isinstance(a, dict), "Must be a dictionary of tensors"
        assert isinstance(b, torch.nn.Module), "Must be a module"
        for k, v in b.state_dict(keep_vars=True).items():
            check(a[k], v)
            if check_gradient:
                assert a[k].contents.gradient
                check(a[k].contents.gradient, v.grad)

    return matches
