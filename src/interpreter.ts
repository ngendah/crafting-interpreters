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
  abstract getEnvironment(): Environment<T>;
  abstract nil(): T;

  abstract resolve(expr: Expr, depth: number): void;

  visit(value: Expr | Stmt): T {
    if (value instanceof Literal) return this.evaluateLiteral(value);
    if (value instanceof Grouping) return this.evaluateGrouping(value);
    if (value instanceof Unary) return this.evaluateUnary(value);
    if (value instanceof Binary) return this.evaluateBinary(value);
    if (value instanceof Expression) return this.evaluateExpression(value);
    if (value instanceof Var) return this.evaluateVar(value);
    if (value instanceof Print) return this.evaluatePrint(value);
    if (value instanceof Variable) return this.evaluateVariable(value);
    if (value instanceof Assign) return this.evaluateAssign(value);
    if (value instanceof Block)
      return this.evaluateBlock(value, new Environment(this.getEnvironment()));
    if (value instanceof If) return this.evaluateIf(value);
    if (value instanceof Logical) return this.evaluateLogical(value);
    if (value instanceof While) return this.evaluateWhile(value);
    if (value instanceof Function) return this.evaluateFunction(value);
    if (value instanceof Call) return this.evaluateCall(value);
    if (value instanceof Return) return this.evaluateReturn(value);
    return this.nil();
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
  // FIXME: call is an expr not stmt
  abstract evaluateCall(stmt: Call): T;
  abstract evaluateReturn(stmt: Return): T;

  protected evaluate(eOs: Expr | Stmt): T {
    return eOs.accept(this);
  }

  interpret(statements: Stmt[]): void {
    try {
      for (const stmt of statements) {
        stmt.accept(this);
      }
    } catch (error) {
      // FIXME:
      console.error(error);
    }
  }
}
