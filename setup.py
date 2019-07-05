#!/usr/bin/python
"""setup.py for PyRFA."""

from setuptools import setup
from setuptools.dist import Distribution
# To use a consistent encoding
from codecs import open
from os import path
import platform

here = path.abspath(path.dirname(__file__))

if platform.system() == 'Windows':
    packagefile = 'pyrfa.pyd'
else:
    packagefile = 'pyrfa.so'


class BinaryDistribution(Distribution):
    """For binary distribution."""

    def is_pure(self):
        """is_pure function."""
        return False


with open(path.join(here, 'README.md'), encoding='utf-8') as f:
    long_description = f.read()

setup(
    name='pyrfa',
    version='8.5.3',
    description='Python API for Refinitiv (Thomson Reuters) Enterprise Platform',
    long_description=long_description,
    long_description_content_type='text/markdown',
    url='https://github.com/devcartel/pyrfa',
    author='DevCartel',
    author_email='support@devcartel.com',
    license='MIT',
    classifiers=[
        'Development Status :: 5 - Production/Stable',
        'Intended Audience :: Developers',
        'Topic :: Software Development :: Build Tools',
        'License :: OSI Approved :: MIT License',
        'Programming Language :: Python :: 2',
        'Programming Language :: Python :: 2.6',
        'Programming Language :: Python :: 2.7',
        'Programming Language :: Python :: 3',
        'Programming Language :: Python :: 3.4',
        'Programming Language :: Python :: 3.5',
        'Programming Language :: Python :: 3.6',
        'Programming Language :: Python :: 3.7',
    ],
    keywords='refinitiv thomson reuters enterprise python rfa api feed elektron trep rmds rdfd ats market data',
    packages=['pyrfa'],
    package_data={
        'pyrfa': [packagefile, 'enumtype.def', 'RDMFieldDictionary'],
    },
    include_package_data=True,
    distclass=BinaryDistribution,
)
