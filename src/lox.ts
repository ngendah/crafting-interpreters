import {
  Assign,
  Binary,
  Block,
  Call,
  Class,
  Expression,
  Function,
  Get,
  Grouping,
  If,
  Literal,
  Logical,
  Print,
  Return,
  Set,
  This,
  Unary,
  Var,
  Variable,
  While,
} from "./ast";
import {
  ClassKind,
  Expr,
  FunctionKind,
  isTruthy,
  RuntimeError,
  Stack,
  Stmt,
  ThrowableReturn,
  Token,
  TokenType,
  Value,
} from "./common";
import Environment from "./environment";
import { Interpreter } from "./interpreter";
import { Callable } from "./callable";
import CallableFunction from "./callable-function";
import { Resolver } from "./resolver";
import CallableClass from "./callable-class";
import { ClassInstance } from "./class-instance";

class Clock extends Callable<Value> {
  constructor() {
    super(0);
  }

  call(): Value {
    return Date.now().toString();
  }
}

export class Lox extends Interpreter<Value> {
  globals = new Environment<Value>();
  protected environment = this.globals;

  private defineGlobal(name: string, value: Value) {
    this.globals.values.set(name, value);
  }

  constructor(protected readonly printer: (...args: unknown[]) => void) {
    super();
    this.defineGlobal("clock", new Clock());
  }

  getEnvironment(): Environment<Value> {
    return this.environment;
  }

  nil(): Value {
    return null;
  }

  protected stringfy(value: Value): string {
    if (value == null) return "nil";
    if (typeof value == "number") {
      // FIXME:
      let text = value.toString();
      if (text.endsWith(".0")) {
        text = text.substring(0, text.length - 2);
      }
      return text;
    }
    return value.toString();
  }

  protected operator(lhs: Value, rhs: Value, operator: Token): Value {
    switch (operator.type) {
      case TokenType.MINUS:
        if (typeof lhs == "number" && typeof rhs == "number") {
          return lhs - rhs;
        }
        break;
      case TokenType.SLASH:
        if (typeof lhs == "number" && typeof rhs == "number") {
          return lhs / rhs;
        }
        break;
      case TokenType.STAR:
        if (typeof lhs == "number" && typeof rhs == "number") {
          return lhs * rhs;
        }
        break;
      case TokenType.PLUS:
        if (typeof lhs == "number" && typeof rhs == "number") {
          return lhs + rhs;
        } else if (typeof lhs == "string" && typeof rhs == "string") {
          return lhs + rhs;
        }
        break;
      case TokenType.GREATER:
        if (typeof lhs == "number" && typeof rhs == "number") {
          return lhs > rhs;
        }
        break;
      case TokenType.GREATER_EQUAL:
        if (typeof lhs == "number" && typeof rhs == "number") {
          return lhs >= rhs;
        }
        break;
      case TokenType.LESS:
        if (typeof lhs == "number" && typeof rhs == "number") {
          return lhs < rhs;
        }
        break;
      case TokenType.LESS_EQUAL:
        if (typeof lhs == "number" && typeof rhs == "number") {
          return lhs <= rhs;
        }
        break;
      case TokenType.BANG_EQUAL:
        if (typeof lhs == "number" && typeof rhs == "number") {
          return !(lhs == rhs);
        }
        break;
      case TokenType.EQUAL_EQUAL:
        if (typeof lhs == "number" && typeof rhs == "number") {
          return lhs == rhs;
        }
        break;
    }
    throw new RuntimeError(
      operator,
      `Invalid values  lhs=${lhs}, rhs=${rhs} for operator ${operator.lexeme}`,
    );
  }

  evaluateLiteral(expr: Literal): Value {
    return expr.value;
  }

  evaluateGrouping(expr: Grouping): Value {
    return this.evaluate(expr);
  }

  evaluateUnary(expr: Unary): Value {
    const right = this.evaluate(expr.right);
    if (right && expr.operator.type == TokenType.MINUS) {
      return -right;
    } else if (right && expr.operator.type == TokenType.BANG) {
      return !isTruthy(right);
    }
    throw new RuntimeError(expr.operator, `Unhandled unary operator..`);
  }

  evaluateBinary(expr: Binary): Value {
    const left = this.evaluate(expr.left);
    const right = this.evaluate(expr.right);
    return this.operator(left, right, expr.operator);
  }

  evaluateExpression(expr: Expression): Value {
    return this.evaluate(expr.expression);
  }

  evaluateVar(expr: Var): Value {
    let val: Value = this.nil();
    if (expr.initializer) {
      val = this.evaluate(expr.initializer);
    }
    if (!expr.name.lexeme) {
      // FIXME: interpreter error?
      throw new RuntimeError(expr.name, "var without a lexeme");
    }
    this.environment.define(expr.name, val);
    return val;
  }

  evaluatePrint(expr: Print): Value {
    const val = this.evaluate(expr.expression);
    const str = this.stringfy(val);
    this.printer(str);
    return str;
  }

  evaluateVariable(expr: Variable): Value {
    return this.environment.get(expr.name);
  }

  evaluateAssign(expr: Assign): Value {
    const value = this.evaluate(expr.value);
    this.environment.assign(expr.name, value);
    return value;
  }

  evaluateBlock(block: Block, environment: Environment<Value>): Value {
    const previousEnvironment = this.environment;
    const returnValue = this.nil();
    try {
      this.environment = environment;
      for (const stmt of block.statements) {
        this.evaluate(stmt);
      }
    } finally {
      this.environment = previousEnvironment;
    }
    return returnValue;
  }

  evaluateIf(stmt: If): Value {
    const condition = this.evaluate(stmt.condition);
    if (isTruthy(condition)) {
      return this.evaluate(stmt.thenBranch);
    } else if (stmt.elseBranch) {
      return this.evaluate(stmt.elseBranch);
    }
    return this.nil();
  }

  evaluateLogical(stmt: Logical): Value {
    const left = this.evaluate(stmt.left);
    if (stmt.operator.type == TokenType.OR) {
      if (isTruthy(left)) return left;
    } else {
      if (!isTruthy(left)) return left;
    }
    return this.evaluate(stmt.right);
  }

  evaluateWhile(stmt: While): Value {
    while (isTruthy(this.evaluate(stmt.condition))) {
      this.evaluate(stmt.body);
    }
    return this.nil();
  }

  evaluateFunction(stmt: Function): Value {
    this.globals.define(
      stmt.name,
      new CallableFunction(stmt, this.environment),
    );
    return this.nil();
  }

  evaluateCall(stmt: Call): Value {
    const callee = this.evaluate(stmt.callee);
    const args: (Value | Expr)[] = stmt.args.map((arg) =>
      arg instanceof Function ? arg : this.evaluate(arg),
    );
    if (callee instanceof Callable) {
      return callee.call(this, args);
    }
    throw new RuntimeError(stmt.paren, "Can only call functions and classes.");
  }

  evaluateReturn(stmt: Return): Value {
    let value: Value = this.nil();
    if (stmt.value) value = this.evaluate(stmt.value);
    throw new ThrowableReturn(value);
  }

  evaluateClass(stmt: Class): Value {
    const methods = new Map<string, CallableFunction<Value>>();
    for (const method of stmt.methods) {
      const func = new CallableFunction(method, this.environment);
      methods.set(method.name.toString(), func);
    }
    this.globals.define(stmt.name, new CallableClass(stmt.name, methods));
    return this.nil();
  }

  evaluateGet(expr: Get): Value {
    const object = this.evaluate(expr.object);
    if (object instanceof ClassInstance) {
      return object.get(expr.name);
    }
    throw new RuntimeError(expr.name, "Only class instance have properties.");
  }

  evaluateSet(expr: Set): Value {
    const object = this.evaluate(expr.object);
    if (object instanceof ClassInstance) {
      const value = this.evaluate(expr.value);
      object.set(expr.name, value);
      return value;
    }
    throw new RuntimeError(expr.name, "Only class instance have fields.");
  }

  evaluateThis(expr: This): Value {
    throw new RuntimeError(
      "",
      `Lox.evaluateThis is not implemented, use instead 'LoxWithResolver'.`,
    );
  }

  // eslint-disable-next-line @typescript-eslint/no-unused-vars
  resolve(expr: Expr, depth: number): void {
    throw new RuntimeError(
      "",
      `Lox.resolve not implemented, use instead 'LoxWithResolver'.`,
    );
  }
}

export class LoxResolver extends Resolver<Value> {
  protected scopes = new Stack<Map<string, boolean>>();
  protected currentFunction: FunctionKind | undefined;
  protected currentFunctionIsClsInitializer = false;
  protected currentCls: ClassKind | undefined;
  constructor(protected readonly interpreter: Interpreter<Value>) {
    super(interpreter);
  }

  beginScope(): Map<string, boolean> {
    const scope = new Map<string, boolean>();
    this.scopes.push(scope);
    return scope;
  }

  endScope() {
    this.scopes.pop();
  }

  declare(name: Token) {
    if (this.scopes.isEmpty) return;
    const scope = this.scopes.peek!;
    if (scope.has(name.toString())) {
      throw new RuntimeError(name, "Variable already exists.");
    }
    scope.set(name.toString(), false);
  }

  define(name: Token) {
    if (this.scopes.isEmpty) return;
    this.scopes.peek?.set(name.toString(), true);
  }

  resolveLocal(expr: Expr, name: Token) {
    // TODO: iterator pattern?
    for (let index = this.scopes.length - 1; index >= 0; index = index - 1) {
      if (this.scopes.at(index)?.has(name.toString())) {
        this.interpreter.resolve(expr, this.scopes.length - 1 - index);
      }
    }
  }

  resolveBlock(block: Block): void {
    this.beginScope();
    this.resolve(block.statements);
    this.endScope();
  }

  resolveVar(expr: Var): void {
    if (this.scopes.peek?.get(expr.name.toString())) {
      throw new RuntimeError(
        expr.name,
        `Variable already exists on the scope.`,
      );
    }
    this.declare(expr.name);
    if (expr.initializer) {
      this.accept(expr.initializer);
    }
    this.define(expr.name);
  }

  resolveAssign(expr: Assign): void {
    this.accept(expr.value);
    this.resolveLocal(expr, expr.name);
  }

  resolveFunction(stmt: Function): void {
    this.declare(stmt.name);
    this.define(stmt.name);
    const enclosingFunction = this.currentFunction;
    this.currentFunction = stmt.kind;
    this.beginScope();
    for (const param of stmt.params) {
      this.declare(param);
      this.define(param);
    }
    this.resolve(stmt.body);
    this.endScope();
    this.currentFunction = enclosingFunction;
  }

  resolveExpression(expr: Expression): void {
    this.accept(expr.expression);
  }

  resolveIf(stmt: If): void {
    this.accept(stmt.condition);
    this.accept(stmt.thenBranch);
    if (stmt.elseBranch) this.accept(stmt.elseBranch);
  }

  resolvePrint(expr: Print): void {
    this.accept(expr.expression);
  }

  resolveVariable(expr: Variable): void {
    this.resolveLocal(expr, expr.name);
  }

  resolveReturn(stmt: Return): void {
    if (
      !this.currentFunction ||
      ![
        FunctionKind.FUNCTION,
        FunctionKind.LAMBDA,
        FunctionKind.METHOD,
      ].includes(this.currentFunction)
    )
      throw new RuntimeError(
        stmt.keyword,
        `Cannot return from top-level code.`,
      );
    if (stmt.value) {
      if (this.currentFunctionIsClsInitializer) {
        throw new RuntimeError(
          stmt.keyword,
          `Can't return a value from a class initializer.`,
        );
      }
      this.accept(stmt.value);
    }
  }

  resolveWhile(stmt: While): void {
    this.accept(stmt.condition);
    this.accept(stmt.body);
  }

  resolveBinary(expr: Binary): void {
    this.accept(expr.left);
    this.accept(expr.right);
  }

  resolveCall(stmt: Call): void {
    this.accept(stmt.callee);
    for (const arg of stmt.args) {
      this.accept(arg);
    }
  }

  resolveLogical(stmt: Logical): void {
    this.accept(stmt.left);
    this.accept(stmt.right);
  }

  resolveUnary(expr: Unary): void {
    this.accept(expr.right);
  }

  // eslint-disable-next-line @typescript-eslint/no-unused-vars
  resolveLiteral(expr: Literal): void {}

  // eslint-disable-next-line @typescript-eslint/no-unused-vars
  resolveGrouping(expr: Grouping): void {}

  resolveClass(stmt: Class): void {
    const previousCls = this.currentCls;
    this.currentCls = "class";
    this.declare(stmt.name);
    this.define(stmt.name);
    const scope = this.beginScope();
    scope.set(TokenType.THIS, true);
    for (const method of stmt.methods) {
      const currentFunctionIsClsInitializer =
        this.currentFunctionIsClsInitializer;
      this.currentFunctionIsClsInitializer = method.name.toString() == "init";
      this.resolveFunction(method);
      this.currentFunctionIsClsInitializer = currentFunctionIsClsInitializer;
    }
    this.endScope();
    this.currentCls = previousCls;
  }

  resolveGet(expr: Get): void {
    this.accept(expr.object);
  }

  resolveSet(expr: Set): void {
    this.accept(expr.value);
    this.accept(expr.object);
  }

  resolveThis(expr: This): void {
    if (this.currentCls != "class") {
      throw new RuntimeError(
        expr.keyword,
        `Can't use 'this' keyword outside of a class.`,
      );
    }
    this.resolveLocal(expr, expr.keyword);
  }
}

export class LoxWithResolver extends Lox {
  private locals = new Map<Expr, number>();
  constructor(protected readonly printer: (...args: unknown[]) => void) {
    super(printer);
  }

  lookupVariable(name: Token, expr: Expr): Value {
    const distance = this.locals.get(expr);
    if (isTruthy<number>(distance)) {
      const val = this.environment.at(distance, name);
      if (!val) {
        throw new RuntimeError(name, `Variable not found in the current scope`);
      }
      return val;
    } else {
      return this.globals.get(name);
    }
  }

  resolver(): Resolver<Value> {
    return new LoxResolver(this);
  }

  override evaluateVariable(expr: Variable): Value {
    return this.lookupVariable(expr.name, expr);
  }

  override evaluateAssign(expr: Assign): Value {
    const value = this.evaluate(expr.value);
    const distance = this.locals.get(expr);
    if (isTruthy<number>(distance)) {
      this.environment.assignAt(distance, expr.name, value);
    } else {
      this.globals.assign(expr.name, value);
    }
    return value;
  }

  override evaluateThis(expr: This): Value {
    return this.environment.get(expr.keyword);
  }

  override resolve(expr: Expr, depth: number): void {
    this.locals.set(expr, depth);
  }

  override interpret(statements: Stmt[]): void {
    this.resolver().resolve(statements);
    super.interpret(statements);
  }
}
