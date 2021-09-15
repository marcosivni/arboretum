# Arboretum

## Library for metric indexes

This repository contains a forked branch of the Arboretum library maintained by [GBDI](http://www.gbdi.icmc.usp.br/). The main and original branch is available [here](https://bitbucket.org/gbdi/arboretum/src/master/). This forked version includes:

- Support for diversity queries ([BRID](https://dl.acm.org/doi/abs/10.1145/2484838.2484854) and [BRIDGE](https://dl.acm.org/doi/10.1109/CBMS.2014.21) searches) - Sequential scan.
- Older metric indexes

## Installation

Arboretum compilation produces a static library file (`libarboretum.a`) to be linked to your C++ project. The `MAKEFILE` document for Arboretum compilation is located in the `lib/arboretumgcc` folder. 

```/bin/bash
$ cd arboretum/arboretum/lib/arboretumgcc
$ make && make clean
```

The result `libarboretum.a` will be placed in the `/lib` folder.

> **NOTE:** A C++ compiler (g++, mingw, etc.) is required in this compilation stage.

## Usage

Link the Arboretum headers and the static library in your C++ project.

```qt
INCLUDEPATH += arboretum/arboretum/include
LIBS += -L arboretum/arboretum/lib/ -larboretum
```

## Notes 

- See License
- _(C) THIS SOFTWARE IS PROVIDED ``AS IS'' AND ANY EXPRESSED OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHORS OF THIS SOFTWARE OR ITS CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE._
