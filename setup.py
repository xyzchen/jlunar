#!env python
#-*- coding:utf-8 -*-

from distutils.core import setup
from distutils.extension import Extension

version = '0.1'

cmdclass = {}
source_files = ['jlunar.cpp', 'jlunarJQ.cpp', 'pylunar.cpp']

setup(
      name="pylunar",
      version="0.1",
      description="Python bindings for lunar",
      author="陈逸少",
      author_email="jmchxy@gmail.com",
      url="https://github.com/xyzchen/pylunar",
      download_url="https://github.com/xyzchen/pylunar/archive/master.zip",
      cmdclass=cmdclass,
      ext_modules=[Extension("pylunar", source_files)]
      )
