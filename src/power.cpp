/*
 * Power properties emulation statefs provider
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
#include <iostream>


class Provider : public statefs::AProvider
{
public:
    Provider() : AProvider("power-emu") {
        std::shared_ptr<Dst> dst(new Dst("Battery"));
        insert(std::static_pointer_cast<statefs::ANode>(dst));

        std::shared_ptr<Src> src(new Src("BatteryEmu", dst));
        insert(std::static_pointer_cast<statefs::ANode>(src));
        *src << Discrete("ChargePercentage", "100")
             << Discrete("IsCharging", "false")
             << Discrete("OnBattery", "true")
             << Discrete("LowBattery", "false")
             << Discrete("ChargeBars", "8")
             << Discrete("TimeUntilLow", "100000")
             << Discrete("TimeUntilFull", "0");
    }
    virtual ~Provider() {}

    virtual void release() {
        delete this;
    }
};

static Provider *provider = nullptr;

EXTERN_C struct statefs_provider * statefs_provider_get(void)
{
    if (provider)
        throw std::logic_error("provider ptr is already set");
    provider = new Provider();
    return provider;
}
