FROM node:20-alpine

LABEL org.opencontainers.image.version="0.97.45" org.opencontainers.image.source="https://github.com/EmanuelCatania/Mu-Linux"

WORKDIR /app

COPY package.json package-lock.json* ./
RUN npm install --omit=dev

COPY server.js ./

ENV PORT=8090
EXPOSE 8090

CMD ["npm", "start"]
