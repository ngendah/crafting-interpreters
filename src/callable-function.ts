import { Block, Function } from "./ast";
import { Callable } from "./callable";
import { Interpreter } from "./interpreter";
import Environment from "./environment";
import {
  FunctionKind,
  Instance,
  ThrowableReturn,
  Token,
  TokenType,
} from "./common";

export default class CallableFunction<T> extends Callable<T> {
  constructor(
    public readonly declaration: Function,
    public readonly closure: Environment<T>,
  ) {
    super(declaration.params.length);
  }

  get isInitializer(): boolean {
    return (
      this.declaration.kind == FunctionKind.METHOD &&
      this.declaration.name.toString() == "init"
    );
  }

  bind(instance: Instance): CallableFunction<T> {
    const environment = new Environment<T>(this.closure);
    // FIXME: rm 'as T'
    environment.define(
      new Token(TokenType.THIS, 0, TokenType.THIS),
      instance as T,
    );
    return new CallableFunction<T>(this.declaration, environment);
  }

  call(interpreter: Interpreter<T>, args: T[]): T {
    const environment = new Environment<T>(this.closure);
    this.declaration.params.forEach((param, index) => {
      const arg = args[index];
      if (arg instanceof Function && arg.kind == FunctionKind.LAMBDA) {
        interpreter.evaluate(
          new Function(param, arg.params, arg.body, arg.kind),
        );
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
        if (this.isInitializer) {
          return this.closure.at(
            0,
            new Token(TokenType.THIS, 0, TokenType.THIS),
          ) as T;
        }
        return value.get<T>();
      }
      throw value;
    }
  }
}
