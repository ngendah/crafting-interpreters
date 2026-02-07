import { RuntimeError, Token, Value } from "./common";

export default class Environment {
  values = new Map<string, Value>();

  constructor() {}

  define(name: Token, value: Value): void {
    this.values.set(name.lexeme!, value);
  }

  getToken(name: Token): Value {
    if (name.lexeme && this.values.has(name.lexeme)) {
      return this.values.get(name.lexeme) ?? null;
    }
    throw new RuntimeError(name, `Undefined variable ${name.lexeme}.`);
  }

  assign(name: Token, value: Value): void {
    if (name.lexeme && this.values.has(name.lexeme)) {
      this.values.set(name.lexeme, value);
      return;
    }
    throw new RuntimeError(name, `Undefined variable '${name.lexeme}'.`);
  }
}
