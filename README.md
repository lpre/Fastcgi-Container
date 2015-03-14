# Fastcgi Container
Fastcgi Container is a framework for development of high performance FastCGI applications in C++.

Fastcgi Container is a branch of Yandex's <a href="https://github.com/golubtsov/Fastcgi-Daemon">Fastcgi Daemon</a>.

What's new compared to Fastcgi Daemon:

* The package is written in C++11 and does not use Boost libraries anymore 
* Support of request filters
* Support of servlets as an extensions of request handlers
* Support of sessions
* Support of authentication and authorization 
* The framework provides Page Compiler - a command-line C++ server page compiler which generates C++ servlets from <a href="http://en.wikipedia.org/wiki/JavaServer_Pages">JSP-like</a> source files 

# Requirements

* A C++11 compliant compiler with complete support of C++11 regex (e.g., GCC 4.9 meets the minimum feature set required to build the package)
* GNU build system (Autotools)
* Currently the framework can be built on Linux only

# Dependencies

* <a href="https://github.com/mnmlstc/core">MNMLSTC Core</a> - a C++11 library that adds several library features that are to be included in C++14 and beyond
* libfcgi
* libfcgi++
* libxml2
* libssl
* libcrypto
 
# License 

<a href="LICENSE">GPL v.3.0</a>
 
# Docs

* <a href="docs/API.md">API</a>
* <a href="page-compiler/docs/page_compiler.md">C++ Page Compiler</a>

# Examples

