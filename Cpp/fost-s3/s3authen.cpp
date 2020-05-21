/**
    Copyright 2009-2020 Red Anchor Trading Co. Ltd.

    Distributed under the Boost Software License, Version 1.0.
    See <http://www.boost.org/LICENSE_1_0.txt>
 */


#include "fost-aws.hpp"
#include <fost/s3.hpp>


void fostlib::aws::s3::rest_authentication(
        const string &account,
        const ascii_printable_string &bucket,
        http::user_agent::request &request) {
    hmac signature(sha1, account_setting<string>(account, L"API secret"));

    signature << request.method() << "\n";

    if (request.headers().exists("Content-MD5")) {
        signature << request.headers()["Content-MD5"].value();
    }
    signature << "\n";

    if (request.headers().exists("Content-Type")) {
        signature << coerce<string>(request.headers()["Content-Type"]);
    }
    signature << "\n";

    signature << request.headers()["Date"].value() << "\n";

    signature << coerce<string>(request.address().pathspec().underlying());

    request.headers().set(
            "Authorization",
            "AWS " + account_setting<string>(account, L"API key") + ":"
                    + coerce<string>(coerce<base64_string>(signature.digest())));
}
