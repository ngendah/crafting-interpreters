import { Expr, Visitor } from '../common'

export class Grouping implements Expr {
  constructor(public readonly expression:Expr) { }

  accept<T>(visitor: Visitor<T>): T {
    return visitor.visit(this);
  }

}