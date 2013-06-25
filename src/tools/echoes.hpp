/*
 * log.hpp
 *
 *  Created on: 2013.06.25.
 *      Author: Baarnus
 */

#ifndef LOG_HPP_
#define LOG_HPP_

// If DEBUG this includes needed
#ifndef NDEBUG
#include <stdio.h>
#include <string>
#endif
// Echo functions
#ifndef NDEBUG
#define echo(s) fprintf(stdout, (std::string(s) + "\n").c_str()); fflush(stdout)
#define echof(s,...) fprintf(stdout, (std::string(s) + "\n").c_str(),__VA_ARGS__); fflush(stdout)
#define echor(e) fprintf(stderr, (std::string(e) + "\n").c_str()); fflush(stderr)
#define echorf(e,...) fprintf(stderr, (std::string(e) + "\n").c_str(), __VA_ARGS__); fflush(stderr)
#else
#define echo(s) void(0)
#define echof(s,...) void(0)
#define echor(e) void(0)
#define echorf(e,...) void(0)
#endif

#endif /* LOG_HPP_ */
