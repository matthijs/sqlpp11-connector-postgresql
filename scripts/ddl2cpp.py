#!/usr/bin/python
#
# Generate C++ structs for the tables.

import argparse
import errno
import os
import psycopg2
import re

# Generate a argument parser
parser = argparse.ArgumentParser(description='Create C++ structs from a database table structure.')
parser.add_argument('-u', '--user', dest='user', required=True, help='PostgreSQL user')
parser.add_argument('--host', dest='host', required=True, help='PostgreSQL host')
parser.add_argument('-p', '--password', dest='password', required=True, help='PostgreSQL password')
parser.add_argument('-d', '--dbname', dest='dbname', required=True, help='PostgreSQL database')
parser.add_argument('-o', '--output-dir', dest='outputdir', help='Output directory', default='tables/')
parser.add_argument('-n', '--namespace', dest='namespace', help='C++ namespace', default='model')
parser.add_argument('-s', '--schemaPattern', dest='schemaPattern', help='LIKE clause pattern for table schema', default='public')
parser.add_argument('-l', '--lowerCaseFileNames', dest='lowerCaseFileNames', help='true if filenames should be lowercase', default='false')
args = parser.parse_args()

def mkdir_p(path):
    try:
        os.makedirs(path)
    except OSError as exc:  # Python >2.5
        if exc.errno == errno.EEXIST and os.path.isdir(path):
            pass
        else:
            raise

def _getIncludeGuard(namespace, tableSchema, tableName):
    val = re.sub("[^A-Za-z0-9]+", "_", namespace + "_" + tableSchema + "_" + tableName + "_h")
    return val.upper()

def _writeLine(fd, indent, line):
    fd.write(("\t" * indent) + line + "\n")

# SQL types
types = {
    'tinyint': 'tinyint',
    'smallint': 'smallint',
    'integer': 'integer',
    'int': 'integer',
    'bigint': 'bigint',
    'char': 'char_',
    'character': 'char_',
    'varchar': 'varchar',
    'character varying': 'varchar',
    'text': 'text',
    'bool': 'boolean',
    'boolean': 'boolean',
    'double': 'floating_point',
    'double precision': 'floating_point',
    'float': 'floating_point',
    'numeric': 'floating_point',
    'decimal': 'floating_point',
    'real': 'floating_point',
    'smallserial': 'smallint',
    'serial': 'integer',
    'bigserial': 'bigint',

    # Json types (could be mapped to for example nohlmann/json)
    'json' : 'text',
    'jsonb' : 'text',

    # Map to the day/time point
    'date': 'day_point',
    'time without time zone': 'time_point',
    'time with time zone': 'time_point',
    'timestamp without time zone': 'time_point',
    'timestamp with time zone': 'time_point',
    'abstime': 'time_point',

    # More mappings to varchar
    'name': 'varchar',
    'oid': 'varchar',
    'ARRAY': 'varchar',
    'pg_node_tree': 'varchar',
    'xid': 'varchar',
    'regproc': 'varchar',
    'pg_lsn': 'varchar',
    'inet': 'varchar',
    'interval': 'varchar',
    'bytea': 'varchar',
    'anyarray': 'varchar',

    # User defined types, for now a varchar
    'USER-DEFINED': 'varchar',
}

nsList = args.namespace.split('::')

# Make output-dir if it doesn't exist
mkdir_p(args.outputdir)

# Connect to the database and fetch information from the information_schema
# schema
conn = psycopg2.connect("""host={0} user={1} password={2} dbname={3}""".format(args.host, args.user, args.password, args.dbname))
curs = conn.cursor()

# First fetch all tables
tableQuery = """SELECT table_schema, table_name FROM information_schema.tables WHERE table_schema LIKE '{0}' ORDER BY table_schema ASC""".format(args.schemaPattern)
#print tableQuery
curs.execute(tableQuery)
tables = curs.fetchall()
for table in tables:
    tableSchema = table[0]
    tableName = table[1]

    schemaDir = tableSchema
    tableFileName = tableName + ".h"
    if args.lowerCaseFileNames == "true":
        schemaDir = tableSchema.lower()
        tableFileName = tableFileName.lower()

    schemaDir = os.path.join(args.outputdir, schemaDir);
    mkdir_p(schemaDir)

    tableFileName = os.path.join(schemaDir, tableFileName)
    fd = open(tableFileName, 'w')
    _writeLine(fd, 0, "#ifndef " + _getIncludeGuard(args.namespace, tableSchema, tableName))
    _writeLine(fd, 0, "#define " + _getIncludeGuard(args.namespace, tableSchema, tableName))
    _writeLine(fd, 0, "")
    _writeLine(fd, 0, "")
    _writeLine(fd, 0, "#include <sqlpp11/table.h>")
    _writeLine(fd, 0, "#include <sqlpp11/char_sequence.h>")
    _writeLine(fd, 0, "#include <sqlpp11/column_types.h>")
    _writeLine(fd, 0, "")
    for ns in nsList:
        _writeLine(fd, 0, "namespace " + ns + " {")
    _writeLine(fd, 0, "")
    _writeLine(fd, 0, "namespace " + tableSchema + "_ {")

    tableColumnsNamespace = tableName + "_"
    _writeLine(fd, 1, "namespace " + tableColumnsNamespace + " {")

    # Fetch all columns for this table
    columnQuery = """SELECT * FROM information_schema.columns WHERE table_schema = '{0}' AND table_name = '{1}' ORDER BY table_name ASC, ordinal_position ASC""".format(tableSchema, tableName)
    #print columnQuery
    curs.execute(columnQuery)
    columns = curs.fetchall()
    for column in columns:
        _writeLine(fd, 0, "")
        _writeLine(fd, 2, "struct " + column[3].capitalize() + " {")
        _writeLine(fd, 3, "struct _alias_t {")
        _writeLine(fd, 4, 'static constexpr const char _literal[] = R"("{0}")";'.format(column[3]))
        _writeLine(fd, 4, "using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;")
        _writeLine(fd, 4, "template<typename T>")
        _writeLine(fd, 5, "struct _member_t {")
        _writeLine(fd, 6, "T " + column[3] + ";")
        _writeLine(fd, 6, "T &operator()() { return " + column[3] + "; }")
        _writeLine(fd, 6, "const T &operator()() const { return " + column[3] + "; }")
        _writeLine(fd, 5, "};")
        _writeLine(fd, 3, "};")

        # Build the traits
        traits = "using _traits = ::sqlpp::make_traits<::sqlpp::" + types[column[7]]

        # Check for an autoincrement value (check if nextval is available and we
        # have a sequence in the form of tablename_columnname_seq)
        if column[5] and column[5].find("nextval") >= 0 and column[5].find(tableName + "_" + column[3] + "_seq") >= 0:
            traits+= ", sqlpp::tag::must_not_insert, sqlpp::tag::must_not_update"

        # Is column not null and no default value?
        if column[6] == "NO" and not column[5]:
            traits+= ", sqlpp::tag::require_insert"

        # Field can be NULL
        if column[6] == "YES":
            traits+= ", sqlpp::tag::can_be_null"

        _writeLine(fd, 0, "")
        _writeLine(fd, 3, traits + ">;")
        _writeLine(fd, 2, "};")

    _writeLine(fd, 1, "} // namespace " + tableColumnsNamespace)
    _writeLine(fd, 0, "")

    # Columns are now written as C++ structs, now use them in the definition of
    # the table.
    _writeLine(fd, 1, "struct " + tableName + " : sqlpp::table_t<" + tableName + ",")
    if len(columns) == 1:
        _writeLine(fd, 4, tableColumnsNamespace + "::" + columns[0][3].capitalize() + "> {")
    else:
        for column in columns[:-1]:
            _writeLine(fd, 4, tableColumnsNamespace + "::" + column[3].capitalize() + ",")
        _writeLine(fd, 4, tableColumnsNamespace + "::" + columns[-1][3].capitalize() + "> {")

    _writeLine(fd, 2, "using _value_type = sqlpp::no_value_t;")
    _writeLine(fd, 2, "struct _alias_t {")
    _writeLine(fd, 3, 'static constexpr const char _literal[] = R"("{0}"."{1}")";'.format(tableSchema, tableName))
    _writeLine(fd, 3, "using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;")
    _writeLine(fd, 3, "template<typename T>")
    _writeLine(fd, 4, "struct _member_t {")
    _writeLine(fd, 5, "T " + tableName + ";")
    _writeLine(fd, 5, "T &operator()() { return " + tableName + "; }");
    _writeLine(fd, 5, "const T &operator()() const { return " + tableName + "; }");
    _writeLine(fd, 4, "};")
    _writeLine(fd, 2, "};")

    _writeLine(fd, 1, "};")

    # end of namespace
    _writeLine(fd, 0, "} // namespace " + tableSchema)
    for ns in reversed(nsList):
        _writeLine(fd, 0, "} // namespace " + ns)
    _writeLine(fd, 0, "")
    _writeLine(fd, 0, "#endif")

    # Close file
    fd.close()

conn.close()

