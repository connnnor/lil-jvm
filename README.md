Java code:


```java
public class Add {
  public static int add(int a, int b) {
    return a + b;
  }
}

```

javap output:

```
Classfile /Users/connorkelleher/workspace-local/lil-jvm/Add.class
  Last modified Feb 8, 2025; size 236 bytes
  SHA-256 checksum 8b8c3bf8da010950e44568e3b341f45fecc2b7a861a46a6359da4d3bff62e411
  Compiled from "Add.java"
public class Add
  minor version: 0
  major version: 64
  flags: (0x0021) ACC_PUBLIC, ACC_SUPER
  this_class: #7                          // Add
  super_class: #2                         // java/lang/Object
  interfaces: 0, fields: 0, methods: 2, attributes: 1
Constant pool:
   #1 = Methodref          #2.#3          // java/lang/Object."<init>":()V
   #2 = Class              #4             // java/lang/Object
   #3 = NameAndType        #5:#6          // "<init>":()V
   #4 = Utf8               java/lang/Object
   #5 = Utf8               <init>
   #6 = Utf8               ()V
   #7 = Class              #8             // Add
   #8 = Utf8               Add
   #9 = Utf8               Code
  #10 = Utf8               LineNumberTable
  #11 = Utf8               add
  #12 = Utf8               (II)I
  #13 = Utf8               SourceFile
  #14 = Utf8               Add.java
{
  public Add();
    descriptor: ()V
    flags: (0x0001) ACC_PUBLIC
    Code:
      stack=1, locals=1, args_size=1
         0: aload_0
         1: invokespecial #1                  // Method java/lang/Object."<init>":()V
         4: return
      LineNumberTable:
        line 1: 0

  public static int add(int, int);
    descriptor: (II)I
    flags: (0x0009) ACC_PUBLIC, ACC_STATIC
    Code:
      stack=2, locals=2, args_size=2
         0: iload_0
         1: iload_1
         2: iadd
         3: ireturn
      LineNumberTable:
        line 3: 0
}
SourceFile: "Add.java"
```
