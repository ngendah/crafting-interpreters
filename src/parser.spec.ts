import { Parser } from "./parser";
import { Token, TokenType } from "./common";
import { Scanner } from "./scanner";

describe("Parser", () => {
  describe("util methods", () => {
    it("isAtEnd", () => {
      const parser = new Parser([new Token(TokenType.EOF, 1)]);
      expect(parser.isAtEnd).toEqual(true);
    });

    it("previous", () => {
      const parser = new Parser([
        new Token(TokenType.NUMBER, 1, "0.123", 0.123),
        new Token(TokenType.COMMA, 1, ";"),
        new Token(TokenType.EOF, 1),
      ]);
      const previous = parser.advance();
      expect(previous.type).toEqual(parser.previous.type);
    });

    it("peek", () => {
      const parser = new Parser([
        new Token(TokenType.NUMBER, 1, "0.123", 0.123),
        new Token(TokenType.COMMA, 1, ";"),
        new Token(TokenType.EOF, 1),
      ]);
      expect(parser.peek().type).toEqual(TokenType.NUMBER);
      parser.advance();
      expect(parser.peek().type).toEqual(TokenType.COMMA);
    });

    it("check", () => {
      const parser = new Parser([
        new Token(TokenType.NUMBER, 1, "0.123", 0.123),
        new Token(TokenType.COMMA, 1, ";"),
        new Token(TokenType.EOF, 1),
      ]);
      expect(parser.check(TokenType.NUMBER)).toEqual(true);
      parser.advance();
      expect(parser.check(TokenType.COMMA)).toEqual(true);
      parser.advance();
      expect(parser.check(TokenType.EOF)).toEqual(false);
    });

    it("match", () => {
      const parser = new Parser([
        new Token(TokenType.NUMBER, 1, "0.123", 0.123),
        new Token(TokenType.COMMA, 1, ";"),
        new Token(TokenType.EOF, 1),
      ]);
      expect(parser.match(TokenType.NUMBER)).toEqual(true);
      expect(parser.match(TokenType.COMMA)).toEqual(true);
      expect(parser.isAtEnd).toEqual(true);
    });
  });

  describe("parse()", () => {
    it("parses EOF token", () => {
      const parser = new Parser([new Token(TokenType.EOF, 1)]);
      const stmts = parser.parse();
      expect(stmts.length).toEqual(0);
    });

    describe("primary expression", () => {
      it("parses a number", () => {
        const scanner = new Scanner("0.123;");
        const tokens = scanner.scan();
        const parser = new Parser(tokens);
        const stmts = parser.parse();
        expect(stmts).toHaveLength(1);
      });

      it("parses boolean", () => {
        const scanner = new Scanner("true;");
        const tokens = scanner.scan();
        const parser = new Parser(tokens);
        const stmts = parser.parse();
        expect(stmts).toHaveLength(1);
      });

      it("parses nil", () => {
        const scanner = new Scanner("nil;");
        const tokens = scanner.scan();
        const parser = new Parser(tokens);
        const stmts = parser.parse();
        expect(stmts).toHaveLength(1);
      });

      it("parses an identifier", () => {
        const scanner = new Scanner("_analygator;");
        const tokens = scanner.scan();
        const parser = new Parser(tokens);
        const stmts = parser.parse();
        expect(stmts).toHaveLength(1);
      });

      it("parses a group", () => {
        const scanner = new Scanner("(0.123);");
        const tokens = scanner.scan();
        const parser = new Parser(tokens);
        const stmts = parser.parse();
        expect(stmts).toHaveLength(1);
      });
    });

    describe("unary expression", () => {
      it("parses a negative number", () => {
        const scanner = new Scanner("-0.123;");
        const tokens = scanner.scan();
        const parser = new Parser(tokens);
        const stmts = parser.parse();
        expect(stmts).toHaveLength(1);
      });
    });
  });

  it("builds up get calls", () => {
    const scanner = new Scanner("egg.scramble(3).with(cheddar);");
    const tokens = scanner.scan();
    const parser = new Parser(tokens);
    const stmts = parser.parse();
    expect(stmts).toHaveLength(1);
  });
});
