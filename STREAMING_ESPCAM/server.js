const express = require("express");
const http = require("http");
const WebSocket = require("ws");
const path = require("path");

const app = express();
const server = http.createServer(app);

// Serve nothing but our static HTML/JS
app.use(express.static(path.join(__dirname, "public")));

// — Ingest WebSocket (ESP32 → here) on port 8080, path /esp32
let latestFrame = null;
const ingestWss = new WebSocket.Server({ port: 8080, path: "/esp32" });
ingestWss.on("connection", (ws) => {
  console.log("📸 ESP32 connected");
  ws.on("message", (data) => {
    latestFrame = data; // raw JPEG bytes (Buffer)
    console.log("📥 Ingest got frame, bytes:", data.length);
    // broadcast immediately to all viewers
    viewerWss.clients.forEach((c) => {
      if (c.readyState === WebSocket.OPEN) {
        c.send(latestFrame);
        console.log("⚡️ Broadcast to viewer, bytes:", data.length);
      }
    });
  });
  ws.on("close", () => console.log("❌ ESP32 disconnected"));
});
console.log("🚀 Ingest WS on ws://<your-ip>:8080/esp32");

// — Viewer WebSocket (browser → here) on port 3000, path /stream
const viewerWss = new WebSocket.Server({ server, path: "/stream" });
viewerWss.on("connection", (ws) => {
  console.log("🖥️ Viewer connected");
  if (latestFrame) {
    ws.send(latestFrame);
    console.log("📤 Sent stored frame to new viewer");
  }
  ws.on("close", () => console.log("👋 Viewer disconnected"));
});

// Start HTTP+WS for viewers
server.listen(3000, () => {
  console.log("🌐 HTTP + Stream WS on http://<your-ip>:3000");
});
