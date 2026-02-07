import { writeFile } from "node:fs/promises";

export function defineClass(
  baseName: string,
  className: string,
  fieldList: string[],
  imports: string[],
): string {
  let cls = imports.join("\n");

  cls += "\n\n";
  cls += `export default class ${className} implements ${baseName} {
`;
  const ctorParams = fieldList
    .map((f) => {
      const splits = f
        .split(" ")
        .map((s) => s.trim())
        .filter(Boolean);
      return `public readonly ${splits[1]}:${splits[0]}`;
    })
    .join(",");

  cls += `  constructor(${ctorParams}) { }\n`;

  cls += `
  accept<T>(visitor: Visitor<T>): T {
    return visitor.visit(this);
  }\n
`;

  cls += "}";

  return cls;
}

type ClsDef = {
  type: string;
  imports: string[];
};

type ClsDefResults = {
  className: string;
  content: string;
};

export function defineClasses(
  baseName: string,
  def: ClsDef[],
): Promise<ClsDefResults>[] {
  return def.map(async ({ type, imports }): Promise<ClsDefResults> => {
    const [className, fieldList] = type.split(":");
    return {
      className: className.trim().toLowerCase(),
      content: defineClass(
        baseName,
        className.trim(),
        fieldList.split(","),
        imports,
      ),
    };
  });
}

export async function defineAst() {
  const defs: ClsDefResults[] = await Promise.all([
    ...defineClasses("Expr", [
      {
        type: "Assign: Token name, Expr value",
        imports: [
          'import { Expr, Visitor } from "./base"',
          'import { Token } from "../common"',
        ],
      },
      {
        type: "Binary: Expr left, Token operator, Expr right",
        imports: [
          'import { Expr, Visitor } from "./base"',
          'import { Token } from "../common"',
        ],
      },
      {
        type: "Grouping : Expr expression",
        imports: ['import { Expr, Visitor } from "./base"'],
      },
      {
        type: "Literal : string|number|boolean|null value",
        imports: ['import { Expr, Visitor } from "./base"'],
      },
      {
        type: "Unary : Token operator, Expr right",
        imports: [
          'import { Expr, Visitor } from "./base"',
          'import { Token } from "../common"',
        ],
      },
      {
        type: "Variable: Token name",
        imports: [
          'import { Expr, Visitor } from "./base"',
          'import { Token } from "../common"',
        ],
      },
    ]),
    ...defineClasses("Stmt", [
      {
        type: "Expression: Expr expression",
        imports: ['import { Expr, Stmt, Visitor } from "./base"'],
      },
      {
        type: "Print: Expr expression",
        imports: ['import { Expr, Stmt, Visitor } from "./base"'],
      },
      {
        type: "Var: Token name, Expr initializer",
        imports: [
          'import { Expr, Stmt, Visitor } from "./base"',
          'import { Token } from "../common"',
        ],
      },
    ]),
  ]);

  await Promise.all(
    defs.map((cls) => writeFile(`./src/ast/${cls.className}.ts`, cls.content)),
  );
}

defineAst();
