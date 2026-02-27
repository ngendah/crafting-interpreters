import { CallableFn } from "./common";
import { Interpreter } from "./interpreter";

export abstract class Callable<T> implements CallableFn {
  constructor(public readonly arity: number) {}

  abstract call(interpreter: Interpreter<T>, args: T[]): T;
}
