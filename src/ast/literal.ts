import { Expr, Value, Visitor } from '../common'

export class Literal implements Expr {
  constructor(public readonly value:Value) { }

  accept<T>(visitor: Visitor<T>): T {
    return visitor.visit(this);
  }

}