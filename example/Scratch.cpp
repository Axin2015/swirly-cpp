/*
 * Swirly Order-Book and Matching-Engine.
 * Copyright (C) 2013, 2015 Swirly Cloud Limited.
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
#include <swirly/elm/Factory.hpp>
#include <swirly/elm/Model.hpp>

#include <iostream>

namespace swirly {

inline std::ostream& operator <<(std::ostream& os, const Rec& rec)
{
    return os << rec.recType() << ' ' << rec.mnem() << ' ' << rec.display();
}

class MockModel : public Model {
protected:
    RecSet doReadAsset(const Factory& factory) const override
    {
        RecSet s;
        s.insert(factory.newAsset("EUR", "Euro Dollar", AssetType::CURRENCY));
        s.insert(factory.newAsset("GBP", "Sterling", AssetType::CURRENCY));
        s.insert(factory.newAsset("USD", "US Dollar", AssetType::CURRENCY));
        s.insert(factory.newAsset("USD", "US Dollar", AssetType::CURRENCY));
        return s;
    }
    RecSet doReadContr(const Factory& factory) const override
    {
        RecSet s;
        return s;
    }
    RecSet doReadMarket(const Factory& factory) const override
    {
        RecSet s;
        return s;
    }
    RecSet doReadTrader(const Factory& factory) const override
    {
        RecSet s;
        return s;
    }

public:
    MockModel() noexcept = default;
    virtual ~MockModel() noexcept = default;

    // Copy.
    constexpr MockModel(const MockModel&) noexcept = default;
    MockModel& operator =(const MockModel&) noexcept = default;

    // Move.
    constexpr MockModel(MockModel&&) noexcept = default;
    MockModel& operator =(MockModel&&) noexcept = default;
};

} // swirly

int main(int argc, char* argv[])
{
    using namespace boost;
    using namespace std;
    using namespace swirly;

    try {
        BasicFactory f;
        MockModel m;
        auto s = m.readAsset(f);
        cout << "--\n";
        for (const auto& r : s) {
            const auto& asset = static_cast<const Asset&>(r);
            cout << asset << endl;
        }

        RequestPtr req{new Request{"MARAYL"}};
        cout << req->trader() << endl;
        return 0;
    } catch (const std::exception& e) {
        cerr << "error: " << e.what() << endl;
    }
    return 1;
}
