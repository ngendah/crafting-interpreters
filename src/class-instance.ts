import CallableClass from "./callable-class";
import CallableFunction from "./callable-function";
import { Instance, isTruthy, RuntimeError, Token } from "./common";

export class ClassInstance<T> extends Instance {
  private fields = new Map<string, T>();
  constructor(protected readonly cls: CallableClass<T>) {
    super();
  }

  get(name: Token): T {
    const value = this.fields.get(name.toString());
    if (isTruthy<T>(value)) {
      return value;
    }
    const method = this.cls.methods.get(name.toString());
    // FIXME: rm 'as T'
    if (method instanceof CallableFunction) return method.bind(this) as T;
    throw new RuntimeError(name, `Undefined property ${name.toString()}.`);
  }

  set(name: Token, value: T): void {
    this.fields.set(name.toString(), value);
  }

  toString(): string {
    return this.cls.name.toString();
  }
}
