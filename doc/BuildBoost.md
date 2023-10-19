# Build Boost

---
**NOTE**

If your trying to build the latest version of boost, cmake might warn about that. Use Boost_NO_WARN_NEW_VERSIONS. 
If weird things happens, remember that you can use --debug-find.
---

---
**NOTE**

Avoid to use env vars... it's a mess... You can use this defines in cmake to set the paths:
```
-DBOOST_INCLUDEDIR=C:/Boost/boost_1_83_0
-DBoost_LIBRARY_DIR=C:/Boost/lib
```
---

---
**NOTE**
If you have different msvc version you can force one of them with
```
-DBoost_COMPILER with "--vc143"
```
---


## Download boost
You can download Boost 1.83.0 from the official site:
- https://www.boost.org/users/history/version_1_83_0.html

## Confire and build
You can optionally build dynamically with `link=shared` or static avoiding it. 
```
.\bootstrap.bat --with-libraries=all
.\b2 install link=shared
```
- When building **statically** boost add a prefix to the libraries with "lib". CMake need to know it. You can let it know with the define:
```
-DBoost_USE_STATIC_LIBS=ON
```

## Example
```
-DBoost_NO_WARN_NEW_VERSIONS=ON
-DBoost_USE_STATIC_LIBS=ON
-DBOOST_INCLUDEDIR=C:/Boost/boost_1_83_0
-DBoost_LIBRARY_DIR=C:/Boost/lib
```
