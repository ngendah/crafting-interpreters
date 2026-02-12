import { RuntimeError, Token } from "./common";

export default class Environment<T> {
  values = new Map<string, T>();

  constructor(public readonly enclosing?: Environment<T>) {}

  define(name: Token, value: T): void {
    this.values.set(name.lexeme!, value);
  }

  get(name: Token | string): T {
    return name instanceof Token ? this.getT(name) : this.getS(name);
  }

  private getS(name: string): T {
    if (name && this.values.has(name)) {
      return this.values.get(name) as T;
    }
    if (this.enclosing) {
      return this.enclosing.get(name);
    }
    throw new RuntimeError(name, `Undefined variable ${name}`);
  }

  private getT(name: Token) {
    if (name.lexeme && this.values.has(name.lexeme)) {
      return this.values.get(name.lexeme) as T;
    }
    if (this.enclosing) {
      return this.enclosing.get(name);
    }
    throw new RuntimeError(name, `Undefined variable ${name.lexeme}.`);
  }

  assign(name: Token, value: T): void {
    if (name.lexeme && this.values.has(name.lexeme)) {
      this.values.set(name.lexeme, value);
      return;
    }
    if (this.enclosing) {
      this.enclosing.assign(name, value);
      return;
    }
    throw new RuntimeError(name, `Undefined variable '${name.lexeme}'.`);
  }
}
