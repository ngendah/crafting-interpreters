import { Expr, Stmt, Token, Visitor } from '../common'

export class Call implements Stmt {
  constructor(public readonly callee:Expr, public readonly paren:Token, public readonly args:Expr[]) { }

  accept<T>(visitor: Visitor<T>): T {
    return visitor.visit(this);
  }

}