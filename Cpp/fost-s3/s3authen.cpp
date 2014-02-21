/*
    Copyright 2009-2014, Felspar Co Ltd. http://fost.3.felspar.com/
    Distributed under the Boost Software License, Version 1.0.
    See accompanying file LICENSE_1_0.txt or copy at
        http://www.boost.org/LICENSE_1_0.txt
*/


#include "fost-aws.hpp"
#include <fost/s3.hpp>


using namespace fostlib;
using namespace fostlib::aws::s3;


void fostlib::aws::s3::rest_authentication(
    const string &account, const ascii_printable_string &bucket,
    http::user_agent::request &request
) {
    hmac signature(sha1, account_setting< string >(account, L"API secret"));
    signature << request.method() << "\n";

    if ( request.headers().exists("Content-MD5") )
        signature << request.headers()["Content-MD5"].value();
    signature << "\n";

    if ( request.headers().exists("Content-Type") )
        signature << request.headers()["Content-Type"].value();
    signature << "\n";

    signature << request.headers()["Date"].value() << "\n";

    signature << "/" << coerce< string >( bucket ) <<
        coerce< string >( request.address().pathspec().underlying() );

    request.headers().set("Authorization", L"AWS " +
        account_setting< string >(account, L"API key") + L":" +
        coerce< string >( coerce< base64_string >( signature.digest() ) ));
}

