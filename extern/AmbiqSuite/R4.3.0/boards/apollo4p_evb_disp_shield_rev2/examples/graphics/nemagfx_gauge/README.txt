Name:
=====
 nemagfx_gauge


Description:
============
 Example of the app running under NemaGFX gauge examples.

There is a guiBuilder
project file located in "src/NemaGUI_Builder_project", please use Nema guiBuilder
version 1.0.1 or higher to open it. If you have some application level
modifition from the guiBuilder and you want to demonstrate it on the board,
you need perform flowing two steps

1. copy the files from "src/NemaGUI_Builder_project/generated" into
"src/generated" except two source file should examine, those are
framebuffers.c
main.c

2. porting “framebuffers.c“ in GuiBuilder generated code into “nemagui_porting.h“
to change the framebuffers and screen resolution

Feel free to use any text comparison tools such as diff to see
the difference, and port all the differences into new generated code.
After that, you can compile the whole project again.


******************************************************************************


