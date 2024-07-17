# Jack-Compiler

A compiler written in C++ for the Jack language used in the nand2tetris course. The supplied Jack code is compiled into intermediate virtual-machine code, which can then be translated into any platform-specific assembly code provided that such translator exists.

To run:

For a single file:
```
cd src/Compiler
./compiler <filepath>
```
Replace filepath with the path to the intended file, including the .jack extension. For a file `<folderpath>/<filename>.jack`, outputs the vm code into a file with the name format `<folderpath>/<filename>.vm`.

For a directory:
```
cd src/Compiler
./compiler <folderpath>
```
All .jack files in the directory will be translated into separate .vm files with the format `<folderpath>/<filename>.vm`.

There are a few example Jack programs included along with either their translated vm code or the generated xml tree.

Note: For now, the compiler assumes that the Jack code is error free. This may or may not change in the future.

Warning: In the VM emulator supplied by nand2tetris, the user may have to remove the last newline from the VM code in order to properly run the program.