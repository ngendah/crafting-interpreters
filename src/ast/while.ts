import { Expr, Stmt, Visitor } from '../common'

export class While implements Stmt {
  constructor(public readonly condition:Expr, public readonly body:Stmt) { }

  accept<T>(visitor: Visitor<T>): T {
    return visitor.visit(this);
  }

}