import { Expr, Stmt, Token, Visitor } from '../common'

export class Set implements Stmt {
  constructor(public readonly object:Expr, public readonly name:Token, public readonly value:Expr) { }

  accept<T>(visitor: Visitor<T>): T {
    return visitor.visit(this);
  }

}