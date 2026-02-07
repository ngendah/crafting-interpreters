export interface Expr {
  accept<T>(visitor: Visitor<T>): T;
}

export interface Stmt {
  accept<T>(visitor: Visitor<T>): T;
}

export interface Visitor<T> {
  visit(value: Expr | Stmt): T;
}
