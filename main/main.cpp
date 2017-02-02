// Fastcgi Container - framework for development of high performance FastCGI applications in C++
// Copyright (C) 2011 Ilya Golubtsov <golubtsov@yandex-team.ru> (Fastcgi Daemon)
// Copyright (C) 2015 Alexander Ponomarenko <contact@propulsion-analysis.com>

// This file is part of Fastcgi Container.
//
// Fastcgi Container is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License (LGPL) as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Fastcgi Container is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License (LGPL) for more details.
//
// You should have received a copy of the GNU Lesser General Public License (LGPL)
// along with Fastcgi Container. If not, see <http://www.gnu.org/licenses/>.

#include <csignal>
#include <cstdlib>
#include <iostream>
#include <stdexcept>

#include <unistd.h>

#include <errno.h>
#include <syslog.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/resource.h>

#include <pwd.h>
#include <grp.h>
#include <fcntl.h>

#include "fcgi_server.h"
#include "fastcgi3/config.h"
#include "fastcgi3/util.h"
#include "details/globals.h"

#ifdef HAVE_DMALLOC_H
#include <dmalloc.h>
#endif

fastcgi::FCGIServer *server;

uid_t
name_to_uid(const char *name) {
	if (!name) {
		return -1;
	}
	long const buflen = sysconf(_SC_GETPW_R_SIZE_MAX);
	if (buflen == -1) {
		return -1;
	}
	char buf[buflen];
	struct passwd pwbuf, *pwbufp;
	if (0 != getpwnam_r(name, &pwbuf, buf, buflen, &pwbufp) || !pwbufp) {
		return -1;
	}
	return pwbufp->pw_uid;
}

gid_t
name_to_gid(const char *name) {
	if (!name) {
		return -1;
	}
	long const buflen = sysconf(_SC_GETGR_R_SIZE_MAX);
	if (buflen == -1) {
		return -1;
	}
	char buf[buflen];
	struct group grbuf, *grbufp;
	if (0 != getgrnam_r(name, &grbuf, buf, buflen, &grbufp) || !grbufp) {
		return -1;
	}
	return grbufp->gr_gid;
}

bool
daemonize() {
	printf("Starting Fastcgi-Container as a daemon\n");

	struct rlimit rl;
	if (getrlimit(RLIMIT_NOFILE, &rl) < 0) {
		throw std::logic_error("Can’t get file limit");
	}

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

	if (rl.rlim_max == RLIM_INFINITY) {
		rl.rlim_max = 1024;
	}
	for (int i=0; i<rl.rlim_max; ++i) {
		close(i);
	}

	chdir("/");
	umask(0002);

	int fd0 = open("/dev/null", O_RDWR);
	int fd1 = dup(0);
	int fd2 = dup(0);

	openlog("fastcgi-container", LOG_CONS | LOG_PID, LOG_DAEMON);
	if (fd0 != 0 || fd1 != 1 || fd2 != 2) {
		syslog(LOG_ERR, "Unexpected file descriptors %d %d %d", fd0, fd1, fd2);
		return false;
	}

	syslog(LOG_INFO, "Fastcgi-Container has been started as a daemon");
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
		gid_t group = -1;
		for (int i = 1; i < argc; ++i) {
			if (!strcmp(argv[i], "--group") && ((i+1) < argc)) {
				group = name_to_gid(argv[i+1]);
				if (getegid()!=group) {
					if (setegid(group)<0) {
						printf("Could not change group to '%s'=%d\n", argv[i+1], group);
						return EXIT_FAILURE;
					}
					printf("Changed group to '%s'=%d\n", argv[i+1], group);
				}
				break;
			}
		}

		uid_t user = -1;
		for (int i = 1; i < argc; ++i) {
			if (!strcmp(argv[i], "--user") && ((i+1) < argc)) {
				user = name_to_uid(argv[i+1]);
				if (geteuid()!=user) {
					if (seteuid(user)<0) {
						printf("Could not change user to '%s'=%d\n", argv[i+1], user);
						return EXIT_FAILURE;
					}
					printf("Changed user to '%s'=%d\n", argv[i+1], user);
				}
				break;
			}
		}

		for (int i = 1; i < argc; ++i) {
			if (!strcmp(argv[i], "--daemon")) {
				if (!daemonize()) {
					printf("Could not run as daemon\n");
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
