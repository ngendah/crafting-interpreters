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
import { Interpreter } from "./interpreter";

export abstract class Resolver<T> implements Visitor<void> {
  constructor(protected readonly interpreter: Interpreter<T>) {}

  visit(value: Expr | Stmt) {
    if (value instanceof Literal) return this.resolveLiteral(value);
    if (value instanceof Grouping) return this.resolveGrouping(value);
    if (value instanceof Unary) return this.resolveUnary(value);
    if (value instanceof Binary) return this.resolveBinary(value);
    if (value instanceof Expression) return this.resolveExpression(value);
    if (value instanceof Var) return this.resolveVar(value);
    if (value instanceof Print) return this.resolvePrint(value);
    if (value instanceof Variable) return this.resolveVariable(value);
    if (value instanceof Assign) return this.resolveAssign(value);
    if (value instanceof Block) return this.resolveBlock(value);
    if (value instanceof If) return this.resolveIf(value);
    if (value instanceof Logical) return this.resolveLogical(value);
    if (value instanceof While) return this.resolveWhile(value);
    if (value instanceof Function) return this.resolveFunction(value);
    if (value instanceof Call) return this.resolveCall(value);
    if (value instanceof Return) return this.resolveReturn(value);
  }

  abstract resolveLiteral(expr: Literal): void;
  abstract resolveGrouping(expr: Grouping): void;
  abstract resolveUnary(expr: Unary): void;
  abstract resolveBinary(expr: Binary): void;
  abstract resolveExpression(expr: Expression): void;
  abstract resolveVar(expr: Var): void;
  abstract resolvePrint(expr: Print): void;
  abstract resolveVariable(expr: Variable): void;
  abstract resolveAssign(expr: Assign): void;
  abstract resolveBlock(block: Block): void;
  abstract resolveIf(stmt: If): void;
  abstract resolveLogical(stmt: Logical): void;
  abstract resolveWhile(stmt: While): void;
  abstract resolveFunction(stmt: Function): void;
  // FIXME: call is an expr not a stmt
  abstract resolveCall(stmt: Call): void;
  abstract resolveReturn(stmt: Return): void;

  protected accept(eOs: Expr | Stmt): void {
    return eOs.accept(this);
  }

  resolve(statements: Stmt[]): void {
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
