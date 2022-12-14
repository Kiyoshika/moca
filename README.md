# moca - [MO]dified [C] l[A]nguage
My first ever compiled programming language - very experimental.

This is aimed to be a blend of C, C++ and some custom features.

You can think of this being a stripped version of C as its base with some features from C++ & my own features such as:
* RAII
* Struct methods
* Struct method unions (SMUs) (custom feature)
* Optional auto-generated getters/setters (custom feature)
* References
* More explicit functions calls (custom feature) - see [below](#changes-to-function-calls) for more details
* Lambdas
* Namespaces
* Named Parameters (custom feature)
* Default parameter values

# Language Design
Please see [here](LANGUAGE_DESIGN.md) for the language design (still in progress).

# Building from Source
* git clone this repo
* `cd moca`
* `mkdir build && cd build`
* `cmake -DCMAKE_BUILD_TYPE=Debug ..` (or can use `Release` mode)
* `make`

Will add steps on how to compile `moca` code once the actual compiler is written and functional to some degree.

# CTags
This project (optionally) uses ctags, use `make tags` from the project root to generate the tags file and be sure to set `set tags=./tags;$HOME` in your `~/.vimrc`.

# Tests
I'm not using a testing library so all the tests are "hand-crafted". Inside the `test/` directory in your build folder you can use:

`make CTEST_OUTPUT_ON_FAILURE=1 test`

to run the test suite which will display the error (if any) to `stderr` in the console.
