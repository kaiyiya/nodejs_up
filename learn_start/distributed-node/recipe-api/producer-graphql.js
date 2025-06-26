#!/usr/bin/env node
const server = require("fastify")();
const graphql = require("fastify-gql");
const fs = require("fs");
console.log(__dirname);

const schema = fs.readFileSync("../shared/graphql-schema.gql").toString();

const HOST = process.env.HOST || "127.0.0.1";
const PORT = process.env.PORT || 4000;

const resolvers = {
  Query: {
    pid: () => process.pid,
    recipe: async (_obj, { id }) => {
      if (id != 42) throw new Error(`recipe ${id} not found`);
      return {
        id,
        name: "ChickenTiiiii",
        steps: "let sup",
      };
    },
  },
  Recipe: {
    ingredients: async (obj) => {
      return obj.id != 42
        ? []
        : [
            { id: 1, name: "chkcken", quantity: "1 lb" },
            { id: 2, name: "Sauce", quantity: "2 cups" },
          ];
    },
  },
};

server
  .register(graphql, { schema, resolvers, graphiql: true })
  .listen(PORT, HOST, () => {
    console.log("启动啦");
  });
