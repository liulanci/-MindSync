const mysql = require('mysql2/promise');
const config = require('../config');

let pool = null;

function getPool() {
  if (!pool) {
    pool = mysql.createPool(config.db);
  }
  return pool;
}

async function query(sql, params) {
  const [rows] = await getPool().execute(sql, params);
  return rows;
}

async function queryOne(sql, params) {
  const rows = await query(sql, params);
  return rows[0] || null;
}

async function insert(sql, params) {
  const [result] = await getPool().execute(sql, params);
  return result;
}

async function update(sql, params) {
  const [result] = await getPool().execute(sql, params);
  return result;
}

async function remove(sql, params) {
  const [result] = await getPool().execute(sql, params);
  return result;
}

async function transaction(callback) {
  const conn = await getPool().getConnection();
  try {
    await conn.beginTransaction();
    const result = await callback(conn);
    await conn.commit();
    return result;
  } catch (err) {
    await conn.rollback();
    throw err;
  } finally {
    conn.release();
  }
}

module.exports = { getPool, query, queryOne, insert, update, remove, transaction };
