import { Expr, Stmt, Visitor } from "./base"
import { Token } from "../common"

export default class Var implements Stmt {
  constructor(public readonly name:Token,public readonly initializer:Expr) { }

  accept<T>(visitor: Visitor<T>): T {
    return visitor.visit(this);
  }

}