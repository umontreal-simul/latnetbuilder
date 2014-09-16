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
    ctx.load('compiler_cxx gnu_dirs waf_unit_test')
    ctx.add_option('--link-static', action='store_true', help='statically link with Boost and FFTW')
    ctx.add_option('--build-docs', action='store_true', default=False, help='build documentation')
    ctx.add_option('--build-examples', action='store_true', default=False, help='build examples')
    ctx.add_option('--boost', action='store', help='prefix under which Boost is installed')
    ctx.add_option('--fftw',  action='store', help='prefix under which FFTW is installed')

def configure(ctx):
    ctx.load('compiler_cxx gnu_dirs waf_unit_test')
    ctx.check(features='cxx', cxxflags='-std=c++11')
    ctx.env.append_unique('CXXFLAGS', ['-std=c++11', '-Wall'])

    def add_cxx_option(option):
        # suppress Boost ublas warnings
        if ctx.check(features='cxx', cxxflags=option, mandatory=False):
            ctx.env.append_value('CXXFLAGS', option)

    add_cxx_option('-Wno-unused-local-typedefs')
    add_cxx_option('-Wno-unused-function')

    if ctx.options.link_static:
        #flags = ['-static', '-static-libgcc', '-static-libstdc++']
        flags = ['-static-libgcc', '-static-libstdc++']
        if ctx.check(features='cxx cxxprogram',
                linkflags=flags,
                mandatory=False):
            ctx.env.append_unique('LINKFLAGS', flags)

    ctx.version_file()

    def add_deps_path(what, where):
        try:
            prefix = ctx.root.find_node(where)
            ctx.env.append_value('INCLUDES', prefix.find_dir('include').abspath())
            ctx.env.append_value('LIBPATH',  prefix.find_dir('lib').abspath())
        except:
            ctx.fatal("cannot locate %s installation under `%s'" % (what, where))

    def st(f):
        if ctx.options.link_static:
            def f_static(*a, **kw):
                if 'lib' in kw:
                    kw['stlib'] = kw['lib']
                    del kw['lib']
                if 'shlib' in kw:
                    kw['lib'] = kw['shlib']
                    del kw['shlib']
                return f(*a, **kw)
            return f_static
        else:
            def f_shared(*a, **kw):
                if 'shlib' in kw:
                    kw['lib'] = kw.get('lib', []) + kw['shlib']
                    del kw['shlib']
                return f(*a, **kw)
            return f_shared

    # options
    if ctx.options.boost:
        add_deps_path('Boost', ctx.options.boost)
        add_deps_path('FFTW', ctx.options.fftw)

    # FFTW
    st(ctx.check)(features='cxx cxxprogram', header_name='fftw3.h')
    st(ctx.check)(features='cxx cxxprogram', lib='fftw3', uselib_store='FFTW')

    # Boost
    st(ctx.check)(features='cxx cxxprogram',
            header_name='boost/program_options.hpp')
    st(ctx.check)(features='cxx cxxprogram',
            lib='boost_program_options',
            uselib_store='PROGRAM_OPTIONS')
    st(ctx.check)(features='cxx cxxprogram',
            header_name='boost/chrono/chrono_io.hpp',
            lib=['boost_chrono', 'boost_system'],
            shlib=['rt'],
            uselib_store='CHRONO',
            mandatory=False)

    # Doxygen
    if ctx.options.build_docs:
        ctx.find_program('doxygen', var='DOXYGEN', mandatory=False)

    # examples
    if ctx.options.build_examples:
        ctx.env.BUILD_EXAMPLES = True

    # version
    ctx.define('LATBUILDER_VERSION', ctx.version())
    ctx.msg("Setting Lattice Builder version to", VERSION)

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
    verfile = ctx.path.find_node('VERSION')
    if verfile:
        verfile.delete()
    from waflib import Scripting
    Scripting.distclean(ctx)


def build(ctx):

    if ctx.variant:
        print("Building variant `%s'" % (ctx.variant,))

    ctx.recurse('latcommon')
    ctx.recurse('latbuilder')
    ctx.recurse('doc')
    if ctx.env.BUILD_EXAMPLES:
        ctx.recurse('examples')

    lc_inc_dir = ctx.path.find_dir('latcommon/include')
    lb_inc_dir = ctx.path.find_dir('latbuilder/include')
    src_dir = ctx.path.find_dir('src')

    prog = 'latbuilder-%s' % ctx.version()

    ctx(features='cxx cxxprogram',
            source=src_dir.ant_glob('*.cc'),
            includes=[lb_inc_dir, lc_inc_dir],
            lib=ctx.env.LIB_FFTW + ctx.env.LIB_PROGRAM_OPTIONS + ctx.env.LIB_CHRONO,
            stlib=ctx.env.STLIB_FFTW + ctx.env.STLIB_PROGRAM_OPTIONS + ctx.env.STLIB_CHRONO,
            target=prog,
            use=['latbuilder', 'latcommon'],
            install_path=None)

    ctx.install_as('${BINDIR}/latbuilder', prog, chmod=755)


# build variants

from waflib.Build import BuildContext
class debug(BuildContext):
    cmd = 'debug'
    variant = 'debug'