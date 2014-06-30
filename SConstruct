#!/usr/bin/python
#
# Build script for our cms.

# Imports
import os

# Test boost version
def CheckBoost(context, version):
    # Boost versions are in format major.minor.subminor
    v_arr = version.split(".")
    version_n = 0
    if len(v_arr) > 0:
            version_n += int(v_arr[0])*100000
    if len(v_arr) > 1:
            version_n += int(v_arr[1])*100
    if len(v_arr) > 2:
            version_n += int(v_arr[2])

    context.Message('Checking for Boost version >= %s... ' % (version))
    ret = context.TryCompile("""
#include <boost/version.hpp>

#if BOOST_VERSION < %d
#error Installed boost is too old!
#endif

int main() {
    return 0;
}
""" % version_n, '.cpp')
    context.Result(ret)
    return ret

# Environment
env = Environment()
try:
    env["CXX"] = os.environ["CXX"]
except:
    pass

# No need to configure when cleaning.
if not env.GetOption('clean'):

    # Configure, check if all dependencies exists.
    conf = Configure(env, custom_tests = {
        'CheckBoost': CheckBoost })

    # Flags we want anyway
    # -Werror removed
    #conf.env.Append(CCFLAGS = "-Wall -std=c++11 -fvisibility=hidden -fvisibility-inlines-hidden -DBOOST_SPIRIT_USE_PHOENIX_V3 -I../sqlpp11/include -I../sqlpp11-connector-postgresql/include")
    conf.env.Append(CCFLAGS = "-Wall -std=c++11 -DBOOST_SPIRIT_USE_PHOENIX_V3 -I../sqlpp11/include -I../sqlpp11-connector-postgresql/include")

    # Coverage reporting
    if ARGUMENTS.get('coverage', 0):
        conf.env.Append(CCFLAGS = " -fprofile-arcs -ftest-coverage")
        conf.env.Append(LIBS = "-lgcov")

    # Debug flags
    if ARGUMENTS.get('debug', 0):
        conf.env.Append(CCFLAGS = "-g")
    else:
        conf.env.Append(CCFLAGS = "-DBOOST_DISABLE_ASSERTS")
        conf.env.Append(LINKFLAGS = "-s")

    # Better view.
    conf.env.Append(CXXCOMSTR = "Compiling $TARGET")
    conf.env.Append(LINKCOMSTR = "Linking $TARGET")

    # check if the ldap library is available
    if not conf.CheckLib('pq', language='c++'):
        print "Libpq not installed"
        Exit(1)

    env = conf.Finish()

# Export environment
Export('env')

objs = []
#objs.append(env.Object(env.Glob('*.cpp')))
objs.append(env.SConscript(['src/SConscript']))

sqlpppostgresql = env.SharedLibrary(target='sqlpp-postgresql', source=objs)

