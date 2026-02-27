import { Expr, Token, Visitor } from '../common'

export class This implements Expr {
  constructor(public readonly keyword:Token) { }

  accept<T>(visitor: Visitor<T>): T {
    return visitor.visit(this);
  }

}