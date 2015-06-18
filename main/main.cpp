// Fastcgi Container - framework for development of high performance FastCGI applications in C++
// Copyright (C) 2011 Ilya Golubtsov <golubtsov@yandex-team.ru> (Fastcgi Daemon)
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

#include <csignal>
#include <cstdlib>
#include <iostream>
#include <stdexcept>

#include <unistd.h>

#include <errno.h>

#include <sys/types.h>
#include <sys/stat.h>

#include "fcgi_server.h"
#include "fastcgi3/config.h"
#include "fastcgi3/util.h"
#include "details/globals.h"

#ifdef HAVE_DMALLOC_H
#include <dmalloc.h>
#endif

fastcgi::FCGIServer *server;

bool
daemonize() {
	/*
	 * Here are the steps to become a daemon:
	 *
	 * 1.	fork() so the parent can exit, this returns control to the command line or shell invoking your program.
	 * 		This step is required so that the new process is guaranteed not to be a process group leader.
	 * 		The next step, setsid(), fails if you're a process group leader.
	 *
	 * 2. 	setsid() to become a process group and session group leader.
	 * 		Since a controlling terminal is associated with a session, and this new session has not yet acquired a controlling
	 * 		terminal our process now has no controlling terminal, which is a Good Thing for daemons.
	 *
	 * 3.	fork() again so the parent, (the session group leader), can exit. This means that we, as a non-session group leader,
	 * 		can never regain a controlling terminal.
	 *
	 * 4.	chdir("/") to ensure that our process doesn't keep any directory in use. Failure to do this could make it so that
	 * 		an administrator couldn't unmount a filesystem, because it was our current directory.
	 * 		[Equivalently, we could change to any directory containing files important to the daemon's operation.]
	 *
	 * 5.	umask(0) so that we have complete control over the permissions of anything we write. We don't know what umask we
	 * 		may have inherited. [This step is optional]
	 *
	 * 6.	close() fds 0, 1, and 2. This releases the standard in, out, and error we inherited from our parent process.
	 * 		We have no way of knowing where these fds might have been redirected to. Note that many daemons use sysconf() to
	 * 		determine the limit _SC_OPEN_MAX. _SC_OPEN_MAX tells you the maximun open files/process. Then in a loop, the daemon
	 * 		can close all possible file descriptors. You have to decide if you need to do this or not. If you think that there
	 * 		might be file-descriptors open you should close them, since there's a limit on number of concurrent file descriptors.
	 *
	 * 7.	Establish new open descriptors for stdin, stdout and stderr. Even if you don't plan to use them, it is still a good idea
	 * 		to have them open. The precise handling of these is a matter of taste; if you have a logfile, for example, you might wish
	 * 		to open it as stdout or stderr, and open '/dev/null' as stdin; alternatively, you could open '/dev/console' as stderr and/or
	 * 		stdout, and '/dev/null' as stdin, or any other combination that makes sense for your particular daemon.
	 *
	 * From http://web.archive.org/web/20120914180018/http://www.steve.org.uk/Reference/Unix/faq_2.html#SEC16
	 */

	pid_t pid = fork();
	if (-1 == pid) {
		std::stringstream ss;
		ss << "Could not become a daemon: fork #1 failed: " << fastcgi::StringUtils::error(errno);
		throw std::logic_error(ss.str());
	}
	if (0 != pid) {
		_exit(0); // exit parent
	}

	pid_t sid = setsid();
	if (-1 == sid) {
		std::stringstream ss;
		ss << "Could not become a daemon: setsid failed: " << fastcgi::StringUtils::error(errno);
		throw std::logic_error(ss.str());
	}

	pid = fork();
	if (-1 == pid) {
		std::stringstream ss;
		ss << "Could not become a daemon: fork #2 failed: " << fastcgi::StringUtils::error(errno);
		throw std::logic_error(ss.str());
	}
	if (0 != pid) {
		_exit(0); // exit session leader
	}

	for (int i = getdtablesize(); i--; ) {
		close(i);
	}
	umask(0002); // disable: S_IWOTH
	chdir("/");

	const char *devnullptr = "/dev/nullptr";
	stdin = fopen(devnullptr, "a+");
	if (stdin == nullptr) {
		return false;
	}
	stdout = fopen(devnullptr, "w");
	if (stdout == nullptr) {
		return false;
	}
	stderr = fopen(devnullptr, "w");
	if (stderr == nullptr) {
		return false;
	}
	return true;
}

void
signalHandler(int signo) {
	if ((SIGINT == signo || SIGTERM == signo) && nullptr != ::server) {
		::server->stop();
	}
}   

void
setUpSignalHandlers() {
	if (SIG_ERR == signal(SIGINT, signalHandler)) {
		throw std::runtime_error("Cannot set up SIGINT handler");
	} 
	if (SIG_ERR == signal(SIGTERM, signalHandler)) {
		throw std::runtime_error("Cannot set up SIGTERM handler");
	}
}

int
main(int argc, char *argv[]) {	
	using namespace fastcgi;
	try {
		for (int i = 1; i < argc; ++i) {
			if (!strcmp(argv[i], "--daemon")) {
				if (!daemonize()) {
					return EXIT_FAILURE;
				}
				break;
			}
		}

		std::unique_ptr<Config> config(Config::create(argc, argv));
		FCGIServer::writePid(*config);
		std::shared_ptr<Globals> globals(new Globals(config.get()));
		FCGIServer server(globals);
		::server = &server;
		setUpSignalHandlers();
		server.start();
		server.join();
		return EXIT_SUCCESS;
	} catch (const std::exception &e) {
		std::cerr << e.what() << std::endl;
		return EXIT_FAILURE;
	}
}
