import { Stmt, Visitor } from '../common'

export class Continue implements Stmt {
  constructor() { }

  accept<T>(visitor: Visitor<T>): T {
    return visitor.visit(this);
  }

}