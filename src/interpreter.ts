import Assign from "./ast/assign";
import { Expr, Stmt, Visitor } from "./ast/base";
import Binary from "./ast/binary";
import Expression from "./ast/expression";
import Grouping from "./ast/grouping";
import Literal from "./ast/literal";
import Print from "./ast/print";
import Unary from "./ast/unary";
import Var from "./ast/var";
import Variable from "./ast/variable";
import { RuntimeError, Token, TokenType, Value } from "./common";
import Environment from "./environment";

export default class Interpreter implements Visitor<Value> {
  environment = new Environment();

  constructor(protected readonly printer: (...args: unknown[]) => void) {}

  protected stringfy(value: Value): string {
    if (value == null) return "nil";
    if (typeof value == "number") {
      // FIXME:
      let text = value.toString();
      if (text.endsWith(".0")) {
        text = text.substring(0, text.length - 2);
      }
      return text;
    }
    return value.toString();
  }

  protected isTruthy(value: Value): boolean {
    if (value == "null") return false;
    if (typeof value == "boolean") return value;
    return true;
  }

  protected operator(lhs: Value, rhs: Value, operator: Token): Value {
    switch (operator.type) {
      case TokenType.MINUS:
        if (typeof lhs == "number" && typeof rhs == "number") {
          return lhs - rhs;
        }
        break;
      case TokenType.SLASH:
        if (typeof lhs == "number" && typeof rhs == "number") {
          return lhs / rhs;
        }
        break;
      case TokenType.STAR:
        if (typeof lhs == "number" && typeof rhs == "number") {
          return lhs * rhs;
        }
        break;
      case TokenType.PLUS:
        if (typeof lhs == "number" && typeof rhs == "number") {
          return lhs + rhs;
        } else if (typeof lhs == "string" && typeof rhs == "string") {
          return lhs + rhs;
        }
        break;
      case TokenType.GREATER:
        if (typeof lhs == "number" && typeof rhs == "number") {
          return lhs > rhs;
        }
        break;
      case TokenType.GREATER_EQUAL:
        if (typeof lhs == "number" && typeof rhs == "number") {
          return lhs >= rhs;
        }
        break;
      case TokenType.LESS:
        if (typeof lhs == "number" && typeof rhs == "number") {
          return lhs < rhs;
        }
        break;
      case TokenType.LESS_EQUAL:
        if (typeof lhs == "number" && typeof rhs == "number") {
          return lhs <= rhs;
        }
        break;
      case TokenType.BANG_EQUAL:
        if (typeof lhs == "number" && typeof rhs == "number") {
          return !(lhs == rhs);
        }
        break;
      case TokenType.EQUAL_EQUAL:
        if (typeof lhs == "number" && typeof rhs == "number") {
          return lhs == rhs;
        }
        break;
    }
    throw new RuntimeError(
      operator,
      `Invalid values  lhs=${lhs}, rhs=${rhs} for operator ${operator.lexeme}`,
    );
  }

  visit(eOs: Expr | Stmt): Value {
    if (eOs instanceof Literal) {
      return eOs.value;
    } else if (eOs instanceof Grouping) {
      return this.evaluate(eOs);
    } else if (eOs instanceof Unary) {
      const right = this.evaluate(eOs.right);
      if (right && eOs.operator.type == TokenType.MINUS) {
        return -right;
      } else if (right && eOs.operator.type == TokenType.BANG) {
        return !this.isTruthy(right);
      }
      return null;
    } else if (eOs instanceof Binary) {
      const left = this.evaluate(eOs.left);
      const right = this.evaluate(eOs.right);
      return this.operator(left, right, eOs.operator);
    } else if (eOs instanceof Expression) {
      this.evaluate(eOs);
    } else if (eOs instanceof Var) {
      let val: Value = null;
      if (eOs.initializer) {
        val = this.evaluate(eOs.initializer);
      }
      if (!eOs.name.lexeme) {
        // FIXME: interpreter error?
        throw new RuntimeError(eOs.name, "var without a lexeme");
      }
      this.environment.define(eOs.name, val);
    } else if (eOs instanceof Print) {
      const val = this.evaluate(eOs.expression);
      this.printer(this.stringfy(val));
    } else if (eOs instanceof Variable) {
      return this.environment.getToken(eOs.name);
    } else if (eOs instanceof Assign) {
      const value = this.evaluate(eOs.value);
      this.environment.assign(eOs.name, value);
      return value;
    }
    return null;
  }

  protected evaluate(eOs: Expr | Stmt): Value {
    return eOs.accept(this);
  }

  protected execute(stmt: Stmt): void {
    stmt.accept(this);
  }

  interpret(statements: Stmt[]): void {
    try {
      for (const stmt of statements) {
        this.execute(stmt);
      }
    } catch (error) {
      // FIXME:
      console.error(error);
    }
  }
}
