import { Stmt, Token, Visitor } from '../common'
import { Function } from './function'

export class Class implements Stmt {
  constructor(public readonly name:Token, public readonly methods:Function[]) { }

  accept<T>(visitor: Visitor<T>): T {
    return visitor.visit(this);
  }

}