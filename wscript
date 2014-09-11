#!/usr/bin/env python
# coding: utf-8

APPNAME = 'latbuilder'
VERSION = 'undefined'

top = '.'
out = 'build'

from waflib.Configure import conf

@conf
def version_file(ctx):
    """Update the VERSION file by reading Git tags"""
    try:
        version = ctx.cmd_and_log("git describe --tags --match 'v[0-9].*'").strip()[1:]
        hashpos = version.rfind('-g')
        if hashpos > 0:
            version = version[:hashpos]
        ctx.path.make_node('VERSION').write(version)
    except:
        pass

@conf
def version(ctx):
    """Returns VERSION from environment or by reading the VERSION file"""
    if 'VERSION' in ctx.env:
        version = ctx.env['VERSION']
    else:
        verfile = ctx.path.find_node('VERSION')
        if verfile:
            version = verfile.read().strip()
        else:
            version = 'unknown'
    global VERSION
    VERSION = version
    return version

def options(ctx):
    ctx.load('compiler_cxx')
    ctx.load('gnu_dirs')
    ctx.add_option('--no-docs', action='store', help='do not build documentation')
    ctx.add_option('--boost', action='store', help='prefix under which Boost is installed')
    ctx.add_option('--fftw',  action='store', help='prefix under which FFTW is installed')

def configure(ctx):
    ctx.load('compiler_cxx')
    ctx.load('gnu_dirs')
    ctx.check(features='cxx', cxxflags='-std=c++11')
    ctx.env.append_unique('CXXFLAGS', ['-std=c++11', '-Wall'])

    def add_cxx_option(option):
        # suppress Boost ublas warnings
        if ctx.check(features='cxx', cxxflags=option, mandatory=False):
            ctx.env.append_value('CXXFLAGS', option)

    add_cxx_option('-Wno-unused-local-typedefs')
    add_cxx_option('-Wno-unused-function')

    ctx.version_file()

    def add_deps_path(what, where):
        try:
            prefix = ctx.root.find_node(where)
            ctx.env.append_value('INCLUDES', prefix.find_dir('include').abspath())
            ctx.env.append_value('LIBPATH',  prefix.find_dir('lib').abspath())
        except:
            ctx.fatal("cannot locate %s installation under `%s'" % (what, where))

    # options
    if ctx.options.boost:
        add_deps_path('Boost', ctx.options.boost)
        add_deps_path('FFTW', ctx.options.fftw)

    # Boost
    ctx.check(features='cxx cxxprogram', header_name='boost/program_options.hpp')
    #ctx.check(features='cxx cxxprogram', header_name='boost/chrono/chrono_io.hpp', lib='boost_system')
    #ctx.check(features='cxx cxxprogram', lib='boost_system')

    # FFTW
    ctx.check(features='cxx cxxprogram', header_name='fftw3.h')
    ctx.check(features='cxx cxxprogram', lib='fftw3')

    # Doxygen
    if not ctx.options.no_docs:
        ctx.find_program('doxygen', var='DOXYGEN', mandatory=False)

    # version
    ctx.define('LATBUILDER_VERSION', ctx.version())
    ctx.msg("Setting Lattice Builder version to", VERSION)

    # build variants
    env = ctx.env

    ctx.setenv('release')
    ctx.env.append_unique('CXXFLAGS', ['-O2'])
    ctx.define('NDEBUG', 1)

    ctx.setenv('debug', env.derive())
    ctx.env.append_unique('CXXFLAGS', ['-g'])
    ctx.define('DEBUG', 1)

    #ctx.msg("debug", ctx.all_envs['debug'])
    #ctx.msg("release", ctx.all_envs['release'])


#def dist(ctx):
#    ctx.algo = 'zip' # for Windows

def distclean(ctx):
    verfile = ctx.path.find_node('VERSION')
    if verfile:
        verfile.delete()
    from waflib import Scripting
    Scripting.distclean(ctx)

def build(ctx):
    ctx.recurse('latcommon')
    ctx.recurse('latbuilder')
    ctx.recurse('doc')
    ctx.recurse('examples')

    lc_inc_dir = ctx.path.find_dir('latcommon/include')
    lb_inc_dir = ctx.path.find_dir('latbuilder/include')
    src_dir = ctx.path.find_dir('src')

    prog = 'latbuilder-%s' % ctx.version()

    ctx(features='cxx cxxprogram',
            source=src_dir.ant_glob('*.cc'),
            includes=[lb_inc_dir, lc_inc_dir],
            lib=['boost_program_options', 'boost_chrono', 'boost_system', 'fftw3'],
            target=prog,
            use=['latbuilder', 'latcommon'],
            install_path=None)

    ctx.install_as('${BINDIR}/latbuilder', prog, chmod=755)
