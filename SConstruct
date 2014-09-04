#!/usr/bin/python
#
# Scons script for sqlpp11-connector-postgresql

# Environment
env = Environment()
try:
    env["CXX"] = os.environ["CXX"]
except:
    pass

# No need to configure when cleaning.
if not env.GetOption('clean'):

    # Configure, check if all dependencies exists.
    conf = Configure(env)

    # Flags we want anyway
    conf.env.Append(CCFLAGS = "-Wall -Werror -std=c++11 -fvisibility=hidden -fvisibility-inlines-hidden -I../sqlpp11/include -I../sqlpp11-connector-postgresql/include")

    # Coverage reporting
    if ARGUMENTS.get('coverage', 0):
        conf.env.Append(CCFLAGS = " -fprofile-arcs -ftest-coverage")
        conf.env.Append(LIBS = "-lgcov")

    # Debug flags
    if ARGUMENTS.get('debug', 0):
        conf.env.Append(CCFLAGS = "-g")
    else:
        conf.env.Append(LINKFLAGS = "-s")

    # check if the ldap library is available
    if not conf.CheckLib('pq', language='c++'):
        print "Libpq not installed"
        Exit(1)

    env = conf.Finish()

# Export environment
Export('env')

objs = []
objs.append(env.SConscript(['src/SConscript']))

sqlpppostgresql = env.SharedLibrary(target='sqlpp-postgresql', source=objs)

