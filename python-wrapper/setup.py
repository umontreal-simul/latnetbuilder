
# necessary to push to PyPI
# cf. http://peterdowns.com/posts/first-time-with-pypi.html
# cf. https://tom-christie.github.io/articles/pypi/
# cf. https://pythonhosted.org/setuptools/setuptools.html

# commands:
# python setup.py sdist upload -r testpypi
# python setup.py sdist upload -r pypi


from distutils.util import convert_path
from setuptools import setup, find_packages
import os


##################################################
module = 'latnetbuilder'
##################################################

# get version from __meta__
dir_path = os.path.dirname(os.path.realpath(__file__))
meta_ns = {}
path = convert_path(os.path.join(dir_path, module, '__meta__.py'))
with open(path) as meta_file:
    exec(meta_file.read(), meta_ns)


# read requirements.txt
with open(os.path.join(dir_path, 'requirements.txt'), 'r') as f:
    content = f.read()
li_req = content.split('\n')
install_requires = [e.strip() for e in li_req if len(e)]


name = 'latnetbuilder'
name_url = 'latnetbuilder'

packages = [module]
version = meta_ns['__version__']
description = 'Python Interface for LatNetBuilder'
long_description = 'See github repo README'
author = 'umontreal-simul'
author_email = 'pierre.marion@polytechnique.org'
# github template
url = 'https://github.com/{}/{}'.format(author,
                                        name_url)
download_url = 'https://github.com/{}/{}/tarball/{}'.format(author,
                                                            name_url,
                                                            version)
keywords = ['scientific computation', 'numerical integration', 'C++ backend', 'QMC', 'lattices', 'digital nets', 'Jupyter', 'LatNetBuilder']
license = 'Apache Software License, Version 2.0'
classifiers = ['Framework :: Jupyter',
               'Topic :: Scientific/Engineering :: Mathematics'
               'Development Status :: 5 - Stable',
               'License :: OSI Approved :: Apache Software License',
               'Programming Language :: Python :: 3.6',
               'Programming Language :: C++',
               'Intended Audience :: Financial and Insurance Industry',
               'Intended Audience :: Science/Research'
               ]
include_package_data = True
install_requires = install_requires
zip_safe = False


# ref https://packaging.python.org/tutorials/distributing-packages/
setup(
    name=name,
    version=version,
    packages=packages,
    author=author,
    author_email=author_email,
    description=description,
    long_description=long_description,
    url=url,
    download_url=download_url,
    keywords=keywords,
    license=license,
    classifiers=classifiers,
    include_package_data=include_package_data,
    install_requires=install_requires,
    zip_safe=zip_safe,
)
