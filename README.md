# RIG

RISC In Graphics.

A RISC-V simulator and editor, that can run RISC-V assembly code and generate many useful formats for the file.
It will also contain system calls that will create a graphics window, and you can manipulate its pixels with system calls.

## Supported Specifications:

RISC32I

- 32-bit Integer instructions

### In the future

- Pesudo instructions
- 32-bit Integer Multiplication instructions.
- 32-bit Floating point instructions.
- 32-bit Compressed Integer instructions.
- 64-bit instructions.

# Right now what it can do

- Only CLI
- Parse an assembly file. :D
  - Not entirely, just the text section
  - The data section is NEXT
  - The macros and the other directives will be NEXT
- Parsing the data section so far is correct, and we get the data in a contianer that is easily traversed
- Dumping the instructions in binary / hexadecimal / ascii binary.
- Disassembling the binary format to readable assembly instructions. (NO DATA SECTION, NOT NECC. WORKING CODE, JUST INSTRUCTIONS DISASSEMBLED)

# Next STEPS:

- Loggers
- Adding the text editor
- Constructing the IDE
- Pesudo instructions
- Pre-processing stage:
  - Parsing labels and putting them with the right address (both instructions and data).
  - Parsing Macros.
  - Include files. (Labels change their names to UNIQUE identifiers that are acceptable and created by the preprocessor "file_name_label_name")
- Getting all the directives to work.
- Starting the simulation. (very big step)
- Making sure it works.
- CONCURRENTLY:
  - Support compressed && multiplication instructions.
  - Start with the graphics.

## Problems: