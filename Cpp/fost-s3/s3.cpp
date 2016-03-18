/*
    Copyright 2009-2015, Felspar Co Ltd. http://support.felspar.com/
    Distributed under the Boost Software License, Version 1.0.
    See accompanying file LICENSE_1_0.txt or copy at
        http://www.boost.org/LICENSE_1_0.txt
*/


#include "fost-aws.hpp"
#include <fost/s3.hpp>

#include <boost/filesystem/fstream.hpp>
#include <boost/lambda/bind.hpp>

#include <fost/insert>
#include <fost/log>


using namespace fostlib;
using namespace fostlib::aws::s3;


namespace {
    nullable< string > etag(const boost::filesystem::wpath &file) {
        if ( boost::filesystem::exists(file) ) {
            digester md5_digest(md5);
            md5_digest << file;
            return coerce< string >(coerce< hex_string >(md5_digest.digest()));
        } else {
            return null;
        }
    }
}


/*
    fostlib::aws::s3::bucket
*/

const setting< string > fostlib::aws::s3::bucket::s_account_name(
    "fost-internet/Cpp/fost-aws/s3.cpp",
    "Amazon S3", "Default account name", "default", true);

namespace {
    auto s3do(
        const http::user_agent &ua, http::user_agent::request &request
    ) {
        auto response = ua(request);
        if ( response->status() == 403 ) {
            exceptions::not_implemented exception("S3 response");
            insert(exception.data(), "ua", "base", ua.base());
            insert(exception.data(), "request", "method", request.method());
            insert(exception.data(), "request", "url", request.address());
            insert(exception.data(), "request", "headers", request.headers());
            json rj;
            insert(rj, "status", response->status());
            insert(rj, "body", "size", response->body()->data().size());
            insert(rj, "body", "data",
                coerce<string>(coerce<utf8_string>(response->body()->data())));
            insert(rj, "headers", response->headers());
            insert(exception.data(), "response", rj);
            throw exception;
        }
        fostlib::log::debug(c_fost_aws_s3)
            ("s3req", "method", request.method())
            ("s3req", "url", request.address())
            ("s3req", "headers", request.headers())
            ("s3req", "response-status", response->status())
            ("s3req", "response-headers", response->headers());
        return response;
    }
    url base_url(const ascii_printable_string &bucket) {
        nullable<string> base(account_setting< string >(
            aws::s3::bucket::s_account_name.value(),  L"Base URL",  null));
        return url(url(base.value("https://s3.amazonaws.com/")),
            url::filepath_string(bucket + "/"));
    }
}


fostlib::aws::s3::bucket::bucket(const ascii_printable_string &name)
: m_ua(base_url(name)), name( name ) {
    m_ua.authentication(boost::function< void ( http::user_agent::request & ) >(
            boost::lambda::bind(
                rest_authentication, s_account_name.value(), name, boost::lambda::_1)));
}


url fostlib::aws::s3::bucket::uri(const boost::filesystem::wpath &location) const {
    return url(m_ua.base(), m_ua.base().pathspec() +
        coerce<url::filepath_string>(location));
}


file_info fostlib::aws::s3::bucket::stat(const boost::filesystem::wpath &location) const {
    return file_info(m_ua, name(), location);
}


fostlib::string fostlib::aws::s3::bucket::get(
    const boost::filesystem::wpath &location
) const {
    http::user_agent::request request("GET", uri(location));
    auto response(s3do(m_ua, request));
    switch ( response->status() ) {
        case 200:
            break;
        default:
            exceptions::not_implemented exception(__FUNCTION__);
            insert(exception.data(), "response", "status", response->status());
            throw exception;
    }
    return coerce<string>(coerce<utf8_string>(response->body()->data()));
}


aws::s3::outcome fostlib::aws::s3::bucket::get(
    const boost::filesystem::wpath &location, const boost::filesystem::wpath &file
) const {
    nullable< string > local(etag(file));
    if ( !local.isnull() ) {
        file_info remote(stat(location));
        if ( !remote.exists() ) {
            throw exceptions::unexpected_eof("There is a local file already, but no remote file");
        }
        if ( remote.md5() == local.value() || remote.md5() == "\"" + local.value() + "\"" ) {
            // We already have the same file locally
            return e_match;
        }
    }
    http::user_agent::request request("GET", uri(location));
    auto response(s3do(m_ua, request));
    switch ( response->status() ) {
        case 200:
            break;
        default:
            exceptions::not_implemented exception(__FUNCTION__);
            insert(exception.data(), "response", "status", response->status());
            throw exception;
    }
    boost::filesystem::ofstream stream(file, std::ios::binary);
    if ( response->body()->data().size() != 0 ) {
        stream.write(
            reinterpret_cast<const char *>(&response->body()->data()[0]),
            response->body()->data().size());
    }
    return e_executed;
}


namespace {
    aws::s3::outcome do_put(
        const fostlib::aws::s3::bucket &bucket,
        const http::user_agent &ua,
        const string &hash,
        const boost::filesystem::wpath &location,
        http::user_agent::request &request
    ) {
        aws::s3::file_info remote(bucket.stat(location));
        if ( !remote.exists() ||
                (remote.md5() != hash && remote.md5() != L"\"" + hash + L"\"") ) {
            auto response(s3do(ua, request));
            switch ( response->status() ) {
                case 200:
                case 201:
                    break;
                default:
                    exceptions::not_implemented exception(__FUNCTION__);
                    insert(exception.data(), "response", "status", response->status());
                    throw exception;
            }
            return e_executed;
        } else {
            return e_match;
        }
    }
}
aws::s3::outcome fostlib::aws::s3::bucket::put(
    const boost::filesystem::wpath &file, const boost::filesystem::wpath &location
) const {
    nullable< string > local(etag(file));
    if ( local.isnull() ) {
        throw exceptions::unexpected_eof("Local file could not be read");
    }
    http::user_agent::request request("PUT", uri(location), file);
    return do_put(*this, m_ua, local.value(), location, request);
}
fostlib::aws::s3::outcome fostlib::aws::s3::bucket::put(
    const string &text, const boost::filesystem::wpath &location
) const {
    http::user_agent::request request("PUT", uri(location), text);
    return do_put(*this, m_ua, md5(text), location, request);
}


/*
    fostlib::aws::s3::file_info
*/


namespace {
    auto init_file_info(const http::user_agent &ua, const url &u) {
        http::user_agent::request r("HEAD", u);
        return s3do(ua, r);
    }
}
fostlib::aws::s3::file_info::file_info(const http::user_agent &ua, const ascii_printable_string &bucket, const boost::filesystem::wpath &location )
: m_response( init_file_info(ua, url(ua.base(), ua.base().pathspec() + coerce<url::filepath_string>(location))).release() ), path( location ) {
    switch ( m_response->status() ) {
        case 200:
        case 404:
            break;
        default:
            exceptions::not_implemented exception(__FUNCTION__);
            insert(exception.data(), "response", "status", m_response->status());
            throw exception;
    }
}


bool fostlib::aws::s3::file_info::exists() const {
    return m_response->status() == 200;
}
nullable< string > fostlib::aws::s3::file_info::md5() const {
    if ( exists() && m_response->body()->headers().exists(L"ETag") )
        return m_response->body()->headers()[L"ETag"].value();
    else
        return null;
}

