/*
 * The Restful Matching-Engine.
 * Copyright (C) 2013, 2018 Swirly Cloud Limited.
 *
 * This program is free software; you can redistribute it and/or modify it under the terms of the
 * GNU General Public License as published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without
 * even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with this program; if
 * not, write to the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301, USA.
 */
#ifndef SWIRLYD_RESTREQUEST_HXX
#define SWIRLYD_RESTREQUEST_HXX

#include "RestBody.hxx"

#include <swirly/http/Types.hpp>
#include <swirly/http/Url.hpp>

namespace swirly {
using namespace std::literals::string_view_literals;

class SWIRLY_API RestRequest : public BasicUrl<RestRequest> {
  public:
    RestRequest() noexcept = default;
    ~RestRequest();

    // Copy.
    RestRequest(const RestRequest&) = delete;
    RestRequest& operator=(const RestRequest&) = delete;

    // Move.
    RestRequest(RestRequest&&) = delete;
    RestRequest& operator=(RestRequest&&) = delete;

    auto method() const noexcept { return method_; }
    auto url() const noexcept { return +url_; }
    auto accnt() const noexcept { return +accnt_; }
    auto perm() const noexcept { return +perm_; }
    auto time() const noexcept { return +time_; }
    const auto& body() const noexcept { return body_; }
    auto partial() const noexcept { return partial_; }
    void clear() noexcept
    {
        BasicUrl<RestRequest>::reset();
        method_ = HttpMethod::Get;
        url_.clear();
        field_.clear();
        value_ = nullptr;

        accnt_.clear();
        perm_.clear();
        time_.clear();
        body_.reset();
        partial_ = false;
    }
    void flush() { BasicUrl<RestRequest>::parse(); }
    void set_method(HttpMethod method) noexcept { method_ = method; }
    void append_url(std::string_view sv) { url_ += sv; }
    void append_header_field(std::string_view sv, bool first)
    {
        if (first) {
            field_ = sv;
        } else {
            field_ += sv;
        }
    }
    void append_header_value(std::string_view sv, bool first)
    {
        if (first) {
            if (field_ == "Swirly-Accnt"sv) {
                value_ = &accnt_;
            } else if (field_ == "Swirly-Perm"sv) {
                value_ = &perm_;
            } else if (field_ == "Swirly-Time"sv) {
                value_ = &time_;
            } else {
                value_ = nullptr;
            }
        }
        if (value_) {
            value_->append(sv);
        }
    }
    void append_body(std::string_view sv) { partial_ = !body_.parse(sv); }

  private:
    HttpMethod method_{HttpMethod::Get};
    StringBuf<128> url_;
    StringBuf<16> field_;
    StringBuf<24>* value_{nullptr};
    // Header fields.
    StringBuf<24> accnt_;
    StringBuf<24> perm_;
    StringBuf<24> time_;
    RestBody body_;
    bool partial_{false};
};

} // namespace swirly

#endif // SWIRLYD_RESTREQUEST_HXX
