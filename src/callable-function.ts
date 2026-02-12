import { Block, Function, Lambda } from "./ast";
import { Callable } from "./callable";
import { Interpreter } from "./interpreter";
import Environment from "./environment";
import { ThrowableReturn, Token } from "./common";

export default class CallableFunction<T> extends Callable<T> {
  constructor(
    public readonly declaration: Function,
    public readonly closure: Environment<T>,
  ) {
    super(declaration.params.length);
  }

  call(interpreter: Interpreter<T>, args: T[]): T {
    const environment = new Environment<T>(this.closure);
    this.declaration.params.forEach((param, index) => {
      const arg = args[index];
      if (arg instanceof Lambda) {
        interpreter.evaluate(new Lambda(param, arg.params, arg.body));
      } else if (param instanceof Token) {
        environment.define(param, args[index]);
      } // FIXME: handle error
    });
    try {
      return interpreter.evaluateBlock(
        new Block(this.declaration.body),
        environment,
      );
    } catch (value) {
      if (value instanceof ThrowableReturn) {
        return value.get<T>();
      }
      throw value;
    }
  }
}
