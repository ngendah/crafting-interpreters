import { Expr, Visitor } from "./base"
import { Token } from "../common"

export default class Variable implements Expr {
  constructor(public readonly name:Token) { }

  accept<T>(visitor: Visitor<T>): T {
    return visitor.visit(this);
  }

}