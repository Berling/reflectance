# Reflectance ![build status](https://travis-ci.org/Berling/reflectance.svg?branch=develop)
Reflectance is a simple somewhat reflection like C++ library. One can register a set of properties to construct an instance of a non fundamental type without knowing the actual type or accessing members with only knowing the name of the member. This feature set is not ideal and there is still a lot of manual work to do but it is less painfull (in my humble oppinion) than other approaches. The library is designed to be used in a data driven approach, e.g. instanciating non fundamental types via a description file.

> Note:
> If you are concerened about performance than this is not the library you are looking for. It makes heavy use of type erasure and virtual calls.
