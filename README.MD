libE57Format
==

A library to provide read and write support for the E57 file format.

This is a fork of [E57RefImpl](https://sourceforge.net/projects/e57-3d-imgfmt/) v1.1.332 primarily for use in the [CloudCompare](https://github.com/CloudCompare/CloudCompare) project.

The source is from [E57RefImpl 1.1.332](https://sourceforge.net/projects/e57-3d-imgfmt/files/E57Refimpl-src/) and I stripped out all the extra files to clean it up (and I don't know about licensing/distribution of these) for the initial commit.

Why Fork?
--

The E57RefImpl code hasn't been touched in years and I wanted to make changes to compile this library with macOS. Forking it gives me a bit more freedom to update the code and make changes as required.

Versions of libE57Format will start at 2.0.

I changed the name so that it isn't confused with the **E57RefImpl** project.

License
--
The original code is under the [Boost Software License (BSL1.0)](https://opensource.org/licenses/BSL-1.0).
