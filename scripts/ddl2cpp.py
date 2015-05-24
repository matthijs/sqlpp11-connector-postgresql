#!/usr/bin/python
#
# Generate C++ structs for the tables.

import os
import sys
import psycopg2
import argparse

# Generate a argument parser
parser = argparse.ArgumentParser(description='Create C++ structs from a database table structure.')
parser.add_argument('-u', '--user', dest='user', required=True, help='PostgreSQL user')
parser.add_argument('--host', dest='host', required=True, help='PostgreSQL host')
parser.add_argument('-p', '--password', dest='password', required=True, help='PostgreSQL password')
parser.add_argument('-d', '--dbname', dest='dbname', required=True, help='PostgreSQL database')
parser.add_argument('-o', '--output-dir', dest='outputdir', help='Output directory', default='tables/')
parser.add_argument('-n', '--namespace', dest='namespace', help='C++ namespace', default='model')
args = parser.parse_args()

def _writeLine(fd, indent, line):
    fd.write(("\t" * indent) + line + "\n")

def _getIncludeGuard(namespace, table):
    return namespace.upper() + "_" + table.upper() + "_H"

# SQL types
types = {
    'tinyint': 'tinyint',
    'smallint': 'smallint',
    'integer': 'integer',
    'int': 'integer',
    'bigint': 'bigint',
    'char': 'char_',
    'varchar': 'varchar',
    'character varying': 'varchar',
    'text': 'text',
    'bool': 'boolean',
    'double': 'floating_point',
    'float': 'floating_point',
    'numeric': 'floating_point',

    # For now keep this a varchar
    'date': 'varchar',
    'time without time zone': 'varchar',
    'timestamp without time zone': 'varchar',
}

# Connect to the database and fetch information from the information_schema
# schema
conn = psycopg2.connect("host=" + args.host + " user=" + args.user + " password=" + args.password + " dbname=" + args.dbname)
curs = conn.cursor()

# First fetch all tables
curs.execute("""SELECT table_name FROM information_schema.tables WHERE table_schema = 'public'""")
tables = curs.fetchall()
for table in tables:
    fd = open(os.path.join(args.outputdir, table[0] + '.h'), 'w')
    _writeLine(fd, 0, "#ifndef " + _getIncludeGuard(args.namespace, table[0]))
    _writeLine(fd, 0, "#define " + _getIncludeGuard(args.namespace, table[0]))
    _writeLine(fd, 0, "")
    _writeLine(fd, 0, "#include <sqlpp11/table.h>")
    _writeLine(fd, 0, "#include <sqlpp11/char_sequence.h>")
    _writeLine(fd, 0, "#include <sqlpp11/column_types.h>")
    _writeLine(fd, 0, "")
    _writeLine(fd, 0, "namespace " + args.namespace + " {")
    _writeLine(fd, 0, "")
    _writeLine(fd, 1, "namespace " + table[0] + "_ {")

    # Fetch all columns for this table
    curs.execute("""SELECT * FROM information_schema.columns WHERE table_schema = 'public' AND table_name = '%s' ORDER BY table_name ASC, ordinal_position ASC""" % (table[0],))
    columns = curs.fetchall()
    for column in columns:
        _writeLine(fd, 0, "")
        _writeLine(fd, 2, "struct " + column[3].capitalize() + " {")
        _writeLine(fd, 3, "struct _alias_t {")
        _writeLine(fd, 4, "static constexpr const char _literal[] =\"" + column[3] + "\";")
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
        if column[5] and column[5].find("nextval") >= 0 and column[5].find(table[0] + "_" + column[3] + "_seq") >= 0:
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

    _writeLine(fd, 1, "}")
    _writeLine(fd, 0, "")

    # Columns are now written as C++ structs, now use them in the definition of
    # the table.
    _writeLine(fd, 1, "struct " + table[0] + " : sqlpp::table_t<" + table[0] + ",")
    if len(columns) == 1:
        _writeLine(fd, 4, table[0] + "_::" + columns[0][3].capitalize() + "> {")
    else:
        for column in columns[:-1]:
            _writeLine(fd, 4, table[0] + "_::" + column[3].capitalize() + ",")
        _writeLine(fd, 4, table[0] + "_::" + columns[-1][3].capitalize() + "> {")

    _writeLine(fd, 2, "using _value_type = sqlpp::no_value_t;")
    _writeLine(fd, 2, "struct _alias_t {")
    _writeLine(fd, 3, "static constexpr const char _literal[] = \"" + table[0] + "\";")
    _writeLine(fd, 3, "using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;")
    _writeLine(fd, 3, "template<typename T>")
    _writeLine(fd, 4, "struct _member_t {")
    _writeLine(fd, 5, "T " + table[0] + ";")
    _writeLine(fd, 5, "T &operator()() { return " + table[0] + "; }");
    _writeLine(fd, 5, "const T &operator()() const { return " + table[0] + "; }");
    _writeLine(fd, 4, "};")
    _writeLine(fd, 2, "};")

    _writeLine(fd, 1, "};")

    # end of namespace
    _writeLine(fd, 0, "}")
    _writeLine(fd, 0, "")
    _writeLine(fd, 0, "#endif")

    # Close file
    fd.close()

conn.close()

