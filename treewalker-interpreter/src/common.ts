export const enum TokenType {
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

export abstract class Instance {}
export abstract class CallableFn {}
export type Literal = number | string | boolean | null;
export type Value = string | number | boolean | CallableFn | Instance | null;
export const enum FunctionKind {
  FUNCTION = "function",
  LAMBDA = "lambda",
  METHOD = "method",
}
export type ClassKind = "class";

export function isTruthy<T>(value: unknown): value is T {
  if (typeof value == "boolean") return value;
  if (typeof value == "undefined") return false;
  return value != null;
}

export class Token {
  constructor(
    public readonly type: TokenType,
    public readonly line: number,
    public readonly lexeme?: string,
    public readonly literal?: Literal,
  ) {}

  toString() {
    return `${this.lexeme}`;
  }
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

export class Stack<T> {
  protected items: T[] = [];
  constructor() {}

  get peek(): T | undefined {
    if (this.isEmpty) return;
    return this.items.at(this.items.length - 1);
  }

  get isEmpty(): boolean {
    return this.items.length == 0;
  }

  get isNotEmpty(): boolean {
    return !this.isEmpty;
  }

  push(item: T) {
    this.items.push(item);
  }

  pop(): T | undefined {
    return this.items.pop();
  }

  get length(): number {
    return this.items.length;
  }

  at(index: number): T | undefined {
    return this.items.at(index);
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
