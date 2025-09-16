#include "WebServer.h"
#include "hardware.h"
#include "LedMatrix.h"

#include <ESPAsyncWebServer.h>
#include <pgmspace.h>

// Simple single-page UI to visualize and control the matrix
// Served from PROGMEM to conserve RAM
static const char INDEX_HTML[] PROGMEM = R"HTMLEND(
<!doctype html>
<html lang="en">
  <head>
    <meta charset="utf-8" />
    <meta name="viewport" content="width=device-width, initial-scale=1" />
    <title>LED Matrix</title>
    <style>
      :root { --cell: 16px; --gap: 3px; --on: #21d07a; --off: #1b1f23; --grid: #2d333b; }
      * { box-sizing: border-box; }
      body { margin: 0; background: #0d1117; color: #c9d1d9;
             font-family: system-ui, -apple-system, Segoe UI, Roboto, sans-serif; }
      header { padding: 12px 16px; border-bottom: 1px solid #30363d; display: flex; align-items: center; gap: 12px; }
      h1 { font-size: 16px; margin: 0; font-weight: 600; }
      main { padding: 16px; }
      #status { font-size: 12px; opacity: .8; }
      #grid { display: grid; gap: var(--gap); background: #0d1117; padding: 8px; width: max-content;
              border: 1px solid #30363d; border-radius: 6px; box-shadow: inset 0 0 0 1px #161b22; }
      .cell { width: var(--cell); height: var(--cell); background: var(--off); border-radius: 3px;
              box-shadow: 0 0 0 1px var(--grid) inset; cursor: pointer; transition: background .05s ease; }
      .cell.on { background: var(--on); }
      #grid, .cell { user-select: none; touch-action: none; }
      .row { display: contents; }
      button { background: #238636; color: #fff; border: 1px solid #2ea043; padding: 6px 10px; border-radius: 6px; cursor: pointer; }
      button:disabled { opacity: .6; cursor: default; }
      .toolbar { display: flex; gap: 8px; align-items: center; }
      .spacer { flex: 1; }
    </style>
  </head>
  <body>
    <header>
      <h1>LED Matrix</h1>
      <div id="status">Connecting…</div>
      <div class="spacer"></div>
      <div class="toolbar">
        <button id="refresh">Refresh</button>
        <button id="clear" title="Turn off all pixels">Clear</button>
        <button id="fill" title="Turn on all pixels">Fill</button>
        <label style="font-size:12px;display:flex;align-items:center;gap:6px;">
          <input id="auto" type="checkbox" checked /> Auto-refresh
        </label>
        <label style="font-size:12px;display:flex;align-items:center;gap:8px;">
          <span style="opacity:.8;">Brightness</span>
          <input id="brightness" type="range" min="0" max="15" value="3" />
          <span id="bval">3</span>
        </label>
      </div>
    </header>
    <main>
      <div id="grid"></div>
    </main>
    <script>
      let cols = 0, rows = 0;
      let fb = [];
      let timer = null;
      let isDrawing = false;
      let drawOp = 'paint'; // 'paint' | 'erase'
      let hasMoved = false;
      let startX = 0, startY = 0;
      let drawn;
      let suppressNextClick = false; // used to suppress shift-click default click

      const $ = (s) => document.querySelector(s);
      const grid = $("#grid");
      const statusEl = $("#status");

      function setStatus(text) { statusEl.textContent = text; }

      async function getDisplay() {
        const r = await fetch('/display', { cache: 'no-store' });
        if (!r.ok) throw new Error('Failed to fetch display');
        return r.json();
      }

      function cellOn(x, y) {
        return ((fb[y] >>> x) & 1) === 1;
      }

      function setCell(x, y, on) {
        const mask = (1 << x);
        if (on) fb[y] |= mask; else fb[y] &= ~mask;
      }

      async function writePixel(x, y, on) {
        setCell(x, y, on);
        renderCells();
        try {
          const url = `/display?x=${x}&y=${y}&on=${on ? 1 : 0}`;
          const r = await fetch(url, { method: 'PUT' });
          if (!r.ok) throw new Error('PUT failed');
        } catch (e) {
          await refresh();
        }
      }

      async function togglePixel(x, y) {
        const on = !cellOn(x, y);
        await writePixel(x, y, on);
      }

      function renderSkeleton() {
        grid.style.gridTemplateColumns = `repeat(${cols}, var(--cell))`;
        grid.innerHTML = '';
        for (let y = 0; y < rows; y++) {
          for (let x = 0; x < cols; x++) {
            const d = document.createElement('div');
            d.className = 'cell';
            d.dataset.x = x;
            d.dataset.y = y;
            d.addEventListener('click', (e) => {
              if (suppressNextClick) { suppressNextClick = false; return; }
              togglePixel(x, y);
            });
            d.addEventListener('pointerdown', (e) => {
              isDrawing = true; hasMoved = false; startX = x; startY = y; drawn = new Set();
              drawOp = e.shiftKey ? 'erase' : 'paint';
              // For shift-click without movement, we'll erase on pointerup and suppress the click
              if (e.shiftKey) suppressNextClick = true;
            });
            d.addEventListener('pointerenter', async (e) => {
              if (!isDrawing) return;
              hasMoved = true;
              const key = y * cols + x;
              if (drawn.has(key)) return;
              drawn.add(key);
              await writePixel(x, y, drawOp === 'paint');
            });
            grid.appendChild(d);
          }
        }
      }

      function renderCells() {
        // Update classes without rebuilding DOM
        const children = grid.children;
        let i = 0;
        for (let y = 0; y < rows; y++) {
          for (let x = 0; x < cols; x++, i++) {
            const on = cellOn(x, y);
            const el = children[i];
            if (!el) continue;
            if (on) el.classList.add('on'); else el.classList.remove('on');
          }
        }
      }

      async function refresh() {
        try {
          setStatus('Updating…');
          const data = await getDisplay();
          let rebuild = false;
          if (data.columns !== cols || data.rows !== rows) {
            cols = data.columns; rows = data.rows; rebuild = true;
          }
          fb = data.framebuffer || [];
          if (rebuild || grid.childElementCount !== cols * rows) renderSkeleton();
          renderCells();
          setStatus(`${cols} × ${rows}`);
        } catch (e) {
          setStatus('Disconnected');
        }
      }

      function startAutoRefresh() {
        stopAutoRefresh();
        timer = setInterval(refresh, 1000);
      }
      function stopAutoRefresh() { if (timer) { clearInterval(timer); timer = null; } }

      $('#refresh').addEventListener('click', refresh);
      function endDraw(e) {
        if (!isDrawing) return;
        const wasShift = (e && e.shiftKey) || (drawOp === 'erase');
        if (!hasMoved) {
          if (wasShift) {
            // Shift-click erase
            suppressNextClick = true;
            writePixel(startX, startY, false);
          } else {
            // Plain click toggles (handled by click handler)
          }
        }
        isDrawing = false;
        hasMoved = false;
        drawn = null;
      }
      window.addEventListener('pointerup', endDraw);
      window.addEventListener('pointercancel', endDraw);
      async function clearAll() {
        try {
          setStatus('Clearing…');
          const r = await fetch('/display', { method: 'DELETE' });
          if (!r.ok) throw new Error('DELETE failed');
          fb = Array.from({ length: rows }, () => 0);
          renderCells();
          setStatus(`${cols} × ${rows}`);
        } catch (e) {
          await refresh();
        }
      }
      $('#clear').addEventListener('click', clearAll);
      // Fill all pixels ON
      async function fillAll() {
        try {
          setStatus('Filling…');
          const r = await fetch('/display/fill?on=1', { method: 'POST' });
          if (!r.ok) throw new Error('POST failed');
          fb = Array.from({ length: rows }, () => (cols >= 32 ? 0xFFFFFFFF : ((1 << cols) - 1)));
          renderCells();
          setStatus(`${cols} × ${rows}`);
        } catch (e) {
          await refresh();
        }
      }
      $('#fill').addEventListener('click', fillAll);
      $('#auto').addEventListener('change', (e) => {
        if (e.target.checked) startAutoRefresh(); else stopAutoRefresh();
      });
      // Brightness control
      async function fetchBrightness() {
        try {
          const r = await fetch('/brightness', { cache: 'no-store' });
          if (!r.ok) return;
          const j = await r.json();
          const v = Math.max(0, Math.min(15, j.brightness|0));
          const slider = document.getElementById('brightness');
          const bval = document.getElementById('bval');
          slider.value = v; bval.textContent = v;
        } catch {}
      }
      let bTimer = null;
      function scheduleSetBrightness(v) {
        const bval = document.getElementById('bval');
        bval.textContent = v;
        if (bTimer) clearTimeout(bTimer);
        bTimer = setTimeout(async () => {
          try {
            await fetch(`/brightness?value=${v}`, { method: 'PUT' });
          } catch {}
        }, 150);
      }
      document.addEventListener('input', (e) => {
        if (e.target && e.target.id === 'brightness') {
          scheduleSetBrightness(e.target.value);
        }
      });

      refresh();
      fetchBrightness();
      startAutoRefresh();
    </script>
  </body>
</html>
)HTMLEND";

WebServer::WebServer(Display* display, LedMatrix* ledMatrix)
  : display(display), ledMatrix(ledMatrix)
{
  asyncWebServer = new AsyncWebServer(80);

  asyncWebServer->on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send_P(200, "text/html", INDEX_HTML);
  });

  asyncWebServer->on("/hardware.json", HTTP_GET, [](AsyncWebServerRequest *request) {
    String response = "{";
    response += "\"columns\":";
    response += LED_MATRIX_COLS;
    response += ",\"rows\":";
    response += LED_MATRIX_ROWS;
    response += "}";
    request->send(200, "application/json", response);
  });

  // Return framebuffer as an array of row bitmasks
  asyncWebServer->on("/display", HTTP_GET, [this](AsyncWebServerRequest *request) {
    String response = "{";
    response += "\"columns\":";
    response += this->display->width();
    response += ",\"rows\":";
    response += this->display->height();
    response += ",\"framebuffer\":[";
    for (uint8_t y = 0; y < this->display->height(); y++) {
      if (y > 0) response += ",";
      response += (unsigned long)this->display->rowBits(y);
    }
    response += "]}";
    request->send(200, "application/json", response);
  });

  // Set a pixel: PUT /display?x=..&y=..&on=0|1 (also accepts body params)
  asyncWebServer->on("/display", HTTP_PUT, [this](AsyncWebServerRequest *request) {
    auto getParam = [&](const char* name) -> const AsyncWebParameter* {
      if (request->hasParam(name)) {
        return request->getParam(name);
      }
      if (request->hasParam(name, true)) {
        return request->getParam(name, true);
      }
      return nullptr;
    };

    const AsyncWebParameter* px = getParam("x");
    const AsyncWebParameter* py = getParam("y");
    const AsyncWebParameter* pon = getParam("on");
    if (!px || !py || !pon) {
      request->send(400, "application/json", "{\"error\":\"x, y, and on are required\"}");
      return;
    }

    int x = px->value().toInt();
    int y = py->value().toInt();
    String onStr = pon->value();
    onStr.toLowerCase();
    bool on = (onStr == "1" || onStr == "true" || onStr == "on");

    if (x < 0 || y < 0 || x >= this->display->width() || y >= this->display->height()) {
      request->send(400, "application/json", "{\"error\":\"x or y out of range\"}");
      return;
    }

    bool changed = this->display->setPixel((uint8_t)x, (uint8_t)y, on);
    String json = "{";
    json += "\"x\":"; json += x; json += ",";
    json += "\"y\":"; json += y; json += ",";
    json += "\"on\":"; json += (on ? "true" : "false"); json += ",";
    json += "\"changed\":"; json += (changed ? "true" : "false");
    json += "}";
    request->send(200, "application/json", json);
  });

  // Fill all pixels on/off: POST /display/fill?on=0|1 (default 1)
  asyncWebServer->on("/display/fill", HTTP_POST, [this](AsyncWebServerRequest *request) {
    bool on = true;
    if (request->hasParam("on")) {
      String v = request->getParam("on")->value();
      v.toLowerCase();
      on = (v == "1" || v == "true" || v == "on");
    } else if (request->hasParam("on", true)) {
      String v = request->getParam("on", true)->value();
      v.toLowerCase();
      on = (v == "1" || v == "true" || v == "on");
    }
    this->display->fill(on);
    request->send(200, "application/json", String("{\"filled\":" ) + (on ? "true" : "false") + "}");
  });

  // Clear all pixels: DELETE /display
  asyncWebServer->on("/display", HTTP_DELETE, [this](AsyncWebServerRequest *request) {
    this->display->clear();
    request->send(200, "application/json", "{\"cleared\":true}");
  });

  // Brightness endpoints
  // GET /brightness -> {"brightness":0..15}
  asyncWebServer->on("/brightness", HTTP_GET, [this](AsyncWebServerRequest *request) {
    String json = "{";
    json += "\"brightness\":"; json += (int)this->ledMatrix->intensity();
    json += "}";
    request->send(200, "application/json", json);
  });
  // PUT /brightness?value=0..15 (also accepts body param)
  asyncWebServer->on("/brightness", HTTP_PUT, [this](AsyncWebServerRequest *request) {
    auto getParam = [&](const char* name) -> const AsyncWebParameter* {
      if (request->hasParam(name)) {return request->getParam(name);}
      if (request->hasParam(name, true)) {return request->getParam(name, true);}
      return nullptr;
    };
    const AsyncWebParameter* pv = getParam("value");
    if (!pv) {
      request->send(400, "application/json", "{\"error\":\"value is required\"}");
      return;
    }
    int v = pv->value().toInt();
    if (v < LED_MATRIX_BRIGHTNESS_MIN) {
      v = LED_MATRIX_BRIGHTNESS_MIN;
    } else if (v > LED_MATRIX_BRIGHTNESS_MAX) {
      v = LED_MATRIX_BRIGHTNESS_MAX;
    }
    this->ledMatrix->setIntensity((uint8_t)v);
    String json = "{";
    json += "\"brightness\":"; json += v; json += "}";
    request->send(200, "application/json", json);
  });

  asyncWebServer->begin();
  Serial.println("HTTP server started");
}
