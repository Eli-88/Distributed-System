from glob import glob
from setuptools import setup
from pybind11.setup_helpers import Pybind11Extension

ext_modules = [
    Pybind11Extension(
        "cpp",
        sorted(glob("cpp/*.cpp")),
        extra_compile_args=["-std=c++20"],
    )
]

setup(ext_modules=ext_modules)
