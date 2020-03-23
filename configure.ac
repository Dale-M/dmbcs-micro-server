#  dmbcs-micro-server    A C++ library providing CGI or built-in
#                        web server functions
#  
#  Copyright (C) 2018  DM Bespoke Computer Solutions Ltd
#  
#  This program is free software: you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation, either version 3 of the License, or (at
#  your option) any later version.
#  
#  This program is distributed in the hope that it will be useful, but
#  WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
#  General Public License for more details.
#  
#  You should have received a copy of the GNU General Public License
#  along with this program.  If not, see <http://www.gnu.org/licenses/>.


# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.69)
AC_INIT([dmbcs-micro-server], [0.3], [http://rdmp.org/dmbcs/contact])
AM_INIT_AUTOMAKE
AM_SILENT_RULES([yes])
AC_CONFIG_SRCDIR([DMBCS/micro-server/query-string.cc])
AC_CONFIG_MACRO_DIR([m4])

# Checks for programs.
AC_PROG_LIBTOOL
AC_PROG_CXX
AC_PROG_AWK
AC_PROG_CC
AC_PROG_CPP
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_MAKE_SET

# Checks for libraries.
PKG_CHECK_MODULES([CURLPP], [curlpp])
AC_SEARCH_LIBS([gcry_md_open], [gcrypt])

# Configure-time program parameters.
AC_ARG_WITH([mac],
            AS_HELP_STRING([--with-mac],
                           [The secret to create the URL MACs]),
            MAC=$withval,
            AC_MSG_WARN([using default MAC (use --with-mac to change)])
            MAC="theuas.")
        
AC_SUBST([MAC])

# Checks for header files.
AC_CHECK_HEADERS([netinet/in.h sys/socket.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_CHECK_HEADER_STDBOOL
AC_C_INLINE
AC_TYPE_SIZE_T
AC_TYPE_UINT16_T
AC_TYPE_UINT8_T

# Checks for library functions.
AC_CHECK_FUNCS([select socket strtol])

AC_CONFIG_FILES([DMBCS/micro-server/makefile
                 makefile
                 dmbcs-micro-server.pc])
AC_OUTPUT
