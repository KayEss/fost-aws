/**
    Copyright 2008-2019, Felspar Co Ltd. <http://support.felspar.com/>

    Distributed under the Boost Software License, Version 1.0.
    See <http://www.boost.org/LICENSE_1_0.txt>
*/


#include <fost/aws>
#include <fost/cli>
#include <fost/main>
#include <fost/unicode>


FSL_MAIN(
        "s3put",
        "Amazon S3 client -- push to S3\nCopyright (C) 2008-2019, Felspar Co. "
        "Ltd.")
(fostlib::ostream &o, fostlib::arguments &args) {
    // Check we have the minimum number of command line arguments
    if (args.size() < 4) {
        o << "Must supply at least one file and a bucket name and an S3 "
             "location to put it to"
          << std::endl;
        return 1;
    }
    args.commandSwitch(
            "a", fostlib::aws::s3::bucket::s_account_name.section(),
            fostlib::aws::s3::bucket::s_account_name.name());
    fostlib::aws::s3::bucket bucket(
            fostlib::coerce<fostlib::ascii_printable_string>(args[2]));
    if (bucket.put(
                fostlib::coerce<fostlib::fs::path>(args[1].value()),
                fostlib::coerce<fostlib::fs::path>(args[3].value()))
        == fostlib::aws::s3::e_match) {
        o << "No upload performed because the remote file is already "
             "the same as the local one"
          << std::endl;
    }
    return 0;
}
