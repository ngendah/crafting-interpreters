import { Expr, Token, Visitor } from '../common'

export class Assign implements Expr {
  constructor(public readonly name:Token, public readonly value:Expr) { }

  accept<T>(visitor: Visitor<T>): T {
    return visitor.visit(this);
  }

}