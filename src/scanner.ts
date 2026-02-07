import {
  Error,
  Token,
  TokenType,
  Literal,
  isDigit,
  isAlphaNumeric,
  keywords,
} from "./common";
import { errors } from "./errors";

export class Scanner {
  current: number = 0;
  start: number = 0;
  line: number = 1;
  tokens: Token[] = [];

  constructor(private readonly source: string) {}

  protected get isAtEnd(): boolean {
    return this.current >= this.source.length;
  }

  protected advance(): string {
    const char = this.source.charAt(this.current);
    this.current += 1;
    return char;
  }

  protected get peek(): string {
    if (this.isAtEnd) return "\0";
    return this.source.charAt(this.current);
  }

  protected get peekNext(): string {
    if (this.current + 1 >= this.source.length) return "\0";
    return this.source.charAt(this.current + 1);
  }

  protected match(char: string): boolean {
    if (this.isAtEnd) return false;
    if (this.source.charAt(this.current) != char) return false;
    this.current += 1;
    return true;
  }

  protected addToken(token: TokenType) {
    this.tokens.push(new Token(token, this.line));
  }

  protected addTokenLiteral(token: TokenType, literal: Literal) {
    const lexeme = this.source.substring(this.start, this.current);
    this.tokens.push(new Token(token, this.line, lexeme, literal));
  }

  protected addIdentifier() {
    const lexeme = this.source.substring(this.start, this.current);
    this.tokens.push(new Token(TokenType.IDENTIFIER, this.line, lexeme));
  }

  protected string() {
    const newline: string = "\n";
    while (this.peek != '"' && !this.isAtEnd) {
      if (this.peek == newline) this.line++;
      this.advance();
    }
    if (this.isAtEnd) {
      errors.push(new Error(this.line, "Unterminated string"));
      return;
    }
    this.advance();
    const value = this.source.substring(this.start + 1, this.current - 1);
    this.addTokenLiteral(TokenType.STRING, value);
  }

  protected number() {
    while (isDigit(this.peek)) this.advance();
    if (this.peek == "." && isDigit(this.peekNext)) {
      this.advance();
      while (isDigit(this.peek)) this.advance();
    }
    const number = Number.parseFloat(
      this.source.substring(this.start, this.current),
    );
    this.addTokenLiteral(TokenType.NUMBER, number);
  }

  protected identifier() {
    while (isAlphaNumeric(this.peek)) this.advance();
    const value = this.source.substring(this.start, this.current);
    if (keywords[value]) {
      this.addToken(keywords[value]);
    } else {
      this.addIdentifier();
    }
  }

  protected scanToken() {
    const char = this.advance();
    switch (char) {
      case "(":
        this.addToken(TokenType.LEFT_PAREN);
        break;
      case ")":
        this.addToken(TokenType.RIGHT_PAREN);
        break;
      case "{":
        this.addToken(TokenType.LEFT_BRACE);
        break;
      case "}":
        this.addToken(TokenType.RIGHT_BRACE);
        break;
      case ",":
        this.addToken(TokenType.COMMA);
        break;
      case ".":
        this.addToken(TokenType.DOT);
        break;
      case "-":
        this.addToken(TokenType.MINUS);
        break;
      case "+":
        this.addToken(TokenType.PLUS);
        break;
      case ";":
        this.addToken(TokenType.SEMICOLON);
        break;
      case "*":
        this.addToken(TokenType.STAR);
        break;
      case "!":
        this.addToken(this.match("=") ? TokenType.BANG_EQUAL : TokenType.BANG);
        break;
      case "=":
        this.addToken(
          this.match("=") ? TokenType.EQUAL_EQUAL : TokenType.EQUAL,
        );
        break;
      case "<":
        this.addToken(this.match("=") ? TokenType.LESS_EQUAL : TokenType.LESS);
        break;
      case ">":
        this.addToken(
          this.match("=") ? TokenType.GREATER_EQUAL : TokenType.GREATER,
        );
        break;
      case "/":
        if (this.match("/")) {
          while (this.peek != "\n" && !this.isAtEnd) this.advance();
        } else {
          this.addToken(TokenType.SLASH);
        }
        break;
      case " ":
      case "\r":
      case "\t":
        break;
      case "\n":
        this.line++;
        break;
      case '"':
        this.string();
        break;
      case "0":
      case "1":
      case "2":
      case "3":
      case "4":
      case "5":
      case "6":
      case "7":
      case "8":
      case "9":
        this.number();
        break;
      case "a":
      case "b":
      case "c":
      case "d":
      case "e":
      case "f":
      case "g":
      case "h":
      case "i":
      case "j":
      case "k":
      case "l":
      case "m":
      case "n":
      case "o":
      case "p":
      case "q":
      case "r":
      case "s":
      case "t":
      case "u":
      case "v":
      case "w":
      case "x":
      case "y":
      case "z":
      case "A":
      case "B":
      case "C":
      case "D":
      case "E":
      case "F":
      case "G":
      case "H":
      case "I":
      case "J":
      case "K":
      case "L":
      case "M":
      case "N":
      case "O":
      case "P":
      case "Q":
      case "R":
      case "S":
      case "T":
      case "U":
      case "V":
      case "W":
      case "X":
      case "Y":
      case "Z":
      case "_":
        this.identifier();
        break;
      default:
        errors.push(new Error(this.line, "Unexpected character."));
        break;
    }
  }

  scan(): Token[] {
    while (!this.isAtEnd) {
      this.start = this.current;
      this.scanToken();
    }
    this.tokens.push(new Token(TokenType.EOF, this.line, "", undefined));
    return this.tokens;
  }
}
