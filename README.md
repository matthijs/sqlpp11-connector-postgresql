sqlpp11-connector-postgresql
============================

PostgreSQL connector for sqlpp11 library

Fork Info
===========

This is a fork of the sqlpp11-connector-postgresql repository.
The base version was not usable out-of-the-box. A series of things had to be fixed.
Since the project appeared to be abandoned for a while now, I decided to
circumvent communication overhead, just start working and see how far I'll come.

Currently I target VS2015, but MinGW and Linux will be next.

CMAKE is the only buildsystem generator I use so far, so I don't support others yet.

I try to get the code clean and ready for production.

I've no idea what Roland Bocks plans are for the future, but in case he wants
to continue his effort I try to support the project by reducing unnecessary hazzle for its users
as good as I can.

I'm currenty working for a company with legacy code juggling with tons of plain sql-strings.
Sqlpp11 appears to be a nice approach to make refactoring less risky in this case.

In case you intend to use this fork, please ask me to stop breaking things and for prioritizations.