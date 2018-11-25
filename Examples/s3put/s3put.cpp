/*
    Copyright 2008-2014, Felspar Co Ltd. http://support.felspar.com/
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
        L"s3put",
        L"Amazon S3 client -- push to S3\nCopyright (C) 2008-2014, Felspar Co. "
        L"Ltd.")
(fostlib::ostream &o, fostlib::arguments &args) {
    // Check we have the minimum number of command line arguments
    if (args.size() < 4) {
        o << "Must supply at least one file and a bucket name and an S3 "
             "location to put it to"
          << std::endl;
        return 1;
    }
    args.commandSwitch(
            "a", aws::s3::bucket::s_account_name.section(),
            aws::s3::bucket::s_account_name.name());
    aws::s3::bucket bucket(coerce<ascii_printable_string>(args[2]));
    if (bucket.put(
                coerce<boost::filesystem::wpath>(args[1].value()),
                coerce<boost::filesystem::wpath>(args[3].value()))
        == aws::s3::e_match) {
        o << "No upload performed because the remote file is already "
             "the same as the local one"
          << std::endl;
    }
    return 0;
}
