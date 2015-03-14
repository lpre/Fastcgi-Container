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

#ifndef UTILS_H_
#define UTILS_H_

std::string basename(const std::string& path);

std::string removeExtension(const std::string& filename);

std::string pathToFile(const std::string& path);

void split(const std::string& str, char c, std::vector<std::string> &list);

std::string replaceAll(std::string str, const std::string& toReplace, const std::string& replaceWith);

void createDirectories(const std::string& path);

void listFiles(const std::string& path, const std::string& ext, std::vector<std::string>& files);

#endif /* UTILS_H_ */
