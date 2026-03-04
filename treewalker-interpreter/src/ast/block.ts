import { Stmt, Visitor } from '../common'

export class Block implements Stmt {
  constructor(public readonly statements:Stmt[]) { }

  accept<T>(visitor: Visitor<T>): T {
    return visitor.visit(this);
  }

}