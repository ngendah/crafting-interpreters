import { Expr, Stmt, Visitor } from "./base"

export default class Expression implements Stmt {
  constructor(public readonly expression:Expr) { }

  accept<T>(visitor: Visitor<T>): T {
    return visitor.visit(this);
  }

}