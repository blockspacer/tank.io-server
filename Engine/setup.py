#!/usr/bin/env python

from distutils.core import setup
from distutils.extension import Extension

setup(name="PackageName",
    ext_modules=[
        Extension("py_connect", ["py-connect.cpp"],extra_compile_args=['-std=c++11'],
        libraries = ["boost_python"])
    ])