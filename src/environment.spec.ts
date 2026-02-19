import { Token, TokenType, Value } from "./common";
import Environment from "./environment";

describe("Environment", () => {
  it("defines a new key-value", () => {
    const env = new Environment<Value>();
    env.define(new Token(TokenType.IDENTIFIER, 1, "a"), 2);
    expect(env.values.get("a")).toEqual(2);
  });

  it("assigns to an existing key", () => {
    const env = new Environment<Value>();
    env.values.set("b", 3);
    const token = new Token(TokenType.IDENTIFIER, 1, "b");
    env.assign(token, 1);
    expect(env.get(token)).toEqual(1);
  });

  describe("Enclosing environment", () => {
    it("assigns to an enclosing environment", () => {
      const enclosing = new Environment<Value>();
      const token = new Token(TokenType.IDENTIFIER, 1, "a");
      enclosing.define(token, 2);
      const env = new Environment<Value>(enclosing);
      env.assign(token, 3);
      expect(env.get(token)).toEqual(3);
    });
  });

  it("fails to get an ancestor, if they dont exist", () => {
    const env = new Environment<Value>();
    expect(env.ancestor(1)).toBeUndefined();
  });

  it("returns 'this' at a distance of 0", () => {
    const env = new Environment<Value>();
    expect(env.ancestor(0)).toEqual(env);
  });

  it("returns a value at a distance", () => {
    const env = new Environment<Value>();
    {
      const env1 = new Environment<Value>(env);
      const token = new Token(TokenType.IDENTIFIER, 1, "a");
      env1.define(token, 2);
      {
        const env2 = new Environment<Value>(env1);
        expect(env2.at(1, token)).toEqual(2);
      }
    }
  });

  it("assigns a value at a distance", () => {
    const env = new Environment<Value>();
    {
      const env1 = new Environment<Value>(env);
      const token = new Token(TokenType.IDENTIFIER, 1, "a");
      env1.define(token, 2);
      {
        const env2 = new Environment<Value>(env1);
        env2.assignAt(1, token, 3);
        expect(env2.at(1, token)).toEqual(3);
      }
    }
  });
});
