/**
    Copyright 2009-2020 Red Anchor Trading Co. Ltd.

    Distributed under the Boost Software License, Version 1.0.
    See <http://www.boost.org/LICENSE_1_0.txt>
 */


#include "fost-aws.hpp"
#include <fost/s3.hpp>

#include <boost/filesystem/fstream.hpp>
#include <boost/lambda/bind.hpp>

#include <fost/insert>
#include <fost/log>


namespace {
    std::optional<fostlib::string> etag(fostlib::fs::path const &file) {
        if (fostlib::fs::exists(file)) {
            fostlib::digester md5_digest(fostlib::md5);
            md5_digest << file;
            return fostlib::coerce<fostlib::string>(fostlib::coerce<fostlib::hex_string>(md5_digest.digest()));
        } else {
            return {};
        }
    }
}


/**
    fostlib::aws::s3::bucket
*/

fostlib::setting<fostlib::string> const fostlib::aws::s3::bucket::s_account_name(
        "fost-internet/Cpp/fost-aws/s3.cpp",
        "Amazon S3",
        "Default account name",
        "default",
        true);

namespace {
    auto s3do(fostlib::http::user_agent const &ua, fostlib::http::user_agent::request &request) {
        auto response = ua(request);
        if (response->status() == 403) {
            fostlib::exceptions::not_implemented exception{"S3 response"};
            insert(exception.data(), "ua", "base", ua.base());
            insert(exception.data(), "request", "method", request.method());
            insert(exception.data(), "request", "url", request.address());
            insert(exception.data(), "request", "headers", request.headers());
            fostlib::json rj;
            insert(rj, "status", response->status());
            insert(rj, "body", "size", response->body()->data().size());
            insert(rj, "body", "data", response->body()->body_as_string());
            insert(rj, "headers", response->headers());
            insert(exception.data(), "response", rj);
            throw exception;
        }
        fostlib::log::debug(fostlib::c_fost_aws_s3)("s3req", "method", request.method())(
                "s3req", "url",
                request.address())("s3req", "headers", request.headers())(
                "s3req", "response-status", response->status())(
                "s3req", "response-headers", response->headers());
        return response;
    }
    fostlib::url base_url(fostlib::ascii_printable_string const &bucket) {
        fostlib::nullable<fostlib::string> base(fostlib::aws::s3::account_setting<fostlib::string>(
                fostlib::aws::s3::bucket::s_account_name.value(), L"Base URL", fostlib::null));
        return fostlib::url(
                fostlib::url(base.value_or("https://s3.amazonaws.com/")),
                fostlib::url::filepath_string(bucket + "/"));
    }
}


fostlib::aws::s3::bucket::bucket(const ascii_printable_string &name)
: m_ua(base_url(name)), name(name) {
    m_ua.authentication([name](auto &rq) {
        rest_authentication(s_account_name.value(), name, rq);
    });
}


fostlib::url fostlib::aws::s3::bucket::uri(
        const boost::filesystem::wpath &location) const {
    return url(
            m_ua.base(),
            m_ua.base().pathspec() + coerce<url::filepath_string>(location));
}


fostlib::aws::s3::file_info fostlib::aws::s3::bucket::stat(
        const boost::filesystem::wpath &location) const {
    return file_info(m_ua, name(), location);
}


fostlib::string fostlib::aws::s3::bucket::get(
        const boost::filesystem::wpath &location) const {
    http::user_agent::request request("GET", uri(location));
    auto response(s3do(m_ua, request));
    switch (response->status()) {
    case 200: break;
    default:
        exceptions::not_implemented exception(__FUNCTION__);
        insert(exception.data(), "response", "status", response->status());
        throw exception;
    }
    return coerce<string>(coerce<utf8_string>(response->body()->data()));
}


fostlib::aws::s3::outcome fostlib::aws::s3::bucket::get(
        const boost::filesystem::wpath &location,
        const boost::filesystem::wpath &file) const {
    nullable<string> local(etag(file));
    if (local) {
        file_info remote(stat(location));
        if (!remote.exists()) {
            throw exceptions::unexpected_eof(
                    "There is a local file already, but no remote file");
        }
        if (remote.md5() == local.value()
            || remote.md5() == "\"" + local.value() + "\"") {
            // We already have the same file locally
            return e_match;
        }
    }
    http::user_agent::request request("GET", uri(location));
    auto response(s3do(m_ua, request));
    switch (response->status()) {
    case 200: break;
    default:
        exceptions::not_implemented exception(__FUNCTION__);
        insert(exception.data(), "response", "status", response->status());
        throw exception;
    }
    boost::filesystem::ofstream stream(file, std::ios::binary);
    if (response->body()->data().size() != 0) {
        stream.write(
                reinterpret_cast<const char *>(&response->body()->data()[0]),
                response->body()->data().size());
    }
    return e_executed;
}


namespace {
    fostlib::aws::s3::outcome
            do_put(const fostlib::aws::s3::bucket &bucket,
                   fostlib::http::user_agent const &ua,
                   fostlib::string const &hash,
                   fostlib::fs::path const &location,
                   fostlib::http::user_agent::request &request) {
        fostlib::aws::s3::file_info remote(bucket.stat(location));
        if (!remote.exists()
            || (remote.md5() != hash && remote.md5() != "\"" + hash + "\"")) {
            auto response(s3do(ua, request));
            switch (response->status()) {
            case 200:
            case 201: break;
            default:
                fostlib::exceptions::not_implemented exception{__PRETTY_FUNCTION__};
                insert(exception.data(), "response", "status",
                       response->status());
                throw exception;
            }
            return fostlib::aws::s3::e_executed;
        } else {
            return fostlib::aws::s3::e_match;
        }
    }
}
fostlib::aws::s3::outcome fostlib::aws::s3::bucket::put(
        const boost::filesystem::wpath &file,
        const boost::filesystem::wpath &location) const {
    nullable<string> local(etag(file));
    if (not local) {
        throw exceptions::unexpected_eof("Local file could not be read");
    }
    http::user_agent::request request("PUT", uri(location), file);
    return do_put(*this, m_ua, local.value(), location, request);
}
fostlib::aws::s3::outcome fostlib::aws::s3::bucket::put(
        const string &text, const boost::filesystem::wpath &location) const {
    http::user_agent::request request("PUT", uri(location), text);
    return do_put(*this, m_ua, md5(text), location, request);
}


/*
    fostlib::aws::s3::file_info
*/


namespace {
    auto init_file_info(const fostlib::http::user_agent &ua, const fostlib::url &u) {
        fostlib::http::user_agent::request r("HEAD", u);
        return s3do(ua, r);
    }
}
fostlib::aws::s3::file_info::file_info(
        const http::user_agent &ua,
        const ascii_printable_string &bucket,
        const boost::filesystem::wpath &location)
: m_response(init_file_info(
                     ua,
                     url(ua.base(),
                         ua.base().pathspec()
                                 + coerce<url::filepath_string>(location)))
                     .release()),
  path(location) {
    switch (m_response->status()) {
    case 200:
    case 404: break;
    default:
        exceptions::not_implemented exception{__PRETTY_FUNCTION__};
        insert(exception.data(), "response", "status", m_response->status());
        throw exception;
    }
}


bool fostlib::aws::s3::file_info::exists() const {
    return m_response->status() == 200;
}
fostlib::nullable<fostlib::string> fostlib::aws::s3::file_info::md5() const {
    if (exists() && m_response->body()->headers().exists("ETag"))
        return m_response->body()->headers()["ETag"].value();
    else
        return null;
}
