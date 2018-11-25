/*
    Copyright 2009-2015, Felspar Co Ltd. http://support.felspar.com/
    Distributed under the Boost Software License, Version 1.0.
    See accompanying file LICENSE_1_0.txt or copy at
        http://www.boost.org/LICENSE_1_0.txt
*/


#ifdef FOST_OS_WINDOWS
#define FOST_AWS_DECLSPEC __declspec(dllexport)
#else
#define FOST_AWS_DECLSPEC
#endif


#include <fost/core>


namespace fostlib {
    extern const module c_fost_aws_s3;
}
