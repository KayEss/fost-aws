/**
    Copyright 2009-2019, Felspar Co Ltd. <http://support.felspar.com/>

    Distributed under the Boost Software License, Version 1.0.
    See <http://www.boost.org/LICENSE_1_0.txt>
*/


#ifndef FOST_DETAIL_S3_HPP
#define FOST_DETAIL_S3_HPP
#pragma once


#include <fost/crypto>
#include <fost/http>


namespace fostlib {


    extern const module c_fost_aws_s3;


    namespace aws {


        namespace s3 {


            /// The actions that might be taken with S3 files
            typedef enum {
                /// The requested action was performed
                e_executed,
                /// The action was not performed because the files match
                e_match
            } outcome;

            /// Implements the authentication signature for requests
            void FOST_AWS_DECLSPEC rest_authentication(
                    const string &account,
                    const ascii_printable_string &bucket,
                    http::user_agent::request &request);


            /// Return the setting value of the requested type for the named
            /// account
            template<typename V>
            inline V account_setting(const string &account, wliteral name) {
                return setting<V>::value(L"S3 account/" + account, name);
            }
            /// Return the setting value of the requested type for the named
            /// account
            template<typename V>
            inline nullable<V> account_setting(
                    const string &account, wliteral name, t_null) {
                return setting<V>::value(L"S3 account/" + account, name, null);
            }


            /// File information available from S3
            class FOST_AWS_DECLSPEC file_info {
                boost::shared_ptr<http::user_agent::response> m_response;

              public:
                file_info(
                        const http::user_agent &,
                        const ascii_printable_string &bucket,
                        const boost::filesystem::wpath &);

                accessors<const boost::filesystem::wpath> path;

                bool exists() const;
                nullable<string> md5() const;
            };


            /// Represents a bucket on S3 or a container on Swift
            class FOST_AWS_DECLSPEC bucket {
                http::user_agent m_ua;
                /// Return the URL for the bucket for the requested location
                url uri(const boost::filesystem::wpath &location) const;

              public:
                /// Use to override the default account name
                static const setting<string> s_account_name;

                /// Construct a bucket connection
                bucket(const ascii_printable_string &name);

                /// The bucket name
                accessors<const ascii_printable_string> name;

                /// Return file information about the requested path on S3
                file_info stat(const boost::filesystem::wpath &) const;

                /// Fetch the requested resource and convert to a string
                string get(const boost::filesystem::wpath &location) const;
                /// Fetch the requested file and save to the file system
                outcome
                        get(const boost::filesystem::wpath &location,
                            const boost::filesystem::wpath &file) const;
                /// Send the specified text to the requested location on S3
                outcome
                        put(const string &text,
                            const boost::filesystem::wpath &location) const;
                /// Send the specified file to the requested location on S3
                outcome
                        put(const boost::filesystem::wpath &file,
                            const boost::filesystem::wpath &location) const;
            };


        }


    }


}


#endif // FOST_DETAIL_S3_HPP
