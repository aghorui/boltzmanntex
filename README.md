Disclaimer
==========

**BoltzmannTex is alpha quality software. While steps have been taken in the
subprograms to create accurate results, this software, in its current state
should not be depended on for actual real world use.**

**Please use this software at your own risk and not as your primary method of
computation.**

BoltzmannTex
===========

BoltzmannTex is a program for performing predefined transformations and
visualisations on acquired data using simple Graphical User Intefaces. This
software is mainly meant for well defined techniques that can be rigidly written
as a program, and do not require regular testing, editing or substantial
knowledge of programming for usage (such as in the case of python scripts).

BoltzmannTex aims to be modular enough to be easily extendable for more
sub-programs.

## Compilation

This Program requires a working installation of the QT development package
(>= 5.12). An offline package may be obtained from [here][qt-offline].

You will also need a working installation of the GCC or Clang toolchain for the
C/C++ compiler. This should come as a prerequisite for the QT development
package.

First you must obtain the project source files, which you may do via downloading
an archive from Github's interface, or run the following `git` command in a
terminal or command prompt:

```
git clone https://github.com/aghorui/BoltzmannTex.git
```

This will create a directory named `BoltzmannTex` with all the project source
files.


### Install Dependencies

BoltzmannTex depends on `QCustomPlot`, a GPL plotting library. This can be
obtained from [here][qcustomplot]. Download the complete archive of the latest
version available (currently 2.1.0) and extract it in `dependencies/qcustomplot`
within the project directory.

### Non-GUI Method

Make sure that the QT tools are in your `PATH`, and accessible via command line.

Navigate to the project directory, then open a terminal or command prompt in the
project directory. Then enter the following commands:

```
mkdir build
cd build
qmake ../BoltzmannTex
```

This will generate the necessary build files, and isolate the end product and
any intermediate files in this `build` directory  Next, type in the following
command:

```
make
```

This will compile the application. The application binary will be available in
the newly made `build` directory in the project directory.

### GUI Method

Open `BoltzmannTex/BoltzmannTex.pro` within the project directory with QT Creator,
then click on the build button. This will compile the application and put it in
a directory prefixed by `build`, within the project directory assuming a default
QTCreator configuration. If you want to change this directory, you can do so
via the "projects" tab in QTCreator.

## Currently Implemented Programs

### Graph Plot

Simple plotting example. Can be used as a template to write other programs.
(To be completed)


## License

BoltzmannTex is currently licensed under GNU GPLv3. Please see `COPYING` for
details.

[qt-offline]: https://www.qt.io/offline-installers
[qcustomplot]: https://www.qcustomplot.com/index.php/download

