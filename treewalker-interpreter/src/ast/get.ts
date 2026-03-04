import { Expr, Stmt, Token, Visitor } from '../common'

export class Get implements Stmt {
  constructor(public readonly object:Expr, public readonly name:Token) { }

  accept<T>(visitor: Visitor<T>): T {
    return visitor.visit(this);
  }

}