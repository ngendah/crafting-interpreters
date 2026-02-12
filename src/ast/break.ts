import { Stmt, Visitor } from '../common'

export class Break implements Stmt {
  constructor() { }

  accept<T>(visitor: Visitor<T>): T {
    return visitor.visit(this);
  }

}