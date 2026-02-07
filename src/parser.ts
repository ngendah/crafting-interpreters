import Binary from "./ast/binary";
import { Expr, Stmt } from "./ast/base";
import Grouping from "./ast/grouping";
import Literal from "./ast/literal";
import Unary from "./ast/unary";
import { Error, Token, TokenType, ParseError } from "./common";
import Print from "./ast/print";
import Expression from "./ast/expression";
import Var from "./ast/var";
import Variable from "./ast/variable";
import Assign from "./ast/assign";

export class Parser {
  current: number = 0;
  constructor(protected readonly tokens: Token[]) {}

  get isAtEnd() {
    return this.peek().type == TokenType.EOF;
  }

  get previous(): Token {
    const token: Token | undefined = this.tokens.at(this.current - 1);
    if (!token) throw new Error(-1, `no tokens at index ${this.current - 1}`);
    return token;
  }

  peek(): Token {
    const token: Token | undefined = this.tokens.at(this.current);
    if (!token) throw new Error(-1, `no tokens at index ${this.current}`);
    return token;
  }

  advance(): Token {
    if (!this.isAtEnd) this.current += 1;
    return this.previous;
  }

  check(type: TokenType): boolean {
    if (this.isAtEnd) return false;
    return this.peek().type == type;
  }

  match(...types: TokenType[]): boolean {
    for (const type of types) {
      if (this.check(type)) {
        this.advance();
        return true;
      }
    }
    return false;
  }

  expression(): Expr {
    return this.assignment();
  }

  assignment() {
    const expr = this.equality();
    if (this.match(TokenType.EQUAL)) {
      const equals = this.previous;
      const value = this.assignment();
      if (expr instanceof Variable) {
        const name = expr.name;
        return new Assign(name, value);
      }
      this.error(equals, "Invalid assignment target.");
    }
    return expr;
  }

  equality(): Expr {
    let expr: Expr = this.comparison();
    while (this.match(TokenType.BANG_EQUAL, TokenType.EQUAL_EQUAL)) {
      const operator: Token = this.previous;
      const right: Expr = this.comparison();
      expr = new Binary(expr, operator, right);
    }
    return expr;
  }

  comparison(): Expr {
    let expr = this.term();
    while (
      this.match(
        TokenType.GREATER,
        TokenType.GREATER_EQUAL,
        TokenType.LESS,
        TokenType.LESS_EQUAL,
      )
    ) {
      const operator: Token = this.previous;
      const right: Expr = this.term();
      expr = new Binary(expr, operator, right);
    }
    return expr;
  }

  term(): Expr {
    let expr = this.factor();
    while (this.match(TokenType.MINUS, TokenType.PLUS)) {
      const operator = this.previous;
      const right: Expr = this.factor();
      expr = new Binary(expr, operator, right);
    }
    return expr;
  }

  factor(): Expr {
    let expr = this.unary();
    while (this.match(TokenType.SLASH, TokenType.STAR)) {
      const operator = this.previous;
      const right: Expr = this.unary();
      expr = new Binary(expr, operator, right);
    }
    return expr;
  }

  unary(): Expr {
    if (this.match(TokenType.BANG, TokenType.MINUS)) {
      const operator = this.previous;
      const right: Expr = this.unary();
      return new Unary(operator, right);
    }
    return this.primary();
  }

  primary(): Expr {
    if (this.match(TokenType.FALSE)) return new Literal(false);
    if (this.match(TokenType.TRUE)) return new Literal(true);
    if (this.match(TokenType.NIL)) return new Literal(null);
    if (this.match(TokenType.NUMBER, TokenType.STRING)) {
      return new Literal(this.previous.literal!);
    }
    if (this.match(TokenType.IDENTIFIER)) {
      return new Variable(this.previous);
    }
    if (this.match(TokenType.LEFT_PAREN)) {
      const expr = this.expression();
      this.consume(TokenType.RIGHT_PAREN, "Expect ) after expression");
      return new Grouping(expr);
    }
    throw this.error(this.peek(), "Expect expression");
  }

  consume(type: TokenType, message: string) {
    if (this.check(type)) return this.advance();
    throw this.error(this.peek(), message);
  }

  error(token: Token, message: string) {
    if (token.type == TokenType.EOF) {
      console.error(token.line, " at end", message);
    } else {
      console.error(token.line, ` at '${token.line}'`, message);
    }
    return new ParseError();
  }

  synchronize() {
    this.advance();
    while (!this.isAtEnd) {
      if (this.previous.type == TokenType.SEMICOLON) return;
      switch (this.peek().type) {
        case TokenType.CLASS:
        case TokenType.FUN:
        case TokenType.VAR:
        case TokenType.FOR:
        case TokenType.IF:
        case TokenType.WHILE:
        case TokenType.PRINT:
        case TokenType.RETURN:
          return;
      }
      this.advance();
    }
  }

  printStatement(): Stmt {
    const value: Expr = this.expression();
    this.consume(TokenType.SEMICOLON, "Expect ';' after value");
    return new Print(value);
  }

  expressionStatement(): Stmt {
    const expr: Expr = this.expression();
    this.consume(TokenType.SEMICOLON, "Expect ';' after value");
    return new Expression(expr);
  }

  varDeclaration(): Stmt {
    const name: Token = this.consume(
      TokenType.IDENTIFIER,
      "Expect variable name",
    );
    let initializer: Expr = new Literal(null);
    if (this.match(TokenType.EQUAL)) {
      initializer = this.expression();
    }
    this.consume(TokenType.SEMICOLON, "Expect ';' after variable declaration");
    return new Var(name, initializer);
  }

  statement(): Stmt {
    if (this.match(TokenType.PRINT)) return this.printStatement();
    return this.expressionStatement();
  }

  declaration(): Stmt | null {
    try {
      if (this.match(TokenType.VAR)) return this.varDeclaration();
      return this.statement();
    } catch {
      this.synchronize();
      return null;
    }
  }

  parse(): Stmt[] {
    const statements: Stmt[] = [];
    while (!this.isAtEnd) {
      const declaration = this.declaration();
      if (declaration) statements.push(declaration);
    }
    return statements;
  }
}
