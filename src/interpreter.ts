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
import { Expr, Stmt, Visitor } from "./common";
import Environment from "./environment";

export abstract class Interpreter<T> implements Visitor<T> {
  globals = new Environment<T>();
  environment = this.globals;

  abstract visit(value: Expr | Stmt): T;

  defineGlobal(name: string, value: T) {
    this.globals.values.set(name, value);
  }

  protected execute(stmt: Stmt): T {
    return stmt.accept(this);
  }

  evaluate(eOs: Expr | Stmt): T {
    return eOs.accept(this);
  }

  abstract evaluateLiteral(expr: Literal): T;
  abstract evaluateGrouping(expr: Grouping): T;
  abstract evaluateUnary(expr: Unary): T;
  abstract evaluateBinary(expr: Binary): T;
  abstract evaluateExpression(expr: Expression): T;
  abstract evaluateVar(expr: Var): T;
  abstract evaluatePrint(expr: Print): T;
  abstract evaluateVariable(expr: Variable): T;
  abstract evaluateAssign(expr: Assign): T;
  abstract evaluateBlock(block: Block, environment: Environment<T>): T;
  abstract evaluateIf(stmt: If): T;
  abstract evaluateLogical(stmt: Logical): T;
  abstract evaluateWhile(stmt: While): T;
  abstract evaluateFunction(stmt: Function): T;
  abstract evaluateCall(stmt: Call): T;
  abstract evaluateReturn(stmt: Return): T;
}
