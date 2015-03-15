# Fastcgi Container
Fastcgi Container is a framework for development of high performance FastCGI applications in C++.

Fastcgi Container is a branch of Yandex's [Fastcgi Daemon](<https://github.com/golubtsov/Fastcgi-Daemon).

What's new compared to Fastcgi Daemon:

* The package is written in C++11 and does not use Boost libraries anymore 
* Support of request filters
* Support of servlets as an extensions of request handlers
* Support of sessions
* Support of authentication and authorization 
* The framework provides Page Compiler - a command-line C++ server page compiler which generates C++ servlets from [JSP-like](http://en.wikipedia.org/wiki/JavaServer_Pages) source files 

Note that all filters (including authentication/authorization filter) are executed under the FastCGI role "RESPONDER" and do not require that the roles "FILTER" and "AUTHORIZER" are supported by FastCGI connector (e.g. corresponding modules for Apache HTTPD).

# License 

[GPL v.3.0](LICENSE)

# Requirements

* A C++11 compliant compiler with complete support of C++11 regex (e.g., GCC 4.9 meets the minimum feature set required to build the package)
* GNU build system (Autotools)
* Currently the framework can be built on Linux only

# Dependencies

* [MNMLSTC Core](https://github.com/mnmlstc/core) - a C++11 library that adds several library features that are to be included in C++14 and beyond
* libfcgi
* libfcgi++
* libxml2
* libssl
* libcrypto

# Build

The project is using GNU Autotools. To build it, execute the following commands in the root directory of the project:

	./autogen.sh
	./configure
	make
	sudo make install 


# Docs

* [API](docs/API.md)
* [C++ Page Compiler](page-compiler/docs/page_compiler.md)

# Examples

The following examples are provided:

* Simple request handler
* Simple request filter
* Simple servlet
* Authentication filter
* Simple athorization realm

## Build

All examples except C++ server page are built together with the Fastcgi Container. 
To re-build them separately, execute the following commands:

	cd ./example/pages
	make

To build the example C++ server page, execute the following commands:

	cd ./example/pages
	fastcgi3-page-compiler -updateFactory=true -updateMakefile=true *.cpsp
	./autogen.sh
	./configure
	make

## Configuring examples to run with HTTPD server 

Copy built example shared libraries from `example/.libs` and `example/page/.libs` as well as example configuration file `example/fastcgi.conf` into working directory (e.g. `~/tmp/fscgi`). Modify configuration file as appropriate.

Configure available HTTPD server to connect with Fastcgi Container. 

For example, to run examples via Unix socket using Apache modules `mod_poxy` and `mod_proxy_fcgi`, add the following entries into Apache configuration file:

	ProxyPass /myapp/ unix:///var/lib/apache2/fcgid/fastcgi3-example.sock|fcgi://localhost/
 
To run examples via tcp socket, add the following entries into Apache configuration file:

	ProxyPass /myapp/ fcgi://localhost:8080/

If you want to run examples in clustered environment, enable the Apache module `mod_proxy_balancer` and add the following entries into Apache configuration:

	ProxyPass /myapp/ balancer://myappcluster/ stickysession=JSESSIONID|jsessionid
	<Proxy balancer://myappcluster/>
    	BalancerMember fcgi://localhost:8080
    	BalancerMember fcgi://localhost:8081
	</Proxy>

For more information, see Apache documentation:
 
* [mod_proxy](http://httpd.apache.org/docs/2.4/mod/mod_proxy.html)
* [mod_proxy_fcgi](http://httpd.apache.org/docs/2.4/mod/mod_proxy_fcgi.html)
* [mod_proxy_balancer](http://httpd.apache.org/docs/2.4/mod/mod_proxy_balancer.html)
 
## Running examples

Start HTTPD server as appropriate (e.g. `sudo /etc/init.d/apache start`).

Start Fastcgi Container with exmple configuration:

	cd ~/tmp/fscgi
	sudo fastcgi3-container --config=./fastcgi.conf
 
Open any web browser and type the following address URL bar:

	http://127.0.0.1/myapp/servlet 

or 
	
	http://127.0.0.1/myapp/myapp/test?a1=22

 
	