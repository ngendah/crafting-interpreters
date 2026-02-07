import { Expr, Visitor } from "./base"

export default class Literal implements Expr {
  constructor(public readonly value:string|number|boolean|null) { }

  accept<T>(visitor: Visitor<T>): T {
    return visitor.visit(this);
  }

}