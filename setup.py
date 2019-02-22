#!/usr/bin/env python
#-*- coding:utf-8 -*-

package = 'pylunar'
version = '0.1'

import sys
from distutils.core import setup
from distutils.extension import Extension

cmdclass = {}
if sys.version_info > (3,):
	source_files = ['jlunar.cpp', 'jlunarJQ.cpp', 'py3lunar.cpp']
else:
	source_files = ['jlunar.cpp', 'jlunarJQ.cpp', 'pylunar.cpp']
#安装
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
