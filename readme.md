Anope Database Convertor - XML
==============================

Intro
-----

This code will convert Anope v1.8.x databases to XML.

Requirements
------------

Tested to compile against Anope 1.9 branch commit 15817208aa. May compile against others.

Instructions
-----------

The code is, presently, a single .c file. It must be built against Anope. It has been tested to build against Anope svn. 

To do this, place db-convert-xml.c in src/tools/ inside your Anope source directory. Following this, run ./Config followed by make and finally make install. 

May need to edit the Makefile first on BSD. Look for all instances of db-convert and duplicate for db-convert-xml.

This will place db-convert-xml in the bin directory of your install path. Copy your .db files (versions 1.8 and 1.9 are supported) into the bin directory and run the db-convert.xml binary. 

On completion of this, your xml database should reside in a file named anope.xml.
