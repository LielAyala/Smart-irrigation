
// ייבוא של Express וספריות נוספות
const express = require('express');
const morgan = require('morgan');
const cors = require('cors');
const dotenv = require('dotenv');
const bodyParser = require('body-parser');
const path = require('path'); // ✅ תיקון השגיאה - הוספת המודול path

dotenv.config(); // טעינת משתני סביבה

const app = express();
const port = process.env.PORT || 3011;


app.use(bodyParser.json()); // מאפשר לקרוא JSON בבקשות
app.use(bodyParser.urlencoded({ extended: true })); // תומך בנתונים מקודדים
app.use(cors());
app.use(morgan('dev'));
app.use(express.json()); // מאפשר קבלת JSON בבקשות POST

// ייבוא הראוטים
const esp = require('./routes/esp');
const tree = require('./routes/treeRout');
// שימוש בנתיבים
app.use('/esp', esp);
app.use('/tree', tree);

// הפעלת שירות קבצים סטטיים מהתיקייה `../public`
app.use(express.static(path.resolve(__dirname, '../public')));


// הפניית כל הבקשות לדף `index.html` כברירת מחדל
app.get('/', (req, res) => {
    //res.send('Server is running!');
    res.sendFile(path.resolve(__dirname, '../public', 'index.html'));
});

app.listen(port, () => {
    console.log(`✅ Server is running at http://localhost:${port}`);
});
