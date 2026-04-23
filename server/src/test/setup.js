const db = require('../config/database');

beforeAll(async () => {
  await db.query('CREATE DATABASE IF NOT EXISTS test_clipboard_sync');
  await db.query('USE test_clipboard_sync');
});

afterAll(async () => {
  await db.query('DROP DATABASE IF EXISTS test_clipboard_sync');
  const pool = db.getPool();
  if (pool) await pool.end();
});

beforeEach(async () => {
  await db.query('SET FOREIGN_KEY_CHECKS = 0');
  const tables = await db.query('SHOW TABLES');
  for (const table of tables) {
    const tableName = Object.values(table)[0];
    // eslint-disable-next-line no-await-in-loop
    await db.query(`TRUNCATE TABLE ${tableName}`);
  }
  await db.query('SET FOREIGN_KEY_CHECKS = 1');
});
