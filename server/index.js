/* ייבוא של Express*/
//nmp install
//const mysql = require('mysql2');
const express = require('express');
const morgan = require('morgan');
const cors = require('cors');
const dotenv = require('dotenv');

/* יצירת אפליקציה ב-Express*/
const app = express();
/* הגדרת הפורט*/
const port = 3011;
//ייבוא את ראוטר שבנינו מחזיק את כל הראוטים שנבנה כל הCRUD 
const esp=require('./routes/esp');
//צריך לעבור בנתיב 
app.use('/esp',esp);


app.use(express.json());
app.use(cors());
app.use(morgan('dev'));
dotenv.config();

/* יצירת נתיב בסיסי לבדיקה*/
app.get('/',(req,res)=>{
   
})
app.listen(port,()=>{
    console.log("server is runing at http://localhost:"+port);
});