import { Stmt, Token, Visitor } from '../common'
import { Function } from './function'

export class Lambda extends Function {
  constructor(public readonly name:Token, public readonly params:Token[], public readonly body:Stmt[]) { super(name,params,body) }

  accept<T>(visitor: Visitor<T>): T {
    return visitor.visit(this);
  }

}