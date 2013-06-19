/*
 * Support for piping of the properties: data written to one property
 * appears as content of the property with the same name on the other
 * end
 *
 * Copyright (C) 2013 Jolla Ltd.
 * Contact: Denis Zalevskiy <denis.zalevskiy@jollamobile.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.

 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.

 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301 USA
 * 
 * http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html
 */

#include "mirror.hpp"
#include <algorithm>

AnalogProperty::AnalogProperty
(statefs::AProperty *parent, char const* defval)
    : parent_(parent), v_(defval)
{}

statefs_ssize_t AnalogProperty::size() const
{
    return std::max(128, (int)v_.size());
}

int AnalogProperty::read
(std::string *h, char *dst, statefs_size_t len, statefs_off_t off)
{
    auto &v = *h;
    if (!off) {
        std::lock_guard<std::mutex> lock(m_);
        v = v_;
    }

    return read_from(v, dst, len, off);
}

updater_type AnalogProperty::get_updater()
{
    using namespace std::placeholders;
    return std::bind(std::mem_fn(&AnalogProperty::update), this, _1);
}

int AnalogProperty::update(std::string const& v)
{
    std::lock_guard<std::mutex> lock(m_);
    v_ = v;
    return v.size();
}

DiscreteProperty::DiscreteProperty
(statefs::AProperty *parent, char const* defval)
    : AnalogProperty(parent, defval)
{}

int DiscreteProperty::getattr() const
{
    return STATEFS_ATTR_READ | STATEFS_ATTR_DISCRETE;
}

bool DiscreteProperty::connect(::statefs_slot *slot)
{
    slot_ = slot;
    return true;
}

void DiscreteProperty::disconnect()
{
    slot_ = nullptr;
}

updater_type DiscreteProperty::get_updater()
{
    using namespace std::placeholders;
    return std::bind(std::mem_fn(&DiscreteProperty::update), this, _1);
}

int DiscreteProperty::update(std::string const& v)
{
    int rc = AnalogProperty::update(v);
    auto slot = slot_;
    if (slot)
        slot_->on_changed(slot_, parent_);
    return rc;
}

Writer::Writer(statefs::AProperty *parent, updater_type update)
    : parent_(parent), update_(update), size_(128)
{}

int Writer::getattr() const
{
    return STATEFS_ATTR_WRITE;
}

statefs_ssize_t Writer::size() const
{
    return size_ ;
}

bool Writer::connect(::statefs_slot *slot)
{
    return false;
}

int Writer::read(std::string *h, char *dst, statefs_size_t len
                 , statefs_off_t off)
{
    return -1;
}

int Writer::write(std::string *h, char const *src
                  , statefs_size_t len, statefs_off_t off)
{
    auto &v = *h;
    if (len) {
        auto max_sz = len + off;
        if (max_sz > v.size()) {
            v.resize(max_sz);
            size_ = max_sz;
        }
        std::copy(src, src + len, &v[off]);
    } else {
        v = "";
    }
            
    return update_(v);
}

Dst::Dst(char const *name) : Namespace(name)
{
}

Src::Src(char const *name, std::shared_ptr<Dst> p)
    : Namespace(name), dst_(p)
{
}
