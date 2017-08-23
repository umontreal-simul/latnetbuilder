#!/usr/bin/env python
# coding: utf-8

top = '.'
out = 'build'

from waflib import Utils

import imp
def waftool(name):
    return imp.load_module('waf_' + name, *imp.find_module(name, ['./latcommon/waftools']))

version = waftool('version')
compiler = waftool('compiler')
deps = waftool('deps')

def options(ctx):
    ctx.recurse('latcommon')
    ctx.add_option('--fftw',  action='store', help='prefix under which FFTW is installed')
    ctx.add_option('--build-docs', action='store_true', default=False, help='build documentation')
    ctx.add_option('--build-examples', action='store_true', default=False, help='build examples (and tests them)')

def configure(ctx):
    ctx.recurse('latcommon')

    ctx.version_file()

    if ctx.options.fftw:
        deps.add_deps_path(ctx, 'FFTW', ctx.options.fftw)

    ctx_check = deps.shared_or_static(ctx, ctx.check)

    # realtime (required for Boost chrono on Linux)
    ctx.check(features='cxx cxxprogram',
            lib='rt',
            uselib_store='RT',
            mandatory=False)

    # Boost Program Options
    ctx_check(features='cxx cxxprogram',
            header_name='boost/program_options.hpp')
    ctx_check(features='cxx cxxprogram',
            lib='boost_program_options',
            uselib_store='PROGRAM_OPTIONS')
    # Boost Chrono
    ctx_check(features='cxx cxxprogram',
            header_name='boost/chrono/chrono_io.hpp',
            lib=['boost_chrono', 'boost_system'],
            shlib=ctx.env.LIB_RT,
            uselib_store='CHRONO',
            mandatory=False)

    # FFTW
    ctx_check(features='cxx cxxprogram', header_name='fftw3.h')
    ctx_check(features='cxx cxxprogram', lib='fftw3', uselib_store='FFTW')

    # NTL
    ctx_check(features='cxx cxxprogram',
            header_name='NTL/vector.h',
            lib=['ntl', 'gmp'],
            shlib=ctx.env.LIB_NTL,
            uselib_store='NTL',
            mandatory=True)

    # Doxygen
    if ctx.options.build_docs:
        ctx.env.BUILD_DOCS = True
        if not ctx.find_program('doxygen', var='DOXYGEN', mandatory=False):
            ctx.fatal('Doxygen is required for building documentation.\n' +
                      'Get it from http://www.stack.nl/~dimitri/doxygen/')

    # examples
    if ctx.options.build_examples:
        ctx.env.BUILD_EXAMPLES = True

    # version
    ctx.define('LATBUILDER_VERSION', ctx.set_version())
    ctx.msg("Setting Lattice Builder version", version.VERSION)

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
    ctx.recurse('latcommon')
    verfile = ctx.path.find_node('VERSION')
    if verfile:
        verfile.delete()
    from waflib import Scripting
    Scripting.distclean(ctx)


def build(ctx):

    if ctx.variant:
        print("Building variant `%s'" % (ctx.variant,))

    ctx.recurse('latcommon')
    ctx.recurse('src')
    ctx.recurse('progs')
    if ctx.env.BUILD_DOCS:
        ctx.recurse('doc')
    if ctx.env.BUILD_EXAMPLES:
        ctx.recurse('examples')
    #ctx.recurse('web-ui')


# build variants

from waflib.Build import BuildContext
class debug(BuildContext):
    cmd = 'debug'
    variant = 'debug'
