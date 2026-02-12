import {
  Assign,
  Binary,
  Block,
  Call,
  Expression,
  Function,
  Grouping,
  If,
  Literal,
  Logical,
  Print,
  Return,
  Unary,
  Var,
  Variable,
  While,
} from "./ast";
import {
  Expr,
  RuntimeError,
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

class Clock extends Callable<Value> {
  constructor() {
    super(0);
  }

  call(): Value {
    return Date.now().toString();
  }
}

export default class Lox extends Interpreter<Value> {
  constructor(protected readonly printer: (...args: unknown[]) => void) {
    super();
    this.defineGlobal("clock", new Clock());
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

  protected isTruthy(value: Value): boolean {
    if (value == null) return false;
    if (typeof value == "boolean") return value;
    return true;
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
      return !this.isTruthy(right);
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
    let val: Value = null;
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
    const returnValue = null;
    try {
      this.environment = environment;
      for (const stmt of block.statements) {
        this.execute(stmt);
      }
    } finally {
      this.environment = previousEnvironment;
    }
    return returnValue;
  }

  evaluateIf(stmt: If): Value {
    if (this.isTruthy(this.evaluate(stmt.condition))) {
      return this.execute(stmt.thenBranch);
    } else if (stmt.elseBranch) {
      return this.execute(stmt.elseBranch);
    }
    return null;
  }

  evaluateLogical(stmt: Logical): Value {
    const left = this.evaluate(stmt.left);
    if (stmt.operator.type == TokenType.OR) {
      if (this.isTruthy(left)) return left;
    } else {
      if (!this.isTruthy(left)) return left;
    }
    return this.evaluate(stmt.right);
  }

  evaluateWhile(stmt: While): Value {
    while (this.isTruthy(this.evaluate(stmt.condition))) {
      this.execute(stmt.body);
    }
    return null;
  }

  evaluateFunction(stmt: Function): Value {
    this.globals.define(
      stmt.name,
      new CallableFunction(stmt, this.environment),
    );
    return null;
  }

  evaluateCall(stmt: Call): Value {
    const callee = this.evaluate(stmt.callee);
    const args: (Value | Expr)[] = stmt.args.map((arg) =>
      arg instanceof Function ? arg : this.evaluate(arg),
    );
    if (callee instanceof CallableFunction) {
      return callee.call(this, args);
    }
    throw new RuntimeError(stmt.paren, "Can only call functions and classes.");
  }

  evaluateReturn(stmt: Return): Value {
    let value: Value = null;
    if (stmt.value) value = this.evaluate(stmt.value);
    throw new ThrowableReturn(value);
  }

  visit(eOs: Expr | Stmt): Value {
    if (eOs instanceof Literal) return this.evaluateLiteral(eOs);
    if (eOs instanceof Grouping) return this.evaluateGrouping(eOs);
    if (eOs instanceof Unary) return this.evaluateUnary(eOs);
    if (eOs instanceof Binary) return this.evaluateBinary(eOs);
    if (eOs instanceof Expression) return this.evaluateExpression(eOs);
    if (eOs instanceof Var) return this.evaluateVar(eOs);
    if (eOs instanceof Print) return this.evaluatePrint(eOs);
    if (eOs instanceof Variable) return this.evaluateVariable(eOs);
    if (eOs instanceof Assign) return this.evaluateAssign(eOs);
    if (eOs instanceof Block)
      return this.evaluateBlock(eOs, new Environment(this.environment));
    if (eOs instanceof If) return this.evaluateIf(eOs);
    if (eOs instanceof Logical) return this.evaluateLogical(eOs);
    if (eOs instanceof While) return this.evaluateWhile(eOs);
    if (eOs instanceof Function) return this.evaluateFunction(eOs);
    if (eOs instanceof Call) return this.evaluateCall(eOs);
    if (eOs instanceof Return) return this.evaluateReturn(eOs);
    return null;
  }

  interpret(statements: Stmt[]): void {
    try {
      for (const stmt of statements) {
        this.execute(stmt);
      }
    } catch (error) {
      // FIXME:
      console.error(error);
    }
  }
}
