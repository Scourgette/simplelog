.. Simplelog documentation master file, created by

Simplelog
=========

.. toctree::
   :maxdepth: 2
   :caption: Contents:


* :ref:`genindex`
* :ref:`search`

Simplelog is a very fast, simple and modular log library for C and C++.
It is cross-plateform (Linux, Windows, Darwin, Android).
Simplelog also provides an assert framework allowing to customize error processing.

It will by default use the relevent logger depending on plateform (stdout, android log, ...).
Engine is also highly configurable to force one or several loggers, for everyone or for a particular log tag.
New loggers can also be implemented as separate projects.

Log formatters can be configured the same way, and custom implementations can be linked and then used
but simplelog engine.

That's what make it highly modular and evolutive. Projects specificities will never be an issue.


Getting started
===============

Get simplelog
-------------

Download from github: `Simplelog <https://github.com/Scourgette/simplelog>`_

Build simplelog
---------------

Simplelog is built using cmake. Your project should just depends on either of the following targets:

* simplelog::simplelog
* simplelog::simplelog_static

Which allows static or dynamic library to be respectively used.

.. code-block:: cmake

  cmake_minimum_required (VERSION 3.1)
  project (test-app)

  ADD_SUBDIRECTORY(simplelog)
  # Or instead of ADD_SUBDIRECTORY, you may fetch it through cmake:
  # FetchContent_Declare(
  #   simplelog
  #   GIT_REPOSITORY "https://github.com/Scourgette/simplelog.git"
  #   GIT_TAG "master")
  # FetchContent_MakeAvailable(simplelog)

  set(SRCS main.c)
  add_executable(test-app ${SRCS})
  target_link_libraries(test-app simplelog::simplelog)

Following build cmake options are available if specific configuration is needed at build time:

* SIMPLELOG_LOG_LEVEL
* SIMPLELOG_ASSERT_ENABLED
* SIMPLELOG_CONFIG_INI
* SIMPLELOG_MAX_LINE_LENGTH
* SIMPLELOG_ASYNCHRONOUS_MAX_QUEUE_SIZE

Start logging
-------------

.. doxygendefine:: SLOG_DECLARE_MODULE


Example
-------

TODO Download example


Log levels
==========

.. doxygendefine:: LOG_LEVEL

Static maximum log level
------------------------

During cmake build, SIMPLELOG_LOG_LEVEL config can be used to define the max log level.
Logs below that level won't be built, and thus won't induce any overhead during program
execution.

Dynamic maximum log level
-------------------------

.. doxygendefine:: SLOG_DEFAULT_LEVEL

Log macros
----------

**Note**

* When logging in C, printf-like format should be used (ie. "error: %s")
* When logging in CPP, `fmt <https://github.com/fmtlib/fmt>`_ format should be used (ie. "error: {}").
  This allows build time optimizations through templates using `fmt <https://github.com/fmtlib/fmt>`_ API.

.. doxygendefine:: SLOGV
.. doxygendefine:: SLOGD
.. doxygendefine:: SLOGI
.. doxygendefine:: SLOGW
.. doxygendefine:: SLOGE
.. doxygendefine:: SLOGP


Asserts
=======

Assert behaviour can be modified at build time using cmake configuration SIMPLELOG_ASSERT_ENABLED.
When disabling asserts, assert code is not built, and expressions in assert may be evaluated or ignored
depending on used macro:

.. doxygendefine:: SLOG_ASSERT
.. doxygendefine:: SLOG_FAIL
.. doxygendefine:: SLOG_WARN
.. doxygendefine:: SLOG_CHECK
.. doxygendefine:: SLOG_CHECKVAL
.. doxygendefine:: SLOG_RETURN



Configuration
=============

Configuration file
------------------

Simplelog can be configured using a .ini configuration file with the
following format:

.. code-block:: ini

  [GENERAL]
  # Asynchronous logging (default 0)
  Async = 1
  # Log formatter (builtins: Default|Null)
  Formatter = Default
  [LOGGERS]
  # Instanciate a "Stdout" logger named "Console"
  Console = Stdout
  # Instanciate a "File" logger with address "/tmp/logs.txt" and named "FileTmp"
  FileTmp = File:/tmp/logs.txt
  [LEVELS]
  # Default log level: verbose (show all logs by default)
  # Default loggers: Console and FileTmp -> all logs are written on those 2 loggers
  * = verbose,Console,FileTmp
  # Logs level for tag "MyTag" is debug -> ignore verbose logs for that tag
  MyTag = debug
  # Logs for tag "AnotherTag" will only be written on Console, FileTmp won't be impacted by those logs
  AnotherTag = Console

Note that loggers names, loggers types and tags are case insensitive.
It means the config "MyTag = debug,FileTmp" can be replaced with "mytag = DEBUG,fIlEtMp".

Simplelog will search for that file at several locations, in the following order:

* Path contained in SIMPLELOG_CONFIG_INI environment variable
* Path contained in SIMPLELOG_CONFIG_INI cmake build configuration
* ./simplelog.ini
* ~/.simplelog.ini
* /etc/simplelog.ini

If no config file is found, default configuration will be used:

* All compiled logs are displayed
* Logger depends on plateform (Android logcat on Android builds, else Stdout)
* Default formatter
* Synchronous logging

Configuration file location can also be initialized with the following API:

.. doxygendefine:: SLOG_CONFIG

Configuration through API
-------------------------

Simplelog can optionally be configured through its API:

.. doxygendefine:: SLOG_SET_ASYNC
.. doxygendefine:: SLOG_REGISTER_LOGGER
.. doxygendefine:: SLOG_DEFAULT_LOGGERS
.. doxygendefine:: SLOG_DEFAULT_LEVEL
.. doxygendefine:: SLOG_FORMATTER

Asynchronous logging
====================

By default logging is done synchronously.
It can also be configured with an asynchronous engine:

* A thread is dedicated for writing logs
* An optimized internal queue is used for caching
* Queue max size can be configured through cmake: SIMPLELOG_ASYNCHRONOUS_MAX_QUEUE_SIZE

