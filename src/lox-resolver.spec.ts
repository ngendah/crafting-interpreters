import { Lox, LoxWithResolver } from "./lox";
import { Parser } from "./parser";
import { Scanner } from "./scanner";

describe("LoxWithResolver", () => {
  //   it("fails to execute bad-boy", () => {
  //     const buffer: any[] = [];
  //     const printer = (...args: any) => buffer.push(...args);
  //     const scanner = new Scanner(`
  //    fun bad() {
  //    var a = "first";
  //    var a = "second";
  //    }
  //     `);
  //     const tokens = scanner.scan();
  //     const parser = new Parser(tokens);
  //     const stmts = parser.parse();
  //     const lox = new LoxWithResolver(printer);
  //     lox.interpret(stmts);
  //   });

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
    const lox = new LoxWithResolver(printer);
    lox.interpret(stmts);
    expect(buffer).toEqual(["2"]);
  });

  it("executes for statements", () => {
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
    const lox = new LoxWithResolver(printer);
    lox.interpret(stmts);
    expect(buffer).toEqual(["1", "2", "3"]);
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

  it("resolves this", () => {
    const buffer: any[] = [];
    const printer = (...args: any) => buffer.push(...args);
    const scanner = new Scanner(`
class Cake {
  taste() {
    var adjective = "delicious";
    print "The " + this.flavor + " cake is " + adjective + "!";
  }
}
var cake = Cake();
cake.flavor = "German chocolate";
cake.taste();
 `);
    const tokens = scanner.scan();
    const parser = new Parser(tokens);
    const stmts = parser.parse();
    const lox = new LoxWithResolver(printer);
    lox.interpret(stmts);
    expect(buffer).toEqual(["The German chocolate cake is delicious!"]);
  });

  it("init initializes a class", () => {
    const buffer: any[] = [];
    const printer = (...args: any) => buffer.push(...args);
    const scanner = new Scanner(`
class Foo {
  init() {
    print this;
  }
}
var foo = Foo();
 `);
    const tokens = scanner.scan();
    const parser = new Parser(tokens);
    const stmts = parser.parse();
    const lox = new LoxWithResolver(printer);
    lox.interpret(stmts);
    expect(buffer).toEqual(["Foo"]);
  });

  it("calls class initializer", () => {
    const buffer: any[] = [];
    const printer = (...args: any) => buffer.push(...args);
    const scanner = new Scanner(`
class Circle {
  init(radius) {
    this.radius = radius;
  }

  area() {
    return 3.141592654 * this.radius * this.radius;
  }
}

var circle = Circle(4);
print circle.area();
 `);
    const tokens = scanner.scan();
    const parser = new Parser(tokens);
    const stmts = parser.parse();
    const lox = new LoxWithResolver(printer);
    lox.interpret(stmts);
    expect(buffer).toEqual(["50.265482464"]);
  });
});
