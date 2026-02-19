import CallableFunction from "./callable-function";
import { Lox, LoxResolver, LoxWithResolver } from "./lox";
import { Parser } from "./parser";
import { Scanner } from "./scanner";

describe("Lox", () => {
  it("prints statement", () => {
    const buffer: any[] = [];
    // HACK: set 'printer = console.log' to print to screen
    const printer = (...args: any) => buffer.push(...args);
    const scanner = new Scanner(`
print "one";
print true;
print 2 + 1;
    `);
    const tokens = scanner.scan();
    const parser = new Parser(tokens);
    const stmts = parser.parse();
    const lox = new Lox(printer);
    lox.interpret(stmts);
    expect(buffer).toHaveLength(3);
    // TODO: "one" or "'one'" ?
    expect(buffer).toEqual(["one", "true", "3"]);
  });

  it("prints nil values", () => {
    const buffer: any[] = [];
    const printer = (...args: any) => buffer.push(...args);
    const scanner = new Scanner(`
var a;
print a;
       `);
    const tokens = scanner.scan();
    const parser = new Parser(tokens);
    const stmts = parser.parse();
    const lox = new Lox(printer);
    lox.interpret(stmts);
    expect(buffer).toEqual(["nil"]);
  });

  it("prints assigned variables", () => {
    const buffer: any[] = [];
    const printer = (...args: any) => buffer.push(...args);
    const scanner = new Scanner(`
var a = "before";
print a;
       `);
    const tokens = scanner.scan();
    const parser = new Parser(tokens);
    const stmts = parser.parse();
    const lox = new Lox(printer);
    lox.interpret(stmts);
    expect(buffer).toEqual(["before"]);
  });

  it("prints re-assigned variables", () => {
    const buffer: any[] = [];
    const printer = (...args: any) => buffer.push(...args);
    const scanner = new Scanner(`
var a = 1;
print a = 2;
    `);
    const tokens = scanner.scan();
    const parser = new Parser(tokens);
    const stmts = parser.parse();
    const lox = new Lox(printer);
    lox.interpret(stmts);
    expect(buffer).toEqual(["2"]);
  });

  it("prints the result of 2 variable addition", () => {
    const buffer: any[] = [];
    const printer = (...args: any) => buffer.push(...args);
    const scanner = new Scanner(`
var a = 1;
var b = 2;
print a + b;
     `);
    const tokens = scanner.scan();
    const parser = new Parser(tokens);
    const stmts = parser.parse();
    const lox = new Lox(printer);
    lox.interpret(stmts);
    expect(buffer).toEqual(["3"]);
  });

  it("scopes variables", () => {
    const buffer: any[] = [];
    const printer = (...args: any) => buffer.push(...args);
    const scanner = new Scanner(`
var a = "global a";
var b = "global b";
var c = "global c";
{
var a = "outer a";
var b = "outer b";
{
var a = "inner a";
print a;
print b;
print c;
}
print a;
print b;
print c;
}
print a;
print b;
print c;
     `);
    const tokens = scanner.scan();
    const parser = new Parser(tokens);
    const stmts = parser.parse();
    const lox = new Lox(printer);
    lox.interpret(stmts);
    expect(buffer).toEqual([
      "inner a",
      "outer b",
      "global c",
      "outer a",
      "outer b",
      "global c",
      "global a",
      "global b",
      "global c",
    ]);
  });

  it("works with operands or and and", () => {
    const buffer: any[] = [];
    const printer = (...args: any) => buffer.push(...args);
    const scanner = new Scanner(`
print "hi" or 2;
print nil or "yes";
print nil and nil;
print 1 and nil;
 `);
    const tokens = scanner.scan();
    const parser = new Parser(tokens);
    const stmts = parser.parse();
    const lox = new Lox(printer);
    lox.interpret(stmts);
    expect(buffer).toEqual(["hi", "yes", "nil", "nil"]);
  });

  it("increments variables", () => {
    const buffer: any[] = [];
    const printer = (...args: any) => buffer.push(...args);
    const scanner = new Scanner(`
var a = 1;
a = a + 1;
print a;
      `);
    const tokens = scanner.scan();
    const parser = new Parser(tokens);
    const stmts = parser.parse();
    const lox = new Lox(printer);
    lox.interpret(stmts);
    expect(buffer).toEqual(["2"]);
  });

  it("loops a while", () => {
    const buffer: any[] = [];
    const printer = (...args: any) => buffer.push(...args);
    const scanner = new Scanner(`
var i = 0;
while (i < 10) {
print i;
i = i + 1;
}
 `);
    const tokens = scanner.scan();
    const parser = new Parser(tokens);
    const stmts = parser.parse();
    const lox = new Lox(printer);
    lox.interpret(stmts);
    expect(buffer).toEqual(["0", "1", "2", "3", "4", "5", "6", "7", "8", "9"]);
  });

  it("excutes for loop", () => {
    const buffer: any[] = [];
    const printer = (...args: any) => buffer.push(...args);
    const scanner = new Scanner(`
var a = 0;
var temp;
for (var b = 1; a < 100; b = temp + b) {
print a;
temp = a;
a = b;
}
 `);
    const tokens = scanner.scan();
    const parser = new Parser(tokens);
    const stmts = parser.parse();
    const lox = new Lox(printer);
    lox.interpret(stmts);
    expect(buffer).toEqual([
      "0",
      "1",
      "1",
      "2",
      "3",
      "5",
      "8",
      "13",
      "21",
      "34",
      "55",
      "89",
    ]);
  });

  it("creates a function on global environment", () => {
    const buffer: any[] = [];
    const printer = (...args: any) => buffer.push(...args);
    const scanner = new Scanner(`
fun sayHi(first, last) {
print "Hi, " + first + " " + last + "!";
}
 `);
    const tokens = scanner.scan();
    const parser = new Parser(tokens);
    const stmts = parser.parse();
    const lox = new Lox(printer);
    lox.interpret(stmts);
    expect(Array.from(lox.globals.values.keys())).toHaveLength(2);
    expect(lox.globals.get("sayHi")).toBeTruthy();
    expect(lox.globals.get("sayHi")).toBeInstanceOf(CallableFunction);
  });

  it("creates a function that we can call", () => {
    const buffer: any[] = [];
    const printer = (...args: any) => buffer.push(...args);
    const scanner = new Scanner(`
fun sayHi(first, last) {
print "Hi, " + first + " " + last + "!";
}
sayHi("Dear", "Reader");
 `);
    const tokens = scanner.scan();
    const parser = new Parser(tokens);
    const stmts = parser.parse();
    const lox = new Lox(printer);
    lox.interpret(stmts);
    expect(buffer).toEqual(["Hi, Dear Reader!"]);
  });

  it("returns from a function call", () => {
    const buffer: any[] = [];
    const printer = (...args: any) => buffer.push(...args);
    const scanner = new Scanner(`
fun fib(n) {
  if(n==2) return 3;
return n+3;
}

print fib(2);
 `);
    const tokens = scanner.scan();
    const parser = new Parser(tokens);
    const stmts = parser.parse();
    const lox = new Lox(printer);
    lox.interpret(stmts);
    expect(buffer).toEqual(["3"]);
  });

  it("returns from a recursive function call", () => {
    const buffer: any[] = [];
    const printer = (...args: any) => buffer.push(...args);
    const scanner = new Scanner(`
fun fib(n) {
if (n <= 1) return n;
return fib(n - 2) + fib(n - 1);
}

for (var i = 0; i < 20; i = i + 1) {
print fib(i);
}
 `);
    const tokens = scanner.scan();
    const parser = new Parser(tokens);
    const stmts = parser.parse();
    const lox = new Lox(printer);
    lox.interpret(stmts);
    expect(buffer).toEqual([
      "0",
      "1",
      "1",
      "2",
      "3",
      "5",
      "8",
      "13",
      "21",
      "34",
      "55",
      "89",
      "144",
      "233",
      "377",
      "610",
      "987",
      "1597",
      "2584",
      "4181",
    ]);
  });

  it("can enclose functions", () => {
    const buffer: any[] = [];
    const printer = (...args: any) => buffer.push(...args);
    const scanner = new Scanner(`
fun makeCounter() {
var i = 0;
fun count() {
i = i + 1;
print i;
}
return count;
}
var counter = makeCounter();
counter();
counter();
 `);
    const tokens = scanner.scan();
    const parser = new Parser(tokens);
    const stmts = parser.parse();
    const lox = new Lox(printer);
    lox.interpret(stmts);
    expect(buffer).toEqual(["1", "2"]);
  });

  it("executes lambda functions", () => {
    const buffer: any[] = [];
    const printer = (...args: any) => buffer.push(...args);
    const scanner = new Scanner(`
fun thrice(fn){
  for (var i = 1; i <= 3; i = i + 1) {
    fn(i);
  }
}

thrice(fun (a) {
  print a;
});
 `);
    const tokens = scanner.scan();
    const parser = new Parser(tokens);
    const stmts = parser.parse();
    const lox = new Lox(printer);
    lox.interpret(stmts);
    expect(buffer).toEqual(["1", "2", "3"]);
  });

  it("executes for loop", () => {
    const buffer: any[] = [];
    const printer = (...args: any) => buffer.push(...args);
    const scanner = new Scanner(`
for (var i = 1; i <= 3; i = i + 1) {
  print i;
}
 `);
    const tokens = scanner.scan();
    const parser = new Parser(tokens);
    const stmts = parser.parse();
    const lox = new Lox(printer);
    lox.interpret(stmts);
    expect(buffer).toEqual(["1", "2", "3"]);
  });
});
