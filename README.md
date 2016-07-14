# OPA-Editor
The sound editor for the Arduino OPA shield  
Version 1.01, 30/06/2016  

Copyright (c) 2015 - 2016 Frédéric Meslin / Thomas Hopper  

Open source software released under the MIT License, please refer to LICENCE file for more information  

Supported platforms:  
- Windows (XP / 7 / 8 and 10)  
- MACOS X 10.6 - 10.10  
- Linux  

Fred's Lab  
http://fredslab.net  
fred@fredslab.net, @marzacdev  


# Requirements

* [Qt 5](https://www.qt.io/download-open-source/) is needed to build the editor.
* The [OPA-Library](https://github.com/Marzac/OPA-Library/) is needed for your Arduino Uno.

Install the library and run the EditorBridge.ino (which can be found in the "Sketches" folder) to be able to control the OPA-Shield from the Editor.


## Windows and MacOS X

Binaries for Windows and MacOS X can be found on http://www.fredslab.net/opa/


## Building on Linux

The version of Qt 5 packaged by your distribution might not be complete to be able 
to build the OPA-Editor. In this case, download the official [Qt 5 linux installer](https://www.qt.io/download-open-source/), for example version 5.7, and install it in your home folder.
Then from the OPA-Editor folder type:

    ~/Qt/5.7/gcc_64/bin/qmake
    make

Then run:
    ./OPAEditor
    


