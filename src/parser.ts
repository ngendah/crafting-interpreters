import {
  Assign,
  Binary,
  Block,
  Break,
  Call,
  Continue,
  Expression,
  Function,
  Grouping,
  If,
  Lambda,
  Literal,
  Logical,
  Print,
  Return,
  Unary,
  Var,
  Variable,
  While,
} from "./ast";
import {
  Error,
  Expr,
  Stmt,
  Token,
  TokenType,
  ParseError,
  FunctionKind,
} from "./common";

export class Parser {
  current: number = 0;
  constructor(protected readonly tokens: Token[]) {}

  get isAtEnd() {
    return this.peek().type == TokenType.EOF;
  }

  get previous(): Token {
    const token: Token | undefined = this.tokens.at(this.current - 1);
    if (!token) throw new Error(-1, `no tokens at index ${this.current - 1}.`);
    return token;
  }

  peek(): Token {
    const token: Token | undefined = this.tokens.at(this.current);
    if (!token) throw new Error(-1, `no tokens at index ${this.current}.`);
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

  assignment(): Expr {
    const expr = this.or();
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

  or(): Expr {
    let expr = this.and();
    while (this.match(TokenType.OR)) {
      const operator = this.previous;
      const right = this.and();
      expr = new Logical(expr, operator, right);
    }
    return expr;
  }

  and(): Expr {
    let expr = this.equality();
    while (this.match(TokenType.AND)) {
      const operator = this.previous;
      const right = this.equality();
      expr = new Logical(expr, operator, right);
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
    return this.call();
  }

  call(): Expr {
    let expr = this.primary();
    while (true) {
      if (this.match(TokenType.LEFT_PAREN)) {
        expr = this.finishCall(expr);
      } else {
        break;
      }
    }
    return expr;
  }

  finishCall(callee: Expr): Expr {
    const args: Expr[] = [];
    if (!this.check(TokenType.RIGHT_PAREN)) {
      do {
        if (args.length >= 255) {
          this.error(this.peek(), "Cant have more than 255 arguments.");
        }
        if (this.match(TokenType.FUN)) {
          const index = args.length > 0 ? args.length - 1 : 0;
          args.push(this.function("lambda", `${index}`));
        } else {
          args.push(this.expression());
        }
      } while (this.match(TokenType.COMMA));
    }
    const paren = this.consume(
      TokenType.RIGHT_PAREN,
      "Expect ')' after arguments.",
    );
    return new Call(callee, paren, args);
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
      this.consume(TokenType.RIGHT_PAREN, "Expect ) after expression.");
      return new Grouping(expr);
    }
    throw this.error(this.peek(), "Expect expression.");
  }

  consume(type: TokenType, message: string): Token {
    if (this.check(type)) return this.advance();
    throw this.error(this.peek(), message);
  }

  error(token: Token, message: string) {
    if (token.type == TokenType.EOF) {
      console.error(token.line, " at end", message);
    } else {
      console.error(token.line, ` at line '${token.line}'`, message);
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

  statement(): Stmt {
    if (this.match(TokenType.PRINT)) return this.printStatement();
    if (this.match(TokenType.LEFT_BRACE)) return new Block(this.block());
    if (this.match(TokenType.IF)) return this.ifStmt();
    if (this.match(TokenType.WHILE)) return this.whileStmt();
    if (this.match(TokenType.FOR)) return this.forStmt();
    if (this.match(TokenType.BREAK)) return this.breakStmt();
    if (this.match(TokenType.CONTINUE)) return this.continueStmt();
    if (this.match(TokenType.RETURN)) return this.returnStmt();
    return this.expressionStatement();
  }

  printStatement(): Stmt {
    const value: Expr = this.expression();
    this.consume(TokenType.SEMICOLON, "Expect ';' after value.");
    return new Print(value);
  }

  expressionStatement(): Stmt {
    const expr: Expr = this.expression();
    this.consume(TokenType.SEMICOLON, "Expect ';' after value.");
    return new Expression(expr);
  }

  declaration(): Stmt | null {
    try {
      if (this.match(TokenType.FUN)) return this.function("function");
      if (this.match(TokenType.VAR)) return this.varDeclaration();
      return this.statement();
    } catch {
      this.synchronize();
      return null;
    }
  }

  function(kind: FunctionKind, id?: string): Function {
    let name: Token;
    if (this.check(TokenType.IDENTIFIER)) {
      name = this.consume(TokenType.IDENTIFIER, `Expect ${kind} name.`);
    } else {
      name = new Token(TokenType.IDENTIFIER, this.current, id);
    }
    this.consume(TokenType.LEFT_PAREN, `Expect '(' before ${kind} parameters.`);
    const params: Token[] = [];
    if (!this.check(TokenType.RIGHT_PAREN)) {
      do {
        const param = this.consume(
          TokenType.IDENTIFIER,
          "Expect parameter name.",
        );
        params.push(param);
      } while (this.match(TokenType.COMMA));
    }
    this.consume(TokenType.RIGHT_PAREN, "Expect ')' after parameters.");
    this.consume(TokenType.LEFT_BRACE, `Expect '{' before ${kind} body.`);
    const body = this.block();
    return kind == "lambda"
      ? new Lambda(name, params, body)
      : new Function(name, params, body);
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
    this.consume(TokenType.SEMICOLON, "Expect ';' after variable declaration.");
    return new Var(name, initializer);
  }

  block(): Stmt[] {
    const statements: Stmt[] = [];
    while (!this.check(TokenType.RIGHT_BRACE) && !this.isAtEnd) {
      const decl = this.declaration();
      if (decl) statements.push(decl);
    }
    this.consume(TokenType.RIGHT_BRACE, "Expect ';' after block.");
    return statements;
  }

  ifStmt(): Stmt {
    this.consume(TokenType.LEFT_PAREN, "Expect ')' after 'if'.");
    const condition = this.expression();
    this.consume(TokenType.RIGHT_PAREN, "Expect ')' after if condition.");
    const thenBranch: Stmt = this.statement();
    let elseBranch: Stmt | undefined;
    if (this.match(TokenType.ELSE)) {
      elseBranch = this.statement();
    }
    return new If(condition, thenBranch, elseBranch);
  }

  whileStmt(): Stmt {
    this.consume(TokenType.LEFT_PAREN, "Expect '(' after 'while'.");
    const condition: Expr = this.expression();
    this.consume(TokenType.RIGHT_PAREN, "Expect ')' after condition.");
    const body = this.statement();
    return new While(condition, body);
  }

  forStmt(): Stmt {
    this.consume(TokenType.LEFT_PAREN, "Expect '(' after 'for'.");
    let initializer: Stmt | null;
    if (this.match(TokenType.SEMICOLON)) {
      initializer = null;
    } else if (this.match(TokenType.VAR)) {
      initializer = this.varDeclaration();
    } else {
      initializer = this.expressionStatement();
    }
    let condition: Expr | null = null;
    if (!this.check(TokenType.SEMICOLON)) {
      condition = this.expression();
    }
    this.consume(TokenType.SEMICOLON, "Expect ';' after loop condition.");
    let increment: Expr | null = null;
    if (!this.check(TokenType.RIGHT_PAREN)) {
      increment = this.expression();
    }
    this.consume(TokenType.RIGHT_PAREN, "Expect ')' after for clauses.");
    let body = this.statement();
    if (increment) {
      body = new Block([body, new Expression(increment)]);
    }
    if (!condition) condition = new Literal(true);
    body = new While(condition, body);
    if (initializer) body = new Block([initializer, body]);
    return body;
  }

  breakStmt(): Stmt {
    this.consume(TokenType.SEMICOLON, "Expect ';' after break clause.");
    return new Break();
  }

  continueStmt(): Stmt {
    this.consume(TokenType.SEMICOLON, "Expect ';' after continue clause.");
    return new Continue();
  }

  returnStmt(): Stmt {
    const keyword = this.previous;
    let value: Expr = new Literal(null);
    if (!this.check(TokenType.SEMICOLON)) {
      value = this.expression();
    }
    this.consume(TokenType.SEMICOLON, "Expect ';' after return value.");
    return new Return(keyword, value);
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
