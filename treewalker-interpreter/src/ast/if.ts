import { Expr, Stmt, Visitor } from '../common'

export class If implements Stmt {
  constructor(public readonly condition:Expr, public readonly thenBranch:Stmt, public readonly elseBranch?:Stmt) { }

  accept<T>(visitor: Visitor<T>): T {
    return visitor.visit(this);
  }

}