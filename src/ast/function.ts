import { Stmt, Token, Visitor } from '../common'

export class Function implements Stmt {
  constructor(public readonly name:Token, public readonly params:Token[], public readonly body:Stmt[]) { }

  accept<T>(visitor: Visitor<T>): T {
    return visitor.visit(this);
  }

}