import { CallableFn } from "./common";
import { Interpreter } from "./interpreter";

export abstract class Callable<T> implements CallableFn {
  constructor(protected readonly arity: number) {}

  abstract call(interpreter: Interpreter<T>, args: T[]): void;
}
