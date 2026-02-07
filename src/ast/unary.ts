import { Expr, Visitor } from "./base"
import { Token } from "../common"

export default class Unary implements Expr {
  constructor(public readonly operator:Token,public readonly right:Expr) { }

  accept<T>(visitor: Visitor<T>): T {
    return visitor.visit(this);
  }

}