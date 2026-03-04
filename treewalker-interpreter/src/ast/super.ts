import { Expr, Token, Visitor } from '../common'

export class Super implements Expr {
  constructor(public readonly keyword:Token, public readonly method:Token) { }

  accept<T>(visitor: Visitor<T>): T {
    return visitor.visit(this);
  }

}