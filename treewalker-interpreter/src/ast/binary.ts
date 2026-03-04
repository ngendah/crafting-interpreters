import { Expr, Token, Visitor } from '../common'

export class Binary implements Expr {
  constructor(public readonly left:Expr, public readonly operator:Token, public readonly right:Expr) { }

  accept<T>(visitor: Visitor<T>): T {
    return visitor.visit(this);
  }

}