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
        L"Amazon S3 client -- fetch from S3\nCopyright (C) 2008-2014, Felspar "
        L"Co. Ltd.")
(fostlib::ostream &o, fostlib::arguments &args) {
    if (args.size() < 3) {
        o << "s3get container remote-path local-path" << std::endl;
        return 1;
    }
    args.commandSwitch(
            "a", aws::s3::bucket::s_account_name.section(),
            aws::s3::bucket::s_account_name.name());
    // Create the bucket object
    aws::s3::bucket bucket(coerce<ascii_printable_string>(args[1]));
    if (bucket.get(
                coerce<boost::filesystem::wpath>(args[2]),
                coerce<boost::filesystem::wpath>(args[3]))
        == aws::s3::e_match) {
        o << "No download because the local file is already "
             "the same as the remote one"
          << std::endl;
    }
    return 0;
}
