# TODO use link below to write custom explanations for failures
# https://docs.pytest.org/en/7.1.x/how-to/assert.html

import pexpect
import tempfile
import re
from pathlib import Path
import os

# markers
# loop
# cond

import pytest

ZHAVA_TYPE = 'zhava'
JAVA_TYPE = 'java'


BINS = {
    ZHAVA_TYPE : os.environ['ZHAVA'],
    JAVA_TYPE  : os.environ['JAVA'],
}


@pytest.fixture(params=[ZHAVA_TYPE, JAVA_TYPE])
def java_type(request):
    return request.param


def runJava(source, srcName, java_type):
    tmpDirName = './tmp/'
    tmpDir = Path(tmpDirName)
    # write source to Java file
    with open(tmpDir / f'{srcName}.java', 'w') as srcFile:
        srcFile.write(source)
        srcFile.flush()
    # compile with javac
    output = pexpect.run(f'javac {srcName}.java',
                         cwd=tmpDir)
    assert output.decode('ascii') is ''
    bin_path = BINS[java_type]
    classFile = tmpDir / f'{srcName}.class'
    output = pexpect.run(f'{bin_path} {srcName}',
                         cwd=tmpDir)
    #print(f'output=\n' + output.decode('ascii'))
    return output.decode('ascii')


def is_start(s):
    return s.startswith(">>>")


def is_continuation(s):
    return s.startswith("...")


def is_expected(s):
    return not (is_start(s) or is_continuation(s))


def strip_prefix(s, prefix):
    return s.removeprefix(prefix)


def remove_leading_spaces(l: list[str]):
    return [s.strip() for s in l]


def runDocTest(loxType, srcName, docTest: str):
    script = ""
    expected = ""
    lines = remove_leading_spaces(docTest.splitlines(True))
    index = 0
    while index < len(lines):
        line = lines[index]
        index += 1
        if is_start(line):
            script = line.removeprefix(">>>") + "\n"
            expected = ""
            while index < len(lines) and is_continuation(lines[index]):
                script += lines[index].removeprefix("...") + "\n"
                index += 1
        while index < len(lines) and is_expected(lines[index]):
            expected += lines[index] + '\n'
            index += 1
        runAndCompare(loxType, srcName, script, expected)

def normalize(s: str):
    return "\n".join(s.splitlines()).rstrip()

def runAndCompare(java_type, srcName, script, expected):
    with open('script.java', 'w') as f:
        f.write(script)
    actual = runJava(script, srcName, java_type)
    if expected and not expected.isspace():
        assert normalize(actual) == normalize(expected)

def runAndComparePattern(java_type, srcName, script, expectedPattern):
    actual = runJava(script, java_type)
    if expectedPattern:
        assert bool(re.match(expectedPattern, normalize(actual)))

def pattern(msg):
    return r"(?s).*" + msg + "(.*)"

def contains(msg):
    return r"(?s).*" + msg + "(.*)"

def error_pattern(msg):
    return r"(?s).*Error(.*)" + msg + "(.*)"

def runtime_error_pattern(msg):
    return r"(?s).*RuntimeError(.*)" + msg + "(.*)"

#
# Tests
#

def test_hello_world(java_type):
    runDocTest(java_type, "HelloWorld", """
        >>> public class HelloWorld {
        ...     public static void main(String[] args){
        ...         System.out.println("Hello World!");
        ...     }
        ... }
        Hello World!
        """)

def test_unary_operator(java_type):
    runDocTest(java_type, "UnaryExpr", """
        >>> public class UnaryExpr {
        ...     static void negate(int a) {
        ...         System.out.println("negate");
        ...         System.out.println(-a);
        ...     }
        ...
        ...     static void preIncrement(int a) {
        ...         System.out.println("preIncrement");
        ...         int b = ++a;
        ...         System.out.println(b);
        ...         System.out.println(a);
        ...     }
        ...
        ...     static void postIncrement(int a) {
        ...         System.out.println("postIncrement");
        ...         int b = a++;    
        ...         System.out.println(b);
        ...         System.out.println(a);
        ...     }
        ...
        ...     public static void main(String[] args) {
        ...         negate(50);
        ...         preIncrement(0);
        ...         postIncrement(0);
        ...     }
        ... }
        negate
        -50
        preIncrement
        1
        1
        postIncrement
        0
        1
        """)

def test_short_circuit(java_type):
    runDocTest(java_type, "ShortCircuit", """
        >>> public class ShortCircuit {
        ...
        ...     static boolean sideEffect() {
        ...         System.out.println("sideEffect");
        ...         return true;
        ...     }
        ...
        ...     static void shortCircuitOr(boolean a) {
        ...         System.out.println(a || sideEffect());
        ...     }
        ...
        ...     static void shortCircuitAnd(boolean a) {
        ...         System.out.println(a && sideEffect());
        ...     }
        ...
        ...     public static void main(String[] args){
        ...         shortCircuitAnd(false);
        ...         shortCircuitOr(true);
        ...     }
        ... }
        false
        true
        """)

def test_comparison_operators_int_gt(java_type):
    runDocTest(java_type, "ComparisonOperatorsGt", """
        >>> public class ComparisonOperatorsGt {
        ...     static void greaterThan(int a, int b) {
        ...         System.out.println(a > b);
        ...     }
        ...     public static void main(String[] args){
        ...         System.out.println("GreaterThan");
        ...         greaterThan(0, 1);
        ...         greaterThan(1, 1);
        ...         greaterThan(2, 1);
        ...     }
        ... }
        GreaterThan
        false
        false
        true
        """)

def test_comparison_operators_int_gt_eq(java_type):
    runDocTest(java_type, "ComparisonOperatorsGtEq", """
               >>> public class ComparisonOperatorsGtEq {
        ...     static void greaterThanOrEqualTo(int a, int b) {
        ...         System.out.println(a >= b);
        ...     }
        ...     public static void main(String[] args){
        ...         System.out.println("GreaterThanOrEqualTo");
        ...         greaterThanOrEqualTo(0, 1);
        ...         greaterThanOrEqualTo(1, 1);
        ...         greaterThanOrEqualTo(2, 1);
        ...     }
        ... }
        GreaterThanOrEqualTo
        false
        true
        true
        """)

def test_comparison_operators_int_lt(java_type):
    runDocTest(java_type, "ComparisonOperatorsLt", """
        >>> public class ComparisonOperatorsLt {
        ...     static void lessThan(int a, int b) {
        ...         System.out.println(a < b);
        ...     }
        ...     public static void main(String[] args){
        ...         System.out.println("LessThan");
        ...         lessThan(0, 1);
        ...         lessThan(1, 1);
        ...         lessThan(2, 1);
        ...     }
        ... }
        LessThan
        true
        false
        false
        """)

def test_comparison_operators_int_lt_eq(java_type):
    runDocTest(java_type, "ComparisonOperatorsLtEq", """
        >>> public class ComparisonOperatorsLtEq {
        ...     static void lessThanOrEqualTo(int a, int b) {
        ...         System.out.println(a <= b);
        ...     }
        ...     public static void main(String[] args){
        ...         System.out.println("LessThanOrEqualTo");
        ...         lessThanOrEqualTo(0, 1);
        ...         lessThanOrEqualTo(1, 1);
        ...         lessThanOrEqualTo(2, 1);
        ...     }
        ... }
        LessThanOrEqualTo
        true
        true
        false
        """)

def test_comparison_operators_int_eq(java_type):
    runDocTest(java_type, "ComparisonOperatorsEq", """
        >>> public class ComparisonOperatorsEq {
        ...     static void equalTo(int a, int b) {
        ...         System.out.println(a == b);
        ...     }
        ... 
        ...     public static void main(String[] args){
        ...         System.out.println("EqualTo");
        ...         equalTo(1, 1);
        ...         equalTo(0, 1);
        ...     }
        ... }
        EqualTo
        true
        false 
        """)

def test_comparison_operators_int_not_eq(java_type):
    runDocTest(java_type, "ComparisonOperatorsNotEq", """
        >>> public class ComparisonOperatorsNotEq {
        ...     static void notEqualTo(int a, int b) {
        ...         System.out.print(a);
        ...         System.out.print(" != ");
        ...         System.out.print(b);
        ...         System.out.print(" -> ");
        ...         System.out.println(a != b);
        ...     }
        ... 
        ...     public static void main(String[] args){
        ...         System.out.println("NotEqualTo");
        ...         notEqualTo(50, 50);
        ...         notEqualTo(50, 77);
        ...     }
        ... }
        NotEqualTo
        50 != 50 -> false
        50 != 77 -> true
        """)


@pytest.mark.loop
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

@pytest.mark.cond
def test_jumping_if_else(java_type):
    # if-else (then branch)
    runDocTest(java_type, "Conditionals", """
        >>> public class Conditionals {
        ...     public static void printPositiveOrNegative(int a) {
        ...         System.out.print("printPositiveOrNegative ");
        ...         System.out.print(a);
        ...         System.out.print(" ");
        ...         if (a > 0) {
        ...             System.out.print("positive");
        ...         } else if(a == 0) {
        ...             System.out.print("zero");
        ...         } else {
        ...             System.out.print("negative");
        ...         }
        ...         System.out.println();
        ...     }
        ... 
        ...     public static void printPositive(int a) {
        ...         System.out.print("printPositive ");
        ...         System.out.print(a);
        ...         if (a > 0) {
        ...             System.out.print(" ");
        ...             System.out.print("positive");
        ...         }
        ...         System.out.println();
        ...     }
        ... 
        ...     public static void main(String[] args){
        ...         printPositiveOrNegative(0);
        ...         printPositiveOrNegative(1);
        ...         printPositiveOrNegative(-1);
        ... 
        ...         printPositive(-1);
        ...         printPositive(1);
        ...     }
        ... }
        printPositiveOrNegative 0 zero
        printPositiveOrNegative 1 positive
        printPositiveOrNegative -1 negative
        printPositive -1
        printPositive 1 positive
        """)


def test_class_method(java_type):
    # if-else (then branch)
    runDocTest(java_type, "ClassMethod", """
        >>> public class ClassMethod {
        ...     public static class Person {
        ...         String name;
        ...         int age;
        ...         public Person(String name, int age) {
        ...             this.name = name;
        ...             this.age = age;
        ...         }
        ...         public void display() {
        ...             System.out.println("Name: " + name + ", Age: " + age);
        ...         }
        ...     }
        ...     public static void main(String[] args){
        ...         Person p = new Person("Xiao Kang", 31);
        ...         p.display();
        ...     }
        ... }
        Name: Xiao Kang, Age: 31
        """)

def test_inheritance(java_type):
    # if-else (then branch)
    runDocTest(java_type, "Inheritance", """
        >>> public class Inheritance {
        ...     public static class A {
        ...         public void method1() {
        ...             System.out.println("Class A method");
        ...         }
        ...     }
        ...     public static class B extends A {
        ...         public void method2() {
        ...             System.out.println("Class B method");
        ...         }
        ...     }
        ...     public static class C extends A {
        ...         public void method1() {
        ...             System.out.println("Class C method");
        ...         }
        ...     }
        ...     public static void main(String[] args){
        ...         A a = new A();
        ...         a.method1();
        ...         B b = new B();
        ...         b.method1();
        ...         b.method2();
        ...         C c = new C();
        ...         c.method1();
        ...     }
        ... }
        Class A method
        Class A method
        Class B method
        Class C method
        """)

def test_fibonacci(java_type):
    # if-else (then branch)
    runDocTest(java_type, "Fibonacci", """
        >>> public class Fibonacci {
        ...     static int fibonacci(int n) {
        ...         if (n <= 1) {
        ...             return n;
        ...         }
        ...         return fibonacci(n - 1) + fibonacci(n - 2);
        ...     }
        ...
        ...     public static void main(String[] args){
        ...         int fibN = fibonacci(9);
        ...         System.out.println(fibN);
        ...     }
        ... }
        34 
        """)

def test_clinit(java_type):
    # Class Init <clinit> method
    runDocTest(java_type, "ClassInit", """
        >>> class ClassInit {
        ... 
        ...     static String str = "string";
        ...     static int one = 1;
        ...     static int two;
        ... 
        ...     static {
        ...         two = 2;
        ...     }
        ... 
        ...     public static void main(String[] args){
        ...         System.out.println(str);
        ...         System.out.println(one);
        ...         System.out.println(two);
        ...     }
        ... 
        ... }
        string
        1
        2
        """)
