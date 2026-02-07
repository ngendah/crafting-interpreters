import Interpreter from "./interpreter";
import { Parser } from "./parser";
import { Scanner } from "./scanner";

describe("Interpreter", () => {
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
    const interpreter = new Interpreter(printer);
    interpreter.interpret(stmts);
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
    const interpreter = new Interpreter(printer);
    interpreter.interpret(stmts);
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
    const interpreter = new Interpreter(printer);
    interpreter.interpret(stmts);
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
    const interpreter = new Interpreter(printer);
    interpreter.interpret(stmts);
    expect(buffer).toEqual(["2"]);
  });
});
