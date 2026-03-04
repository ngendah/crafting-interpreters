import { RuntimeError, Token } from "./common";

export default class Environment<T> {
  values = new Map<string, T>();

  constructor(public readonly enclosing?: Environment<T>) {}

  define(name: Token, value: T): void {
    this.values.set(name.toString(), value);
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
    if (this.values.has(name.toString())) {
      return this.values.get(name.toString()) as T;
    }
    if (this.enclosing) {
      return this.enclosing.get(name);
    }
    throw new RuntimeError(name, `Undefined variable ${name.lexeme}.`);
  }

  assign(name: Token, value: T): void {
    // TODO: with scopes in-place; should assign only set to the current environment?
    if (name.lexeme && this.values.has(name.lexeme)) {
      this.values.set(name.toString(), value);
      return;
    }
    if (this.enclosing) {
      this.enclosing.assign(name, value);
      return;
    }
    throw new RuntimeError(name, `Undefined variable '${name.lexeme}'.`);
  }

  ancestor(distance: number): Environment<T> | undefined {
    let env: Environment<T> | undefined = this as Environment<T>;
    for (let index = 0; index < distance; index = index + 1) {
      env = env?.enclosing;
    }
    return env;
  }

  at(distance: number, name: Token): T | undefined {
    return this.ancestor(distance)?.values.get(name.toString());
  }

  assignAt(distance: number, name: Token, value: T): void {
    this.ancestor(distance)?.assign(name, value);
  }
}
