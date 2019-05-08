/**
    Copyright 2014-2019, Felspar Co Ltd. <http://support.felspar.com/>

    Distributed under the Boost Software License, Version 1.0.
    See <http://www.boost.org/LICENSE_1_0.txt>
*/


#include <fost/aws>
#include <fost/cli>
#include <fost/main>
#include <fost/unicode>


FSL_MAIN(
        "s3get",
        "Amazon S3 client -- fetch from S3\nCopyright (C) 2008-2019, Felspar "
        "Co. Ltd.")
(fostlib::ostream &o, fostlib::arguments &args) {
    if (args.size() < 3) {
        o << "s3get container remote-path local-path" << std::endl;
        return 1;
    }
    args.commandSwitch(
            "a", fostlib::aws::s3::bucket::s_account_name.section(),
            fostlib::aws::s3::bucket::s_account_name.name());
    /// Create the bucket object
    fostlib::aws::s3::bucket bucket(
            fostlib::coerce<fostlib::ascii_printable_string>(args[1]));
    if (bucket.get(
                fostlib::coerce<fostlib::fs::path>(args[2]),
                fostlib::coerce<fostlib::fs::path>(args[3]))
        == fostlib::aws::s3::e_match) {
        o << "No download because the local file is already the same as the "
             "remote one"
          << std::endl;
    }
    return 0;
}
