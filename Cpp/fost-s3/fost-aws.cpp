/**
    Copyright 2015-2019 Red Anchor Trading Co. Ltd.

    Distributed under the Boost Software License, Version 1.0.
    See <http://www.boost.org/LICENSE_1_0.txt>
 */


#include "fost-aws.hpp"


namespace {
    const fostlib::module c_fost_aws(fostlib::c_fost, "aws");
}


extern const fostlib::module fostlib::c_fost_aws_s3(c_fost_aws, "s3");
