import { writeFile } from "node:fs/promises";

type ClsDefResults = {
  filename: string;
  className: string;
  content: string;
};

type ExtraImport = {
  className: string;
  path: string;
};

type Options = { extraImports: ExtraImport[]; extend: boolean };

export function defineClass(
  baseName: string,
  className: string,
  fieldList: string[],
  extraImports: ExtraImport[],
  extend?: boolean,
): string {
  const ctorParamNameType: Iterable<[string, string]> = fieldList.map((f) => {
    const splits = f
      .split(" ")
      .map((s) => s.trim())
      .filter(Boolean);
    return [splits[1], splits[0]];
  });
  const argNames = Array.from(ctorParamNameType).map(([name]) => name);
  const paramTypes = Array.from(ctorParamNameType).map(([, type]) => type);
  const extraImportsClasses = extraImports.map(({ className }) => className);
  const imports = Array.from(
    new Set(
      paramTypes
        .map((type) => {
          const t = type.replace(/[[]]/, "");
          if (t.includes("|")) return t.split("|");
          return t;
        })
        .flat()
        .concat(extend ? ["Visitor"] : [baseName, "Visitor"])
        .filter((cls) => !extraImportsClasses.includes(cls))
        .sort((a, b) => a.localeCompare(b)),
    ),
  ).join(", ");
  const ctorParams = Array.from(ctorParamNameType)
    .map(([name, type]) => {
      let type_ = type;
      if (type.includes("|") && type.includes("[]")) {
        type_ = type
          .split(/([\w_|]+)/)
          .filter(Boolean)
          .map((v) => (v.includes("|") ? `(${v})` : v))
          .join("");
      }
      return `public readonly ${name}:${type_}`;
    })
    .join(", ");

  let cls = `import { ${imports} } from '../common'\n`;
  cls += extraImports
    .map(({ className, path }) => `import { ${className} } from '${path}'`)
    .join("\n");

  if (extraImports.length > 0) cls += "\n";

  cls += "\n";
  cls += `export class ${className} ${extend ? "extends" : "implements"} ${baseName} {
`;
  cls += extend
    ? `  constructor(${ctorParams}) { super(${argNames.join(",")}) }\n`
    : `  constructor(${ctorParams}) { }\n`;

  cls += `
  accept<T>(visitor: Visitor<T>): T {
    return visitor.visit(this);
  }\n
`;

  cls += "}";

  return cls;
}

export function defineClasses(
  baseName: string,
  def: string[],
  options?: Options,
): Promise<ClsDefResults>[] {
  return def.map(async (type): Promise<ClsDefResults> => {
    const [className, fieldList] = type.split(":");
    return {
      filename: className.trim().toLowerCase(),
      className,
      content: defineClass(
        baseName,
        className.trim(),
        fieldList?.split(",") ?? [],
        options?.extraImports ?? [],
        options?.extend ?? false,
      ),
    };
  });
}

export async function defineAst() {
  const defs: ClsDefResults[] = await Promise.all([
    ...defineClasses("Expr", [
      "Assign: Token name, Expr value",
      "Binary: Expr left, Token operator, Expr right",
      "Grouping : Expr expression",
      "Literal : Value value",
      "Unary : Token operator, Expr right",
      "Variable: Token name",
      "This: Token keyword",
    ]),
    ...defineClasses("Stmt", [
      "Expression: Expr expression",
      "Print: Expr expression",
      "Var: Token name, Expr initializer",
      "Block: Stmt[] statements",
      "If: Expr condition, Stmt thenBranch, Stmt elseBranch?",
      "Logical: Expr left, Token operator, Expr right",
      "While: Expr condition, Stmt body",
      "Break",
      "Continue",
      "Call: Expr callee, Token paren, Expr[] args",
      "Function: Token name, Token[] params, Stmt[] body, FunctionKind kind",
      "Return: Token keyword, Expr value",
      "Get: Expr object, Token name",
      "Set: Expr object, Token name, Expr value",
    ]),
    ...defineClasses("Stmt", ["Class: Token name, Function[] methods"], {
      extraImports: [{ className: "Function", path: "./function" }],
      extend: false,
    }),
  ]);

  defs.sort((a, b) => a.className.localeCompare(b.className));

  await Promise.all([
    defs.map(({ className, filename, content }) => {
      const path = `./src/ast/${filename}.ts`;
      console.log("Class", className, path);
      writeFile(path, content);
    }),
    (() => {
      const content = defs
        .map(({ filename }) => {
          return `export * from "./${filename}";`;
        })
        .join("\n");
      const path = "./src/ast/index.ts";
      return writeFile(path, content);
    })(),
  ]);
}

defineAst();
