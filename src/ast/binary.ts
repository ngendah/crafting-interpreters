import { Expr, Visitor } from "./base"
import { Token } from "../common"

export default class Binary implements Expr {
  constructor(public readonly left:Expr,public readonly operator:Token,public readonly right:Expr) { }

  accept<T>(visitor: Visitor<T>): T {
    return visitor.visit(this);
  }

}