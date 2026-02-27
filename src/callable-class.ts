import { Callable } from "./callable";
import { Interpreter } from "./interpreter";
import { Token } from "./common";
import { ClassInstance } from "./class-instance";
import CallableFunction from "./callable-function";

export default class CallableClass<T> extends Callable<T> {
  constructor(
    public readonly name: Token,
    public readonly methods: Map<string, Callable<T>>,
  ) {
    const initializer = methods.get("init");
    super(initializer instanceof CallableFunction ? initializer.arity : 0);
  }

  call(interpreter: Interpreter<T>, args: T[]): T {
    const instance = new ClassInstance<T>(this);
    const initializer = this.methods.get("init");
    if (initializer instanceof CallableFunction) {
      initializer.bind(instance).call(interpreter, args);
    }
    return instance as T;
  }
}
