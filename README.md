# lil jvm

toy jvm project that is very much a work in progress.

## Building

To build from source:

```sh
> mkdir release
> cd release
> cmake -DCMAKE_BUILD_TYPE=Release ..
> make
```

or for a debug build:

```sh
> mkdir debug
> cd debug
> cmake -DCMAKE_BUILD_TYPE=Debug ..
> make
```


This will compile the `java` program for executing class files, and the `javap` program for disassembling class files.


## Usage

Pre-Requisites: some already compiled java Class files.

__Execute java class file__:

```sh
# execute HelloWorld
> ./java HelloWorld
```

If debugging is enabled, additional information is displayed showing info about the current stack frame, like:

```

``
> cd debug
➜  ./java ../AddMain
Processing Method <init>
Processing Attribute Code
Processing Attribute LineNumberTable
Processing Method add
Processing Attribute Code
Processing Attribute LineNumberTable
Processing Method main
Processing Attribute Code
Processing Attribute LineNumberTable
Processing Attribute SourceFile

    # Frames = 1    Stack
   Locals [ false ]
(0x04) 00: iconst_1

    # Frames = 1    Stack [ 1 ]
   Locals [ false ]
(0x05) 01: iconst_2

    # Frames = 1    Stack [ 1 ][ 2 ]
   Locals [ false ]
(0xb8) 02: invokestatic         #   7 AddMain.add:(II)I

    # Frames = 2    Stack
   Locals [ 2 ][ 1 ]
(0x1a) 00: iload_0

    # Frames = 2    Stack [ 2 ]
   Locals [ 2 ][ 1 ]
(0x1b) 01: iload_1

    # Frames = 2    Stack [ 2 ][ 1 ]
   Locals [ 2 ][ 1 ]
(0x60) 02: iadd

    # Frames = 2    Stack [ 3 ]
   Locals [ 2 ][ 1 ]
(0xac) 03: ireturn

    # Frames = 1    Stack [ 3 ]
   Locals [ false ]
(0xb8) 05: invokestatic         #  13 java/lang/System.exit:(I)V
```


__Disassemble java class file:__

```sh
# disassembling HelloWorld.class
> ./javap HelloWorld.class
```

which should output something like:

```
Magic Number              0xcafebabe '
Minor Version                0 '
Major Version               64 '
Constant Pool : 28
  #01 = Methodref         #  2.#  3           // java/lang/Object.<init>:()V
  #02 = Class             #  4                // java/lang/Object
  #03 = NameAndType       #  5:#  6           // <init>:()V
  #04 = Utf8              java/lang/Object
  #05 = Utf8              <init>
  #06 = Utf8              ()V
  #07 = Fieldref          Unknown tag 0x09java/lang/System.out:Ljava/io/PrintStream;
  #08 = Class             # 10                // java/lang/System
  #09 = NameAndType       # 11:# 12           // out:Ljava/io/PrintStream;
  #10 = Utf8              java/lang/System
  #11 = Utf8              out
  #12 = Utf8              Ljava/io/PrintStream;
  #13 = String            # 14                // Hello World!
  #14 = Utf8              Hello World!
  #15 = Methodref         # 16.# 17           // java/io/PrintStream.println:(Ljava/lang/String;)V
  #16 = Class             # 18                // java/io/PrintStream
  #17 = NameAndType       # 19:# 20           // println:(Ljava/lang/String;)V
  #18 = Utf8              java/io/PrintStream
  #19 = Utf8              println
  #20 = Utf8              (Ljava/lang/String;)V
  #21 = Class             # 22                // HelloWorld
  #22 = Utf8              HelloWorld
  #23 = Utf8              Code
  #24 = Utf8              LineNumberTable
  #25 = Utf8              main
  #26 = Utf8              ([Ljava/lang/String;)V
  #27 = Utf8              SourceFile
  #28 = Utf8              HelloWorld.java
Access Flags              0x0021 '
This class                0x0015 // HelloWorld'
Super class               0x0002 '
Interfaces : 0
Fields : 0
Methods : 2
  Method[0]:
    Access Flags         = 0x0001
    Name Index           = 0x0005 // <init>
    Descriptor Index     = 0x0006
    Attributes : 1
      Code Attribute :
        Name Index           = 0x0017 // Code
        Length (Bytes)       = 0x001d
        Max Stack            = 0x0001
        Max Locals           = 0x0001
          (0x2a) 00: aload_0
          (0xb7) 01: invokespecial        #   1 java/lang/Object.<init>:()V
          (0xb1) 04: return
      Exception Table : 0
        Attributes : 1
          Line Number Table :
            Line 1: 0
  Method[1]:
    Access Flags         = 0x0009
    Name Index           = 0x0019 // main
    Descriptor Index     = 0x001a
    Attributes : 1
      Code Attribute :
        Name Index           = 0x0017 // Code
        Length (Bytes)       = 0x0025
        Max Stack            = 0x0002
        Max Locals           = 0x0001
          (0xb2) 00: getstatic            #   7 java/lang/System.out:Ljava/io/PrintStream;
          (0x12) 03: ldc                  #  13 Hello World!
          (0xb6) 05: invokevirtual        #  15 java/io/PrintStream.println:(Ljava/lang/String;)V
          (0xb1) 08: return
      Exception Table : 0
        Attributes : 1
          Line Number Table :
            Line 3: 0
            Line 4: 8
Attributes : 1
  Source File Attribute :
    Name Index           = 0x001b // SourceFile
    Length (Bytes)       = 0x0002
    Source File Index    = 0x001c // HelloWorld.java
```


## Running Tests

Each test specifies a piece of java source code and the expected output on stdout, then compiles it with `javac`, then executes the class file using the actual JVM (`/usr/bin/java`) and this project's jvm.

To execute tests first setup a venv and install pytest, then you can run them like:

```sh
> cd tests
> pytest tests.py
```

Here is a test for conditional and branching instructions. The lines starting with `>>>` or `...` are compiled into a java class file,
then the following lines state the expected output.

```python
def test_while(java_type):
    runDocTest(java_type, "WhileLoop", """
        >>> public class WhileLoop {
        ...     public static void main(String[] args){
        ...         int a = 2;
        ...         while (a < 15) {
        ...             System.out.println(a);
        ...             a += 2;
        ...         }
        ...     }
        ... }
        2
        4
        6
        8
        10
        12
        14
        """)
```

## Notes

I'd previously worked through [Crafting Interpreters](https://craftinginterpreters.com) which walks through implementing a scripting language.
Much of the code in here is inspired by concepts from the book.

