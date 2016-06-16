Swirly is an open-source, restful matching-engine written in C++14. The target platform is
GNU/Linux. The source-code is
[freely available on GitHub](http://github.com/swirlycloud/swirly). Swirly is designed to run
either in [the Cloud](@ref Cloud) or on dedicated infrastructure.

![Arch](@ref arch.png)

How It Works
------------

Market-makers place bids and offers into a central limit order-book. Market-makers can revise or
cancel these "resting" orders. Orders in the order-book are aggregated into price-levels and
presented to market participants as market-data.

A trade happens when an order crosses the spread and matches with an existing bid or offer. Orders
placed in a market are matched by price-time priority. Orders that are not matched remain as working
orders in the central order-book until they are either cancelled or filled.

Getting Started
---------------

- [Build from Source](@ref Build)
- [Swirlyd Manual](@ref Swirlyd)
- [Admin Guide](@ref Admin)

Developers
----------

- [Restful API](@ref RestApi)
- [Swirly Libraries](@ref Library)
- [GitHub Repo](http://github.com/swirlycloud/swirly)
- [Style Guide](@ref Style)
- [Contributing](@ref Contrib)
- [Travis CI](http://travis-ci.org/swirlycloud/swirly)

See [other articles](pages.html) for more information.

Project Status
--------------

The open-source port of Swirly to C++14 is in the early stages of development, so please bear with
us. See the [about page](@ref About) for more information on the project's history.

License
-------

Swirly is free software; you can redistribute it and/or modify it under the terms of the
[GNU General Public License](http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt) as published by
the Free Software Foundation; either version 2 of the License, or (at your option) any later
version.
