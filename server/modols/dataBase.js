
const mysql = require('mysql2');
require('dotenv').config();

const pool = mysql.createPool({
    connectionLimit: 10,
    host: process.env.HOST || 'localhost',
    user: process.env.USER_DB || 'root',
    password: process.env.PASSWORD_DB || '',
    database: process.env.DATABASE || 'smart-irrigation'
});

module.exports = pool.promise();
