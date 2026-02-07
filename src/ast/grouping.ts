import { Expr, Visitor } from "./base"

export default class Grouping implements Expr {
  constructor(public readonly expression:Expr) { }

  accept<T>(visitor: Visitor<T>): T {
    return visitor.visit(this);
  }

}