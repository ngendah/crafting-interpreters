import { Expr, Token, Visitor } from '../common'

export class Variable implements Expr {
  constructor(public readonly name:Token) { }

  accept<T>(visitor: Visitor<T>): T {
    return visitor.visit(this);
  }

}