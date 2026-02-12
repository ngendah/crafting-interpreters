import { Expr, Stmt, Token, Visitor } from '../common'

export class Return implements Stmt {
  constructor(public readonly keyword:Token, public readonly value:Expr) { }

  accept<T>(visitor: Visitor<T>): T {
    return visitor.visit(this);
  }

}