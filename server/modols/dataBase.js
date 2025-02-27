// const mysql = require('mysql2');

// const pool = mysql.createPool({
//     conectionLimit: 10,
//     host: process.env.HOST,
//     user: process.env.USER_DB,
//     password: process.env.PASSWORD_DB,
//     database: process.env.DATABASE
// });

// module.exports = pool.promise();

const mysql = require('mysql2');
require('dotenv').config();

const pool = mysql.createPool({
    connectionLimit: 10,
    host: process.env.HOST || 'localhost',
    user: process.env.USER_DB || 'root',
    password: process.env.PASSWORD_DB || '',
    database: process.env.DATABASE || 'plants_db'
});

module.exports = pool.promise();
