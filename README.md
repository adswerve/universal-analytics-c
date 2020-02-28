# Universal Analytics in C

This library provides a C interface to Google Analytics, supporting the Universal Analytics Measurement Protocol.

**NOTE** this project is reasonably feature-complete for most use-cases, covering all relevant features of the Measurement Protocol, however we still consider it _beta_. Please feel free to file issues for feature requests.

# Contact
Email: `opensource@adswerve.com`

# Usage

For the most accurate data in your reports, Analytics Pros recommends establishing a distinct ID for each of your users, and integrating that ID on your front-end web tracking, as well as tracking calls from other platforms (where this C library would run). This provides for a consistent, correct representation of user engagement, without skewing overall visit metrics (and others).

Please see the the following examples:

- [main.c](./test/main.c) for dynamic allocation example
- [static-alloc.c](./test/static-alloc.c) for static allocation example


We'll be building out additional examples of all available tracking within it. Currently it only highlights Pageview and Event tracking, but the remaining types should be fairly intuitive extensions.

Due to the constraints of the C language, this library's interface that differs slightly from Google's `analytics.js` implementation. In particular, `enum` constants are used to specify parameter fields, rather than strings (i.e. by parameter name). This offers some measurable efficiencies, while remaining quite readable (in our opinion).


# Features not implemented

* Throttling


# License

universal-analytics-c is licensed under the [BSD license](./LICENSE)

