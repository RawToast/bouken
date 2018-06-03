FROM node:8.9.4

RUN npm install -g serve react-scripts

COPY package.json yarn.lock bsconfig.json  ./

RUN yarn >/dev/null 2>&1

COPY public ./public

COPY src ./src

RUN yarn --silent && npm link bs-platform && yarn build

EXPOSE 3000

CMD ["serve", "-s", "build", "-l", "3000"]
