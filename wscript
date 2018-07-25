#!/usr/bin/env python
# coding: utf-8

top = '.'
out = 'build'

from waflib import Utils

import imp
def waftool(name):
    return imp.load_module('waf_' + name, *imp.find_module(name, ['./latticetester/waftools']))

version = waftool('version')
compiler = waftool('compiler')
deps = waftool('deps')

def options(ctx):
    ctx.recurse('latticetester')
    ctx.add_option('--boost', action='store', help='prefix under which Boost is installed')
    ctx.add_option('--fftw',  action='store', help='prefix under which FFTW is installed')
    ctx.add_option('--build-docs', action='store_true', default=False, help='build documentation')
    ctx.add_option('--build-examples', action='store_true', default=False, help='build examples (and tests them)')
    ctx.add_option('--build-conda', action='store_true', default=False, help='build conda package without embedding LatNetBuilder inside')
    ctx.add_option('--build-full-conda', action='store_true', default=False, help='build conda package, and embed LatNetBuilder inside')

def configure(ctx):
    ctx.options.nested = True

    ctx.recurse('latticetester')

    if ctx.options.fftw:
        deps.add_deps_path(ctx, 'FFTW', ctx.options.fftw)
    if ctx.options.boost:
        deps.add_deps_path(ctx, 'boost', ctx.options.boost)

    ctx_check = deps.shared_or_static(ctx, ctx.check)

    # realtime (required for Boost chrono on Linux)
    # ctx.check(features='cxx cxxprogram',
    #         lib='rt',
    #         uselib_store='RT',
    #         mandatory=False)

    # Boost Program Options
    ctx_check(features='cxx cxxprogram',
            header_name='boost/program_options.hpp')
    ctx_check(features='cxx cxxprogram',
            lib='boost_program_options',
            uselib_store='PROGRAM_OPTIONS')

    ctx_check(features='cxx cxxprogram',
            lib='boost_system',
            uselib_store='SYSTEM')
    ctx_check(features='cxx cxxprogram',
            header_name='boost/filesystem.hpp',
            lib='boost_system')
    ctx_check(features='cxx cxxprogram',
            lib='boost_filesystem',
            uselib_store='FILESYSTEM')
    
    # Boost Chrono
    # ctx_check(features='cxx cxxprogram',
    #         header_name='boost/chrono/chrono_io.hpp',
    #         lib=['boost_chrono', 'boost_system'],
    #         shlib=ctx.env.LIB_RT,
    #         uselib_store='CHRONO',
    #         mandatory=False)

    # FFTW
    ctx_check(features='cxx cxxprogram', header_name='fftw3.h')
    ctx_check(features='cxx cxxprogram', lib='fftw3', uselib_store='FFTW')

    # NTL
    # ctx_check(features='cxx cxxprogram',
    #         header_name='NTL/vector.h',
    #         lib=['ntl', 'gmp'],
    #         shlib=ctx.env.LIB_NTL,
    #         uselib_store='NTL',
    #         mandatory=True)

    # Doxygen
    if ctx.options.build_docs:
        ctx.env.BUILD_DOCS = True
        if not ctx.find_program('doxygen', var='DOXYGEN', mandatory=False):
            ctx.fatal('Doxygen is required for building documentation.\n' +
                      'Get it from http://www.stack.nl/~dimitri/doxygen/')

    if ctx.options.build_conda or ctx.options.build_full_conda:
        if not ctx.find_program('conda', var='CONDA', mandatory=False):
            ctx.fatal('Conda is required for building conda package.\n' +
                      'Get it from https://www.anaconda.com/download/')
        ctx.env.BUILD_CONDA = True
        if ctx.options.build_full_conda:
            ctx.env.EMBED_LATNET_CONDA = True
        else:
            ctx.env.EMBED_LATNET_CONDA = False

    # examples
    if ctx.options.build_examples:
        ctx.env.BUILD_EXAMPLES = True

    # version
    ctx.version_file('latnetbuilder')
    version_tag = ctx.set_version('latnetbuilder')
    ctx.define('LATNETBUILDER_VERSION', version_tag)
    ctx.msg("Setting LatNet Builder version", version_tag)

    # build variants
    env = ctx.env.derive()
    env.detach()

    # release (default)
    ctx.env.append_unique('CXXFLAGS', ['-O2'])
    ctx.define('NDEBUG', 1)

    ctx.setenv('debug', env)
    ctx.env.append_unique('CXXFLAGS', ['-g'])
    ctx.define('DEBUG', 1)


def distclean(ctx):
    ctx.recurse('latticetester')
    verfile = ctx.path.find_node('latnetbuilder-version.txt')
    if verfile:
        verfile.delete()
    from waflib import Scripting
    Scripting.distclean(ctx)


def build(ctx):

    if ctx.variant:
        print("Building variant `%s'" % (ctx.variant,))

    ctx.options.nested = True
    ctx.recurse('latticetester')

    ctx.recurse('src')
    ctx.recurse('progs')
    ctx.recurse('data')
    if ctx.env.BUILD_DOCS:
        ctx.recurse('doc')
    if ctx.env.BUILD_EXAMPLES:
        ctx.recurse('examples')
    if ctx.env.BUILD_CONDA: # must be done at the end to copy installed files to conda package
        ctx.recurse('python-wrapper')
    # jupyter notebook
    ctx.install_files("${PREFIX}/share/latnetbuilder", ["python-wrapper/notebooks/Interface.ipynb"])


# build variants

from waflib.Build import BuildContext
class debug(BuildContext):
    cmd = 'debug'
    variant = 'debug'
