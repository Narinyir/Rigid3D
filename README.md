SFML-OpenGL
===========

### Dependencies
* Mesa3D
* glm
* glew
* SFML
* GlUtils

On linux systems, make sure to append the directores containing the above
shared libraries to your __LD_LIBRARY_PATH__. For example, to point the shared
library loader to the GlUtils shared library that ships with this project,
append the following to your .bashrc file:
<code>export LD_LIBRARY_PATH=${LD_LIBRARY_PATH}:<project root>/lib

### Building Libraries (Linux)
#### GTest
* cd build
* <code>premake4 --file="gtest.lua" gmake</code>
* <code>make config=debug</code>
* <code>make config=release</code>

#### GLUtils
* from root folder of project
* <code>premake4 gmake</code>
* <code>make config=debug</code>
* <code>make config=release</code>
