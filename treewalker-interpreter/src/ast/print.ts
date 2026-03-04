import { Expr, Stmt, Visitor } from '../common'

export class Print implements Stmt {
  constructor(public readonly expression:Expr) { }

  accept<T>(visitor: Visitor<T>): T {
    return visitor.visit(this);
  }

}