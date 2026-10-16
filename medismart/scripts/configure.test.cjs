const assert = require('node:assert/strict');
const fs = require('node:fs');
const os = require('node:os');
const path = require('node:path');
const vm = require('node:vm');
const { spawnSync } = require('node:child_process');
const { test } = require('node:test');

function fixture(t) {
  const root = fs.mkdtempSync(path.join(os.tmpdir(), 'medismart-config-test-'));
  t.after(() => fs.rmSync(root, { recursive: true, force: true }));
  for (const directory of ['scripts', 'public', 'medismartVscode/include']) {
    fs.mkdirSync(path.join(root, directory), { recursive: true });
  }
  fs.copyFileSync(path.join(__dirname, 'configure.cjs'), path.join(root, 'scripts/configure.cjs'));
  const settings = {
    FIREBASE_API_KEY: 'test-browser-key',
    FIREBASE_AUTH_DOMAIN: 'example.firebaseapp.com',
    FIREBASE_DATABASE_URL: 'https://example-default-rtdb.firebaseio.com/',
    FIREBASE_PROJECT_ID: 'example',
    FIREBASE_STORAGE_BUCKET: 'example.firebasestorage.app',
    FIREBASE_MESSAGING_SENDER_ID: '123456789',
    FIREBASE_APP_ID: 'test-app-id',
    WIFI_SSID: 'test-network',
    WIFI_PASS: 'test-password',
  };
  const env = Object.fromEntries(Object.entries(process.env).filter(([key]) => !/^(FIREBASE_|WIFI_)/.test(key)));
  return {
    settings,
    writeEnv(values = settings) {
      fs.writeFileSync(path.join(root, '.env'), Object.entries(values).map(([key, value]) => `${key}='${value}'`).join('\n'));
    },
    run(args = [], overrides = {}) {
      return spawnSync(process.execPath, [path.join(root, 'scripts/configure.cjs'), ...args], {
        cwd: os.tmpdir(), env: { ...env, ...overrides }, encoding: 'utf8',
      });
    },
    read(file) { return fs.readFileSync(path.join(root, file), 'utf8'); },
    exists(file) { return fs.existsSync(path.join(root, file)); },
  };
}

test('generates usable browser config without Wi-Fi credentials and preserves firmware strings', t => {
  const f = fixture(t);
  f.settings.WIFI_SSID = 'network "quoted" \\ café';
  f.writeEnv();
  const result = f.run();
  assert.equal(result.status, 0, result.stderr);
  const browser = f.read('public/firebase-config.js');
  assert.ok(!browser.includes('WIFI_') && !browser.includes(f.settings.WIFI_PASS));
  const calls = [];
  const firebase = { apps: [], initializeApp(config) { calls.push(config); this.apps.push(config); } };
  vm.runInNewContext(browser, { firebase });
  vm.runInNewContext(browser, { firebase });
  assert.equal(calls.length, 1);
  assert.equal(calls[0].apiKey, f.settings.FIREBASE_API_KEY);
  assert.equal(Object.keys(calls[0]).length, 7);
  const header = f.read('medismartVscode/include/local_config.h');
  const decode = name => Buffer.from([...header.match(new RegExp(`#define ${name} "([^"]*)"`))[1].matchAll(/\\([0-7]{3})/g)].map(match => parseInt(match[1], 8))).toString();
  assert.equal(decode('WIFI_SSID'), f.settings.WIFI_SSID);
  assert.equal(decode('WIFI_PASS'), f.settings.WIFI_PASS);
  assert.equal(decode('FIREBASE_SETTINGS_URL'), 'https://example-default-rtdb.firebaseio.com/settings.json');
  assert.equal(decode('FIREBASE_VITALS_URL'), 'https://example-default-rtdb.firebaseio.com/vitals.json');
});

test('web mode supports CI environment overrides without requiring Wi-Fi settings', t => {
  const f = fixture(t);
  delete f.settings.WIFI_SSID;
  delete f.settings.WIFI_PASS;
  f.writeEnv();
  const result = f.run(['--web'], { FIREBASE_API_KEY: 'ci-key' });
  assert.equal(result.status, 0, result.stderr);
  assert.ok(f.read('public/firebase-config.js').includes('ci-key'));
  assert.ok(!f.exists('medismartVscode/include/local_config.h'));
});

test('firmware mode does not require browser settings or write browser config', t => {
  const f = fixture(t);
  f.writeEnv(Object.fromEntries(Object.entries(f.settings).filter(([key]) => key.startsWith('WIFI_') || key === 'FIREBASE_DATABASE_URL')));
  const result = f.run(['--firmware']);
  assert.equal(result.status, 0, result.stderr);
  assert.ok(f.exists('medismartVscode/include/local_config.h'));
  assert.ok(!f.exists('public/firebase-config.js'));
});

test('invalid or incomplete settings fail before writing files and do not print values', t => {
  const f = fixture(t);
  for (const overrides of [
    { FIREBASE_API_KEY: '' },
    { FIREBASE_API_KEY: 'replace-with-your-key' },
    { FIREBASE_DATABASE_URL: 'https://example.com/?auth=private-test-token' },
    { WIFI_SSID: 'x'.repeat(33) },
  ]) {
    f.writeEnv({ ...f.settings, ...overrides });
    const result = f.run();
    assert.equal(result.status, 1);
    assert.ok(!result.stderr.includes('private-test-token'));
    assert.ok(!f.exists('public/firebase-config.js'));
    assert.ok(!f.exists('medismartVscode/include/local_config.h'));
  }
});
