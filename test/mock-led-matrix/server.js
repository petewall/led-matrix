#!/usr/bin/env node
'use strict';

const express = require('express');
const path = require('path');

const app = express();
app.use(express.json());

const DEFAULT_COLS = parseInt(process.env.MOCK_LED_MATRIX_COLS || '32', 10);
const DEFAULT_ROWS = parseInt(process.env.MOCK_LED_MATRIX_ROWS || '32', 10);
const DEFAULT_BRIGHTNESS = parseInt(process.env.MOCK_LED_MATRIX_BRIGHTNESS || '3', 10);
const MIN_BRIGHTNESS = 0;
const MAX_BRIGHTNESS = 15;
const PORT = parseInt(process.env.PORT || process.env.MOCK_LED_MATRIX_PORT || '4000', 10);

const visualizations = [
  { id: 'clock', label: 'Clock' },
  { id: 'columns', label: 'Columns' },
  { id: 'text', label: 'Text' },
];

let columns = Number.isFinite(DEFAULT_COLS) && DEFAULT_COLS > 0 ? DEFAULT_COLS : 32;
let rows = Number.isFinite(DEFAULT_ROWS) && DEFAULT_ROWS > 0 ? DEFAULT_ROWS : 32;
let framebuffer = Array.from({ length: rows }, () => 0);
let brightness = Math.max(MIN_BRIGHTNESS, Math.min(MAX_BRIGHTNESS, DEFAULT_BRIGHTNESS));
let currentVisualization = visualizations[0].id;

function setPixel(x, y, on) {
  if (x < 0 || y < 0 || x >= columns || y >= rows) {
    return false;
  }
  const mask = 1 << x;
  const before = framebuffer[y];
  if (on) {
    framebuffer[y] = before | mask;
  } else {
    framebuffer[y] = before & ~mask;
  }
  return framebuffer[y] !== before;
}

function fillDisplay(on) {
  const value = on ? ((columns >= 32 ? 0xFFFFFFFF : (1 << columns) - 1)) : 0;
  framebuffer = Array.from({ length: rows }, () => value);
}

function resetDisplay() {
  framebuffer = Array.from({ length: rows }, () => 0);
}

function getParam(req, name) {
  if (Object.prototype.hasOwnProperty.call(req.query, name)) {
    return req.query[name];
  }
  if (req.body && Object.prototype.hasOwnProperty.call(req.body, name)) {
    return req.body[name];
  }
  return undefined;
}

app.get('/hardware.json', (_req, res) => {
  res.json({ columns, rows });
});

app.get('/display', (_req, res) => {
  res.json({ columns, rows, framebuffer });
});

app.put('/display', (req, res) => {
  const x = parseInt(getParam(req, 'x'), 10);
  const y = parseInt(getParam(req, 'y'), 10);
  const onParam = getParam(req, 'on');
  const onStr = typeof onParam === 'string' ? onParam.toLowerCase() : onParam;
  const on = onStr === 1 || onStr === '1' || onStr === 'true' || onStr === true || onStr === 'on';

  if (!Number.isInteger(x) || !Number.isInteger(y) || typeof onParam === 'undefined') {
    return res.status(400).json({ error: 'x, y, and on are required' });
  }
  if (x < 0 || y < 0 || x >= columns || y >= rows) {
    return res.status(400).json({ error: 'x or y out of range' });
  }

  const changed = setPixel(x, y, on);
  res.json({ x, y, on, changed });
});

app.post('/display/fill', (req, res) => {
  const onParam = getParam(req, 'on');
  const onStr = typeof onParam === 'string' ? onParam.toLowerCase() : onParam;
  const on = onStr === undefined || onStr === 1 || onStr === '1' || onStr === 'true' || onStr === true || onStr === 'on';
  fillDisplay(on);
  res.json({ filled: on });
});

app.delete('/display', (_req, res) => {
  resetDisplay();
  res.json({ cleared: true });
});

app.get('/brightness', (_req, res) => {
  res.json({ brightness });
});

app.put('/brightness', (req, res) => {
  const valueParam = getParam(req, 'value');
  if (typeof valueParam === 'undefined') {
    return res.status(400).json({ error: 'value is required' });
  }
  const value = Math.trunc(Number(valueParam));
  if (!Number.isFinite(value)) {
    return res.status(400).json({ error: 'value must be numeric' });
  }
  brightness = Math.max(MIN_BRIGHTNESS, Math.min(MAX_BRIGHTNESS, value));
  res.json({ brightness });
});

app.get('/visualizations', (_req, res) => {
  res.json({ current: currentVisualization, visualizations });
});

app.post('/visualizations', (req, res) => {
  const idParam = getParam(req, 'id');
  if (typeof idParam === 'undefined') {
    return res.status(400).json({ error: 'id is required' });
  }
  const id = String(idParam);
  const found = visualizations.find((viz) => viz.id === id);
  if (!found) {
    return res.status(404).json({ error: 'visualization not found' });
  }
  currentVisualization = id;
  res.json({ current: currentVisualization });
});

const staticRoot = path.resolve(__dirname, '../../data');
app.use('/', express.static(staticRoot));

app.use((err, _req, res, _next) => {
  /* eslint-disable no-console */
  console.error('Mock LED matrix server error:', err);
  /* eslint-enable no-console */
  res.status(500).json({ error: 'internal error' });
});

function start(port = PORT) {
  const server = app.listen(port, () => {
    const address = server.address();
    const host = typeof address === 'string' ? address : `http://localhost:${address.port}`;
    /* eslint-disable no-console */
    console.log(`Mock LED matrix running at ${host}`);
    console.log(`Serving static assets from ${staticRoot}`);
    /* eslint-enable no-console */
  });
  return server;
}

if (require.main === module) {
  start();
}

module.exports = { app, start };
