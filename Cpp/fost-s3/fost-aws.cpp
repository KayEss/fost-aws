/*
    Copyright 2015, Felspar Co Ltd. http://support.felspar.com/
    Distributed under the Boost Software License, Version 1.0.
    See accompanying file LICENSE_1_0.txt or copy at
        http://www.boost.org/LICENSE_1_0.txt
*/


#include "fost-aws.hpp"


namespace {
    const fostlib::module c_fost_aws(fostlib::c_fost, "aws");
}


extern const fostlib::module fostlib::c_fost_aws_s3(c_fost_aws, "s3");

