import Binary from "./ast/binary";
import { Expr, Visitor } from "./ast/base";
import Grouping from "./ast/grouping";
import Literal from "./ast/literal";
import Unary from "./ast/unary";

export class AstPrinter implements Visitor<string> {
  visit(value: Expr): string {
    if (value instanceof Binary) {
      return this.parenthesize(value.operator.lexeme!, value.left, value.right);
    } else if (value instanceof Grouping) {
      return this.parenthesize("group", value.expression);
    } else if (value instanceof Literal) {
      if (!value.value) return "nil";
      return value.value.toString();
    } else if (value instanceof Unary) {
      return this.parenthesize(value.operator.lexeme!, value.right);
    }
    return "";
  }

  print(expr: Expr): string {
    return this.visit(expr);
  }

  protected parenthesize(name: string, ...expr: Expr[]): string {
    let results = `(${name}`;
    for (const ep of expr) {
      results += " ";
      results += ep.accept<string>(this);
    }
    results += ")";
    return results;
  }
}
