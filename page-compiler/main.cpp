// Fastcgi Container - framework for development of high performance FastCGI applications in C++
// Copyright (C) 2015 Alexander Ponomarenko <contact@propulsion-analysis.com>

// This file is part of Fastcgi Container.
//
// Fastcgi Container is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Fastcgi Container is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Fastcgi Container. If not, see <http://www.gnu.org/licenses/>.

#include <cstdlib>
#include <string>
#include <cctype>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>

#include "page.h"
#include "page_reader.h"
#include "code_writer.h"
#include "utils.h"

std::stringstream factoryIncStream;
std::stringstream factoryStream;
std::vector<std::string> srcFiles;

std::string outputPath = "./";
std::string moduleName = "server_pages";
bool updateMakefile = false;
bool updateFactory = false;

void
displayCopyright() {
	printf(
		"\nC++ Server Page Compiler for FastCGI Container.\n"
		"Copyright (C) 2015,2016 Alexander Ponomarenko.\n\n"
	);
}

void
displayHelp() {
	printf(
		"The following command line options are supported:\n"
		"[-outputPath=<path>]\n"
		"[-moduleName=<name>]\n"
		"[-updateFactory=true|false]\n"
		"[-updateMakefile=true|false]\n"
		"<path>/*.cpsp\n"
		"<path>/page1.cpsp <path>/page2.cpsp ...\n\n"
	);
}

std::string
parse(const std::string& path, Page& page) {
	std::ifstream srcStream(path);
	PageReader pageReader(page, path);
	pageReader.parse(srcStream);

	std::string clazz = removeExtension(basename(path));
	clazz[0] = std::toupper(clazz[0]);
	return std::move(clazz);
}

void
write(const Page& page, const std::string& clazz) {
	std::string implFileName = outputPath + clazz + ".cpp";

	std::string headerPath = outputPath;
	std::string headerFileName = headerPath + clazz + ".h";

	std::unique_ptr<CodeWriter> codeWriter(new CodeWriter(page, clazz));

	std::ofstream implStream(implFileName);
	codeWriter->writeImpl(implStream, basename(headerFileName));

	std::ofstream headerStream(headerFileName);
	codeWriter->writeHeader(headerStream, headerFileName);

	codeWriter->writeFactory(factoryIncStream, factoryStream, basename(headerFileName));

	srcFiles.push_back(implFileName);
}

void
compile(const std::string& path) {
	Page page;
	std::string clazz = parse(path, page);
	write(page, clazz);
}

void
writeFactory() {
	std::ofstream factoryFileStream(outputPath+"factory.cpp");

	factoryFileStream << "#include \"fastcgi3/component_factory.h\"\n";
	factoryFileStream << factoryIncStream.str();
	factoryFileStream << "\n";

	factoryFileStream << "FCGIDAEMON_REGISTER_FACTORIES_BEGIN()\n";
	factoryFileStream << factoryStream.str();
	factoryFileStream << "FCGIDAEMON_REGISTER_FACTORIES_END()\n\n";
}

void
writeMakeFile() {
	std::ofstream makeFileStream(outputPath+"Makefile.am");

	makeFileStream << "pkglib_LTLIBRARIES = " << moduleName << ".la\n\n";
	makeFileStream << moduleName << "_la_LDFLAGS = -module\n";

	makeFileStream << moduleName << "_la_SOURCES = \\\n";
	for (auto& src : srcFiles) {
		makeFileStream << "\t" << src << " \\\n";
	}
	makeFileStream << "\tfactory.cpp\n\n";
}

int
main(int argc, char *argv[]) {

	displayCopyright();

	std::vector<std::string> pages;
	for (int i=1; i<argc; ++i) {
		std::string s = argv[i];
		if (s.at(0)!='-') {
			// path
			if (std::string::npos!=s.find('*')) {
				// This is wildcard template
				// Include all matching files
				// Currently the program supports the simpiest wildcard template:
				// <path>/*.<ext>
				std::vector<std::string> t;
				split(s, '*', t);
				std::string path = t[0];
				if (0==path.length()) {
					path = "./";
				} else {
					path = path.substr(0, path.length()-1);
				}
				std::vector<std::string> files;
				listFiles(path, t[1], files);
				for (auto& f : files) {
					pages.push_back(path+f);
				}
			} else {
				// Add file path
				pages.push_back(s);
			}

		} else if (std::string::npos!=s.find('=')) {
			// -name=value
			std::vector<std::string> p;
			split(s, '=', p);
			if (2==p.size()) {
				std::string name = p[0].substr(1);
				std::transform(name.begin(), name.end(), name.begin(), ::tolower);

				if ("outputpath"==name) {
					outputPath = p[1];
					createDirectories(outputPath);
					if ('/'!=outputPath[outputPath.length()-1]) {
						outputPath += '/';
					}

				} else if ("modulename"==name) {
					moduleName = p[1];

				} else if ("updatemakefile"==name || "makefile"==name) {
					std::string value = p[1];
					std::transform(value.begin(), value.end(), value.begin(), ::tolower);
					updateMakefile = "yes"==value || "true"==value || "1"==value;

				} else if ("updatefactory"==name || "factory"==name) {
					std::string value = p[1];
					std::transform(value.begin(), value.end(), value.begin(), ::tolower);
					updateFactory = "yes"==value || "true"==value || "1"==value;
				}
			}

		} else {
			// -name (without value)
			std::string name = s.substr(1);

			if ("updatemakefile"==name || "makefile"==name) {
				updateMakefile = true;
			} else if ("updatefactory"==name || "factory"==name) {
				updateFactory = true;
			}

		}
	}

	if (pages.empty()) {
		displayHelp();
		return EXIT_SUCCESS;
	}

	try {
		for (auto& page : pages) {
			compile(page);
		}
		if (updateFactory) {
			writeFactory();
		}
		if (updateMakefile) {
			writeMakeFile();
		}
		return EXIT_SUCCESS;
	} catch (const std::exception &e) {
		std::cerr << e.what() << std::endl;
		return EXIT_FAILURE;
	}
}



