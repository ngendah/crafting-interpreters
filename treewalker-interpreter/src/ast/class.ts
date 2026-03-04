import { Stmt, Token, Visitor } from '../common'
import { Function } from './function'
import { Variable } from './variable'

export class Class implements Stmt {
  constructor(public readonly name:Token, public readonly methods:Function[], public readonly superclass?:Variable) { }

  accept<T>(visitor: Visitor<T>): T {
    return visitor.visit(this);
  }

}