# Page Compiler

This document is derived from documentation of <a href="http://pocoproject.org/docs/PageCompilerUserGuide.html">POCO PageCompiler</a>.<br> 

##Contents

* [Introduction](#0)
* [C++ Server Page Syntax](#1)
    * [Hidden Comment](#2)
    * [Implementation Declaration](#3)
    * [Header Declaration](#4)
    * [Expression](#5)
    * [Scriptlet](#6)
    * [Pre-Response Scriptlet](#7)
    * [Include Directives](#8)
    * [C++ Header Include Directive](#9)
    * [C++ Implementation Include Directive](#10)
    * [Component Name Directive](#11)
    * [Page Directive](#12)
        * [class](#13)
        * [namespace](#14)
        * [baseClass](#15)
        * [context](#16)
        * [ctorArg](#17)
        * [export](#18)
        * [contentType](#19)
        * [contentLanguage](#20)
        * [precondition](#21)
        * [path](#22)
    * [Implicit Objects](#23)
        * [request](#24)
        * [response](#25)
        * [responseStream](#26)
* [Invoking the Page Compiler](#27)


## <a name="0">Introduction</a>

PageCompiler is a command line tool that translates HTML files into C++ code, more precisely, subclasses of `fastcgi::Servlet`. 
The source files can contain special directives that allow embedding of C++ code. The syntax of these directives is based on the syntax 
used for Java Server Pages (JSP) and Active Server Pages (ASP). 

The following introductory sample shows the code for a simple page that displays the current date and time. 

	<%@ page class="TimeHandler" %>
	<%@ component name="TestServlet" %>
	<%!
    	#include <chrono>
	%>
	<%
		auto p = std::chrono::system_clock::now();
		auto t = std::chrono::system_clock::to_time_t(p);
	%>
	<html>
		<head>
			<title>Time Sample</title>
		</head>
		<body>
			<h1>Time Sample</h1>
			<p><%= std::ctime(&t) %></p>
		</body>
	</html>


Sending the above code to the page compiler will generate two files, a header file (`TimeHandler.h`) and an implementation file (`TimeHandler.cpp`). 
The files define a subclass of `fastcgi::Servlet` named `TimeHandler`. The generated `handleRequest` member function contains code 
to send the HTML code contained in the source file to the client, as well as the C++ code fragments found  in between the Scriptlet tags.  

## <a name="1">C++ Server Page Syntax</a>


The following special tags are supported in a C++ server page (CPSP) file. 

### <a name="2">Hidden Comment</a>

A hidden comment documents the CPSP file, but is not sent to the client. 

	<%-- comment --%>


### <a name="3">Implementation Declaration</a>

An implementation declaration is copied to the implementation file immediately after the block containing the standard `#include` directives. 
It is used to include additional header files and `using` declarations, as well as to define functions and classes needed later on. 
The file may contain several implementation declaration sections.  

	<%!
		declaration
		...
	%>

Example:

	<%!
		#include <chrono>
		
	    bool isValid(int n) {
			// Code here
			// ...
			return true;
		}
		// ...
	%>


### <a name="4">Header Declaration</a>

A header declaration is copied to the header file immediately after the block containing the standard `#include` directives. 
It can be used to include the header file containing the definition of the base class for the request handler (if a custom base class is required), 
or member functions and/or variables of the servlet class, which will be declared as a private members of the class.

The file may contain several header declaration sections.

	<%!!
    	declaration
		...
	%>

Example:

	<%!!
		#include "test.h"

		// Private member function
		int getNumber() {
			// Code here
			// ...
			return 0;
		}
		...
	%>

### <a name="5">Expression</a>

The result of any valid C++ expression can be directly inserted into the page, provided the result can be written to an output stream. 
Note that the expression must not end with a semicolon. 

	<%= expression %>

### <a name="6">Scriptlet</a>

Arbitrary C++ code fragments can be included using the Scriptlet directive. 

	<%
    	statement
    	...
	%>

Example:

	<%
    	auto now = std::chrono::system_clock::now();
    	std::string dt(DateTimeFormatter::format(now, DateTimeFormat::SORTABLE_FORMAT));
	%>

### <a name="7">Pre-Response Scriptlet</a>

This is similar to an ordinary scriptlet, except that it will be executed before the HTTP response is sent. This can be used to manipulate the HTTP response object. 

	<%%
    	statement
    	...
	%>

The main feature of this directive is that it allows to send a redirect response to the client if a certain condition is true. 

Example: 

	<%%
    	if (!loggedIn) {
    		return response.redirectToPath("/login");
    	}
	%>

### <a name="8">Include Directives</a>

Another CPSP file can be included into the current file using the Include Directive. 

	<%@ include page="path" %>

Another URL handler can be included into the current page using the Include Directive. 

	<%@ include path="/url/path" %>

Another component handler can be included into the current page using the Include Directive. 

	<%@ include component="component_name" %>

###<a name="9">C++ Header Include Directive</a>

Include a C++ header file in the generated header file. 

	<%@ header include="path" %>

This corresponds to: 

	<%!! #include "path" %>

A variant of this directive is: 

	<%@ header sinclude="path" %>

This corresponds to: 

	<%!! #include <path>" %>

Examples:

	<%@ header sinclude="string"
			    sinclude="chrono"
			    include="test.h"
	%>


### <a name="10">C++ Implementation Include Directive</a>

Include a C++ header file in the generated implementation file. 

	<%@ impl include="path" %>

This corresponds to: 

	<%! #include "path" %>

A variant of this directive is: 

	<%@ impl sinclude="path" %>

This corresponds to: 

	<%! #include <path> %>

Examples:

	<%@ impl sinclude="string"
		     sinclude="chrono"
		     include="test.h"
	%>


### <a name="11">Component Name Directive</a>

The Component Name Directive allows the definition of FastCGI component name. 

	<%@ component name="value" %>

Exampe:

	<%@ page class="TimeHandler" %>
	<%@ component name="TestServlet" %>

This will define the component in the class `factory.cpp` as follows:

	FCGIDAEMON_REGISTER_FACTORIES_BEGIN()
	FCGIDAEMON_ADD_DEFAULT_FACTORY("TestServlet", TimeHandler)
	FCGIDAEMON_REGISTER_FACTORIES_END()


### <a name="12">Page Directive</a>

The Page Directive allows the definition of attributes that control various aspects of C++ code generation. 

	<%@ page attr="value" ... %>

The following page attributes are supported: 

#### <a name="13">class</a>

Specifies the name of the generated class. Defaults to the base name of the source file with the word `"Handler"` appended. 

#### <a name="14">namespace</a>

If specified, sets the namespace where the generated classes will be in. No namespace will be used if omitted. 

#### <a name="15">baseClass</a>

Specifies the name of the class used as the base class for the generated  request handler class. 
Defaults to  `fastcgi::Servlet`. 
Do not forget to add a Header Declaration containing an `#include` directive for the header file containing the definition of that class, 
otherwise the generated code won't compile. 

#### <a name="16">context</a>

Allows passing of a context object to the request handler's constructor. The context object is stored in the request handler object and can 
be obtained by calling the context() object. 

The class of the context object must be specified. Cannot be used together with `ctorArg`. 

#### <a name="17">ctorArg</a>

Allows to specify the type of a single argument being passed to the constructor of the generated request handler class. 
Can only be used together with `baseClass`. The argument is passed on to the constructor of the base class, therefore, 
one of the  constructors of the base class must also accept a single argument of the specified type. 

Cannot be used together with `context`. 

#### <a name="18">export</a>

Allows to specify a DLL import/export directive that is being added to the request handler class definition. 
Useful for exporting a request handler class from a Windows DLL. 


#### <a name="19">contentType</a>

Allows you to specify the MIME content type for the page. Defaults to text/html.

#### <a name="20">contentLanguage</a>

Allows to specify a language tag (e.g., "en") that will be sent in the response Content-Language header if the client sends an `Accept-Language`  
header in the request.

#### <a name="21">precondition</a>

Allows to specify a C++ boolean expression which will be evaluated before processing of the page begins. If the expression evaluates to false, 
processing of the page is immediately terminated and no response is sent to the client. 

The expression can be a call to a member function defined in the handler base class. If that function returns false, it can send its own response to the client. 

Example:

	<%@ page precondition="checkCredentials(request, response)" %>

#### <a name="22">path</a>

Specify a server path for the page. If specified, the generated handler class will contain a public static `const std::string` member 
variable named `PATH` containing the specified path. 

Example:

	<%@ page path="/index.html" %>

###<a name="23">Implicit Objects</a>

The following objects are available in the handler code.

#### <a name="24">request</a>

The HTTP request object - raw pointer `fastcgi::HttpRequest*`.

#### <a name="25">response</a>

The HTTP response object - raw pointer `fastcgi::HttpResponse*`.

#### <a name="26">responseStream</a>

The output stream where the response body is written to.

##<a name="27">Invoking the Page Compiler</a>

The Page Compiler is invoked from the command line. The file names of the CPSP files to be compiled are specified as arguments.

A number of options control the code generation.

* `-outputPath=path` : define an output directory
* `-moduleName=name` : define a module name 
* `-updateFactory=true|false` or `-factory=true|false` : update (true) or not (false) the factory class 
* `-updateMakefile=true|false` or `-makefile=true|false` : update (true) or not (false) the make file 
* `path/*.cpsp` : define the path with input files 
* `path/page1.cpsp path/page2.cpsp ...` : define the list of input files

When updating make file and/or factory class, the list of files has to contain all C++ server pages which should be included into the component. 
