#ifndef _STATEFS_MIRROR_HPP_
#define _STATEFS_MIRROR_HPP_

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

#include <statefs/provider.hpp>
#include <mutex>

using statefs::BranchStorage;

template <typename T>
struct PropTraits
{
    PropTraits(char const *name, char const *defval)
        : name_(name), defval_(defval) {}

    char const *name_;
    char const *defval_;
};

typedef std::function<int (std::string const&)> updater_type;

template <typename T>
int read_from(T &src, char *dst, statefs_size_t len, statefs_off_t off)
{
    auto sz = src.size();
    if (off > sz)
        return 0;

    if (off + len > sz)
        len = sz - off;
    memcpy(dst, &src[off], len);
    return len;
}

class AnalogProperty
{
public:
    AnalogProperty(statefs::AProperty *, char const*);

    int getattr() const { return STATEFS_ATTR_READ; }
    statefs_ssize_t size() const;

    bool connect(::statefs_slot *slot) { return false; }

    int read(std::string *h, char *dst, statefs_size_t len, statefs_off_t);

    int write(std::string *h, char const *src
              , statefs_size_t len, statefs_off_t off)
    {
        return -1;
    }

    void disconnect() { }
    void release() {}

    virtual updater_type get_updater();

protected:

    int update(std::string const& v);

    statefs::AProperty *parent_;
    std::mutex m_;
    std::string v_;
};

class DiscreteProperty : public AnalogProperty
{
public:
    DiscreteProperty(statefs::AProperty *parent, char const* defval);
    int getattr() const;
    bool connect(::statefs_slot *slot);
    void disconnect();

    virtual updater_type get_updater();

private:

    int update(std::string const& v);

    ::statefs_slot *slot_;
};

class Src;

class Writer
{
public:
    Writer(statefs::AProperty *parent, updater_type update);

    int getattr() const;
    statefs_ssize_t size() const;

    bool connect(::statefs_slot *slot);

    int read(std::string *h, char *dst, statefs_size_t len
             , statefs_off_t off);

    int write(std::string *h, char const *src
              , statefs_size_t len, statefs_off_t off);

    void disconnect() { }
    void release() {}

protected:
    statefs::AProperty *parent_;
    updater_type update_;
    size_t size_;
};

typedef PropTraits<DiscreteProperty> Discrete;
typedef PropTraits<AnalogProperty> Analog;

class Dst : public statefs::Namespace
{
public:

    Dst(char const *name);

    virtual ~Dst() {}
    virtual void release() {}
};

class Src : public statefs::Namespace
{
public:
    Src(char const *name, std::shared_ptr<Dst> p);

    virtual ~Src() {}
    virtual void release() {}

    template <typename T>
    void insert(PropTraits<T> const &t) 
    {
        typedef statefs::BasicPropertyOwner<T, std::string> prop_type;
        std::shared_ptr<prop_type> prop(new prop_type(t.name_, t.defval_));
        dst_->insert(std::static_pointer_cast<statefs::ANode>(prop));
        statefs::Namespace::insert(new statefs::BasicPropertyOwner
                                   <Writer, std::string>(t.name_, prop->get_impl()->get_updater()));
    }

private:
    std::shared_ptr<Dst> dst_;
};


template <typename T>
Src& operator << (Src &ns, PropTraits<T> const &p)
{
    ns.insert(p);
    return ns;
}

#endif // _STATEFS_PROVIDER_HPP_
