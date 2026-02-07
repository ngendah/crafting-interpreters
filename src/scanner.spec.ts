import { Scanner } from "./scanner";
import { TokenType } from "./common";

describe("Scanner", () => {
  it('scan ""', () => {
    const scanner = new Scanner(``);
    const tokens = scanner.scan();
    expect(tokens.length).toEqual(1);
  });

  it("scans operators", () => {
    const scanner = new Scanner("(){}!*+-/=<>,; <= == >=");
    const tokens = scanner.scan();
    expect(tokens.length).toEqual(18);
    expect(tokens[0].type).toEqual(TokenType.LEFT_PAREN);
    expect(tokens[1].type).toEqual(TokenType.RIGHT_PAREN);
    expect(tokens[2].type).toEqual(TokenType.LEFT_BRACE);
    expect(tokens[3].type).toEqual(TokenType.RIGHT_BRACE);
    expect(tokens[4].type).toEqual(TokenType.BANG);
    expect(tokens[5].type).toEqual(TokenType.STAR);
    expect(tokens[6].type).toEqual(TokenType.PLUS);
    expect(tokens[7].type).toEqual(TokenType.MINUS);
    expect(tokens[8].type).toEqual(TokenType.SLASH);
    expect(tokens[9].type).toEqual(TokenType.EQUAL);
    expect(tokens[10].type).toEqual(TokenType.LESS);
    expect(tokens[11].type).toEqual(TokenType.GREATER);
    expect(tokens[12].type).toEqual(TokenType.COMMA);
    expect(tokens[13].type).toEqual(TokenType.SEMICOLON);
    expect(tokens[14].type).toEqual(TokenType.LESS_EQUAL);
    expect(tokens[15].type).toEqual(TokenType.EQUAL_EQUAL);
    expect(tokens[16].type).toEqual(TokenType.GREATER_EQUAL);
    expect(tokens[17].type).toEqual(TokenType.EOF);
  });

  it("skips empty lines", () => {
    const scanner = new Scanner(`\n\n\t`);
    const tokens = scanner.scan();
    expect(tokens.length).toEqual(1);
  });

  it("scans strings", () => {
    const scanner = new Scanner('"craft"');
    const tokens = scanner.scan();
    expect(tokens.length).toEqual(2);
    expect(tokens[0].type).toEqual(TokenType.STRING);
    expect(tokens[1].type).toEqual(TokenType.EOF);
  });

  describe("identifiers", () => {
    it("scans an identifier", () => {
      const scanner = new Scanner("craft");
      const tokens = scanner.scan();
      expect(tokens.length).toEqual(2);
      expect(tokens[0].type).toEqual(TokenType.IDENTIFIER);
      expect(tokens[1].type).toEqual(TokenType.EOF);
    });

    it("scans identifier starting with _", () => {
      const scanner = new Scanner("_analygator");
      const tokens = scanner.scan();
      expect(tokens.length).toEqual(2);
      expect(tokens[0].type).toEqual(TokenType.IDENTIFIER);
      expect(tokens[1].type).toEqual(TokenType.EOF);
    });
  });

  it("skips comments", () => {
    const scanner = new Scanner("//this is a comment");
    const tokens = scanner.scan();
    expect(tokens.length).toEqual(1);
    expect(tokens[0].type).toEqual(TokenType.EOF);
  });

  describe("numbers", () => {
    it("scans integers", () => {
      const scanner = new Scanner("123");
      const tokens = scanner.scan();
      expect(tokens.length).toEqual(2);
      expect(tokens[0].type).toEqual(TokenType.NUMBER);
      expect(tokens[1].type).toEqual(TokenType.EOF);
    });

    it("scans floats", () => {
      const scanner = new Scanner("0.123");
      const tokens = scanner.scan();
      expect(tokens.length).toEqual(2);
      expect(tokens[0].type).toEqual(TokenType.NUMBER);
      expect(tokens[1].type).toEqual(TokenType.EOF);
    });

    it("scans negative floats", () => {
      const scanner = new Scanner("-0.123");
      const tokens = scanner.scan();
      expect(tokens.length).toEqual(3);
      expect(tokens[0].type).toEqual(TokenType.MINUS);
      expect(tokens[1].type).toEqual(TokenType.NUMBER);
      expect(tokens[2].type).toEqual(TokenType.EOF);
    });
  });
});
