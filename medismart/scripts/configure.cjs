// Node.js 22.9+; no third-party dependencies.
const fs = require('node:fs');
const path = require('node:path');
const { parseEnv } = require('node:util');

const root = path.resolve(__dirname, '..');
const mode = process.argv.slice(2);
if (mode.length > 1 || (mode.length === 1 && !['--web', '--firmware'].includes(mode[0]))) {
  console.error('Usage: node scripts/configure.cjs [--web|--firmware]');
  process.exit(1);
}

try {
  const envPath = path.join(root, '.env');
  const local = fs.existsSync(envPath) ? parseEnv(fs.readFileSync(envPath, 'utf8')) : {};
  const env = { ...local, ...process.env };
  const web = mode[0] !== '--firmware';
  const firmware = mode[0] !== '--web';
  const fields = {
    apiKey: 'FIREBASE_API_KEY',
    authDomain: 'FIREBASE_AUTH_DOMAIN',
    databaseURL: 'FIREBASE_DATABASE_URL',
    projectId: 'FIREBASE_PROJECT_ID',
    storageBucket: 'FIREBASE_STORAGE_BUCKET',
    messagingSenderId: 'FIREBASE_MESSAGING_SENDER_ID',
    appId: 'FIREBASE_APP_ID',
  };
  const required = new Set([
    ...(web ? Object.values(fields) : []),
    ...(firmware ? ['WIFI_SSID', 'WIFI_PASS', 'FIREBASE_DATABASE_URL'] : []),
  ]);
  const missing = [...required].filter(key => !env[key]?.trim() || /^(replace-with-|your-)/.test(env[key]));
  if (missing.length) {
    throw new Error(`Set ${missing.join(', ')} in medismart/.env or the environment. See .env.example.`);
  }
  const database = new URL(env.FIREBASE_DATABASE_URL);
  if (database.protocol !== 'https:' || database.username || database.password || database.search || database.hash || !/^\/$/.test(database.pathname)) {
    throw new Error('FIREBASE_DATABASE_URL must be an HTTPS database root URL, without credentials or query parameters.');
  }
  if (firmware && (Buffer.byteLength(env.WIFI_SSID) > 32 || Buffer.byteLength(env.WIFI_PASS) > 64)) {
    throw new Error('Wi-Fi configuration exceeds the ESP32 SSID (32 bytes) or password (64 bytes) limit.');
  }

  // Validate everything before updating either generated file.
  if (web) {
    const config = Object.fromEntries(Object.entries(fields).map(([key, name]) => [key, env[name]]));
    const contents = '// Generated from environment settings. Do not commit.\n' +
      `if (!firebase.apps.length) firebase.initializeApp(${JSON.stringify(config, null, 2)});\n`;
    fs.writeFileSync(path.join(root, 'public', 'firebase-config.js'), contents);
    console.log('Generated public/firebase-config.js (Firebase web settings only).');
  }
  if (firmware) {
    // Octal escapes preserve UTF-8 bytes and avoid C escape/quote injection.
    const cString = value => '"' + [...Buffer.from(value)].map(byte => '\\' + byte.toString(8).padStart(3, '0')).join('') + '"';
    const base = database.origin;
    const contents = '// Generated from environment settings. Do not commit.\n#pragma once\n' +
      `#define WIFI_SSID ${cString(env.WIFI_SSID)}\n` +
      `#define WIFI_PASS ${cString(env.WIFI_PASS)}\n` +
      `#define FIREBASE_SETTINGS_URL ${cString(base + '/settings.json')}\n` +
      `#define FIREBASE_VITALS_URL ${cString(base + '/vitals.json')}\n`;
    fs.writeFileSync(path.join(root, 'medismartVscode', 'include', 'local_config.h'), contents);
    console.log('Generated medismartVscode/include/local_config.h.');
  }
} catch (error) {
  console.error(`Configuration failed: ${error.message}`);
  process.exit(1);
}
