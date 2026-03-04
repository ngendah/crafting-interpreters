import { Expr, Stmt, Token, Visitor } from '../common'

export class Var implements Stmt {
  constructor(public readonly name:Token, public readonly initializer:Expr) { }

  accept<T>(visitor: Visitor<T>): T {
    return visitor.visit(this);
  }

}