export enum TokenType {
  LEFT_PAREN = "left-paren",
  RIGHT_PAREN = "right-paren",
  LEFT_BRACE = "left-brace",
  RIGHT_BRACE = "right-brace",
  COMMA = "comma",
  DOT = "dot",
  MINUS = "minus",
  PLUS = "plus",
  SEMICOLON = "semicolon",
  SLASH = "slash",
  STAR = "star",
  BANG = "bang",
  EQUAL = "equal",
  GREATER = "greater",
  LESS = "less",
  BANG_EQUAL = "bang-equal",
  EQUAL_EQUAL = "equal-equal",
  GREATER_EQUAL = "greater-equal",
  LESS_EQUAL = "less-equal",
  IDENTIFIER = "identifier",
  STRING = "string",
  NUMBER = "number",
  AND = "and",
  CLASS = "class",
  ELSE = "else",
  FALSE = "false",
  FUN = "fun",
  FOR = "for",
  IF = "if",
  NIL = "nil",
  OR = "or",
  PRINT = "print",
  RETURN = "return",
  SUPER = "super",
  THIS = "this",
  TRUE = "true",
  VAR = "var",
  WHILE = "while",
  BREAK = "break",
  CONTINUE = "continue",
  EOF = "eof",
}

export const keywords: Record<string, TokenType> = {
  and: TokenType.AND,
  class: TokenType.CLASS,
  else: TokenType.ELSE,
  false: TokenType.FALSE,
  for: TokenType.FOR,
  fun: TokenType.FUN,
  if: TokenType.IF,
  nil: TokenType.NIL,
  or: TokenType.OR,
  print: TokenType.PRINT,
  return: TokenType.RETURN,
  super: TokenType.SUPER,
  this: TokenType.THIS,
  true: TokenType.TRUE,
  var: TokenType.VAR,
  while: TokenType.WHILE,
  break: TokenType.BREAK,
  continue: TokenType.CONTINUE,
};

export interface CallableFn {}
export type Literal = number | string | boolean | null;
export type Value = string | number | boolean | CallableFn | null;
export type FunctionKind = "function" | "lambda";

export class Token {
  constructor(
    public readonly type: TokenType,
    public readonly line: number,
    public readonly lexeme?: string,
    public readonly literal?: Literal,
  ) {}
}

export class Error {
  constructor(
    public readonly line: number,
    public readonly description: string,
  ) {}
}

export class ParseError {}

export class RuntimeError {
  constructor(
    public readonly name: Token | string,
    public readonly message: string,
  ) {}
}

export class ThrowableReturn {
  constructor(public readonly value: unknown) {}

  get<T>() {
    return this.value as T;
  }
}

export const isDigit = (char: string): boolean => char >= "0" && char <= "9";
export const isAlpha = (char: string): boolean =>
  (char >= "a" && char <= "z") || (char >= "A" && char <= "Z") || char == "_";
export const isAlphaNumeric = (char: string): boolean =>
  isDigit(char) || isAlpha(char);

export interface Expr {
  accept<T>(visitor: Visitor<T>): T;
}

export interface Stmt {
  accept<T>(visitor: Visitor<T>): T;
}

export interface Visitor<T> {
  visit(value: Expr | Stmt): T;
}
