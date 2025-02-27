// /* ייבוא של Express*/
// //nmp install
// //const mysql = require('mysql2');
// const express = require('express');
// const morgan = require('morgan');
// const cors = require('cors');
// const dotenv = require('dotenv');

// /* יצירת אפליקציה ב-Express*/
// const app = express();
// /* הגדרת הפורט*/
// const port = 3011;
// //ייבוא את ראוטר שבנינו מחזיק את כל הראוטים שנבנה כל הCRUD 
// const esp=require('./routes/esp');
// const Tree=require('./routes/treeRout');
// //צריך לעבור בנתיב 
// app.use('/esp',esp);
// app.use('/tree',Tree);


// app.use(express.json());http://10.9.1.83:3011/esp?
// app.use(cors());
// app.use(morgan('dev'));
// dotenv.config();

// /* יצירת נתיב בסיסי לבדיקה*/
// app.get('/',(req,res)=>{
//     console.log("הגעת לנתיב הראשי");  // להוסיף הודעה למסוף
//     res.send('Server is running!');
// })
// app.get('/tree',(req,res)=>{

// })
// app.listen(port,()=>{
//     console.log("server is runing at http://localhost:"+port);
// });


const express = require('express');
const morgan = require('morgan');
const cors = require('cors');
const dotenv = require('dotenv');

dotenv.config(); // טעינת משתני סביבה

const app = express();
const port = process.env.PORT || 3011;

// ייבוא הראוטים
const esp = require('./routes/esp');
const tree = require('./routes/treeRout');

app.use(express.json());
app.use(cors());
app.use(morgan('dev'));

// שימוש בנתיבים
app.use('/esp', esp);
app.use('/tree', tree);

app.get('/', (req, res) => {
    res.send('Server is running!');
});

app.listen(port, () => {
    console.log(`✅ Server is running at http://localhost:${port}`);
});
