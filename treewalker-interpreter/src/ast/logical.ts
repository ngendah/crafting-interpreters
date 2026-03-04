import { Expr, Stmt, Token, Visitor } from '../common'

export class Logical implements Stmt {
  constructor(public readonly left:Expr, public readonly operator:Token, public readonly right:Expr) { }

  accept<T>(visitor: Visitor<T>): T {
    return visitor.visit(this);
  }

}