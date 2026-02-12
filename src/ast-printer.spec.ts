import { AstPrinter } from "./ast-printer";
import { Binary, Grouping, Literal, Unary } from "./ast";
import { Token, TokenType } from "./common";

describe("AstPrinter", () => {
  it("prints", () => {
    const printer = new AstPrinter();
    const expr = new Binary(
      new Unary(new Token(TokenType.MINUS, 1, "-"), new Literal(123)),
      new Token(TokenType.STAR, 1, "*"),
      new Grouping(new Literal(45.67)),
    );
    expect(printer.print(expr)).toEqual("(* (- 123) (group 45.67))");
  });
});
