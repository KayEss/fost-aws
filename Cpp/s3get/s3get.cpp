/*
    Copyright 2014, Felspar Co Ltd. http://support.felspar.com/
    Distributed under the Boost Software License, Version 1.0.
    See accompanying file LICENSE_1_0.txt or copy at
        http://www.boost.org/LICENSE_1_0.txt
*/


#include <fost/cli>
#include <fost/main>
#include <fost/unicode>
#include <fost/aws>


using namespace fostlib;


FSL_MAIN(
    L"s3get",
    L"Amazon S3 client -- fetch from S3\nCopyright (C) 2008-2014, Felspar Co. Ltd."
)( fostlib::ostream &o, fostlib::arguments &args ) {
    return 0;
}

