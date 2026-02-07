import { Token, TokenType } from "./common";
import Environment from "./environment";

describe("Environment", () => {
  it("defines a new key-value", () => {
    const env = new Environment();
    env.define(new Token(TokenType.IDENTIFIER, 1, "a"), 2);
    expect(env.values.get("a")).toEqual(2);
  });

  it("assigns to an existing key", () => {
    const env = new Environment();
    env.values.set("b", 3);
    const token = new Token(TokenType.IDENTIFIER, 1, "b");
    env.assign(token, 1);
    expect(env.getToken(token)).toEqual(1);
  });
});
